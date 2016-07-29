#include "match_points.hpp"

cv::Ptr<cv::Feature2D> feature = cv::AKAZE::create();
cv::BFMatcher matcher(feature->defaultNorm(), true);

CapturedFrame::CapturedFrame() {
    std::cout << "Created empty CapturedFrame." << std::endl;
}

CapturedFrame::CapturedFrame(const cv::Mat &frame) {
    originalFrame = frame;
    getContoured();
    writeImage(contouredFrame, "contoured");

    std::vector<cv::KeyPoint> tmpVec;
    feature->detect(contouredFrame, tmpVec);
    std::cout << "Reducing keypoints: " << tmpVec.size() << std::endl;
    for (auto key: tmpVec) {
        if (1 < key.octave) {
            detected.keypoints.push_back(key);
        }
    }
    std::cout << "Computing keypoints: " << detected.keypoints.size() << std::endl;
    feature->compute(contouredFrame, detected.keypoints, detected.desc);
}

bool CapturedFrame::isEmpty() {
    return originalFrame.rows == 0;
}

void CapturedFrame::getContoured() {
    const int cannyThresh = 100;
    const auto blurSize = cv::Size(3, 3);
    const auto contoursBG = cv::Scalar(0);
    const auto contoursFG = cv::Scalar(255);

    cv::Mat grayFrame, edgedFrame;
    cv::cvtColor(originalFrame, grayFrame, cv::COLOR_BGR2GRAY);
    cv::blur(grayFrame, grayFrame, blurSize);
    cv::Canny(grayFrame, edgedFrame, cannyThresh, cannyThresh * 2);

    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(edgedFrame, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    std::cout << "Found contours: " << contours.size() << std::endl;

    contouredFrame = cv::Mat(grayFrame.size(), grayFrame.type(), contoursBG);
    for (int i = 0; i >= 0; i = hierarchy[i][0]) {
        cv::drawContours(contouredFrame, contours, i, contoursFG);
    }
}

Spot::Spot(const int startIndex, const cv::KeyPoint &key) {
    startFrame = startIndex - 1;
    addPoint(key);
    first = last;
}

int Spot::start() const {
    return startFrame;
}

int Spot::end() const {
    return startFrame + keypoints.size() - 1;
}

const geometry::Point_2 &Spot::getLastPoint() const {
    return last;
}

geometry::Vector_2 Spot::movement() const {
    return last - first;
}

void Spot::addPoint(const cv::KeyPoint &key) {
    keypoints.push_back(key);
    last = geometry::convert(key.pt);
}

boost::optional<cv::KeyPoint> Spot::atFrame(const int index) const {
    boost::optional<cv::KeyPoint> result;
    if (start() <= index && index <= end()) {
        result = keypoints[index - start()];
    }
    return result;
}

MatchPoints::MatchPoints() {
    index = 0;
}

void MatchPoints::match(const CapturedFrame &previous, const CapturedFrame &current) {
    index++;
    std::vector<cv::DMatch> matches;
    matcher.match(previous.detected.desc, current.detected.desc, matches);

    const auto findSpot = [&](const int preIndex) {
        auto found = indexedSpots.find(preIndex);
        if (found != indexedSpots.end()) {
            auto result = found->second;
            indexedSpots.erase(found);
            return result;
        } else {
            return Spot(index, previous.detected.keypoints[preIndex]);
        }
    };

    std::map<int, Spot> next;
    for (auto m: matches) {
        auto found = findSpot(m.queryIdx);
        found.addPoint(current.detected.keypoints[m.trainIdx]);
        next.insert(std::make_pair(m.trainIdx, found));
    }
    std::cout << "Matched spots: " << next.size() << std::endl;
    auto itr = indexedSpots.begin();
    while (itr != indexedSpots.end()) {
        spots.push_back(itr->second);
        ++itr;
    }
    std::cout << "Add passed spots: " << indexedSpots.size() << std::endl;
    indexedSpots = next;
}

boost::optional<Spot> MatchPoints::nearest(const Spot &spot) const {
    const auto p = spot.getLastPoint();
    const int frameIndex = spot.end();

    boost::optional<Spot> result;
    double d = -1;
    eachSpot([&](const Spot &s) {
        const auto key = s.atFrame(frameIndex);
        if (key) {
            const double od = CGAL::squared_distance(p, geometry::convert(key->pt));
            if (d < 0 || od != 0 && od < d) {
                result = s;
                d = od;
            }
        }
    });
    return result;
}

void MatchPoints::eachSpot(const std::function<void(Spot)> func) const {
    for (auto s: spots) {
        func(s);
    }
    eachCurrentSpot(func);
}
void MatchPoints::eachCurrentSpot(const std::function<void(Spot)> func) const {
    auto itr = indexedSpots.begin();
    while (itr != indexedSpots.end()) {
        func(itr->second);
        ++itr;
    }
}
