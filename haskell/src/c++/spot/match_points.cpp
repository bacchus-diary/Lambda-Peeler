#include "match_points.hpp"

const auto feature = cv::ORB::create();
const cv::BFMatcher matcher(feature->defaultNorm(), true);

void getContoured(const cv::Mat &src, cv::Mat &target) {
    cv::Mat grayFrame;
    cv::cvtColor(src, grayFrame, cv::COLOR_BGR2GRAY);

    const auto sobel = [&]() {
        cv::Mat tmp;
        cv::Sobel(grayFrame, tmp, CV_64F, 1, 1);
        cv::convertScaleAbs(tmp, target, 1, 0);
    };
    const auto laplacian = [&]() {
        cv::Mat tmp;
        cv::Laplacian(grayFrame, tmp, CV_64F, 3);
        cv::convertScaleAbs(tmp, target, 1, 0);
    };
    const auto canny = [&]() {
        cv::Mat tmp;
        cv::blur(grayFrame, tmp, cv::Size(3, 3));
        cv::Canny(tmp, target, 50, 200);
    };
    const auto unsharp = [&]() {
        cv::Mat tmp;
        cv::GaussianBlur(src, tmp, cv::Size(21, 21), 20.0);
        cv::addWeighted(src, 2, tmp, -1, 0, target);
    };
    const auto rough = [&]() {
        cv::Mat tmp, tmp2;
        cv::blur(grayFrame, tmp, cv::Size(30, 30));
        cv::GaussianBlur(tmp, tmp2, cv::Size(21, 21), 20);
        cv::addWeighted(tmp, 2.0, tmp2, -1.0, 0, target);
    };

    unsharp();
}

void detectAndCompute(const cv::Mat &img, Detected &detected) {
    std::vector<cv::KeyPoint> tmpVec;
    feature->detect(img, tmpVec);
    std::cout << "Reducing keypoints: " << tmpVec.size() << std::endl;
    for (auto key: tmpVec) {
        if (1 < key.octave) {
            detected.keypoints.push_back(key);
        }
    }
    std::cout << "Computing keypoints: " << detected.keypoints.size() << std::endl;
    feature->compute(img, detected.keypoints, detected.desc);
}

void matchDescs(const Detected &a, const Detected &b, std::vector<cv::DMatch> *matches) {
    std::vector<cv::DMatch> tmp;
    matcher.match(a.desc, b.desc, tmp);
    std::cout << "Reducing matches: " << tmp.size() << std::endl;
    for (auto m: tmp) {
        if (m.distance < 50) {
            matches->push_back(m);
        }
    }
    std::cout << "Finish discriptors match: " << matches->size() << std::endl;
}

CapturedFrame::CapturedFrame() {
    std::cout << "Created empty CapturedFrame." << std::endl;
}

CapturedFrame::CapturedFrame(const cv::Mat &frame): originalFrame(frame) {
    getContoured(originalFrame, contouredFrame);
    writeImage(contouredFrame, "contoured");
    detectAndCompute(contouredFrame, detected);
}

bool CapturedFrame::isEmpty() {
    return originalFrame.rows == 0;
}

Spot::Spot(const int startIndex, const cv::KeyPoint &key) {
    startFrame = startIndex - 1;
    addPoint(key, 0);
    first = last;
}

int Spot::start() const {
    return startFrame;
}

int Spot::end() const {
    return startFrame + keypoints.size() - 1;
}

int Spot::size() const {
    return keypoints.size();
}

const geometry::Point_2 &Spot::getLastPoint() const {
    return last;
}

geometry::Vector_2 Spot::getAveMovement(int c) const {
    const int size = keypoints.size();
    if (c < 1) {
        c = size;
    } else {
        c = std::min(size, c);
    }
    const auto since = geometry::convert(keypoints[size - c].pt);
    return (last - since) / c;
}

double Spot::getAveDistance(int c) const {
    const int size = distances.size();
    if (c < 1) {
        c = size;
    } else {
        c = std::min(size, c);
    }
    double sum = 0;
    for (int i = size - c; i < size; i++) {
        sum += distances[i];
    }
    return sum / c;
}

void Spot::addPoint(const cv::KeyPoint &key, const double distance) {
    keypoints.push_back(key);
    distances.push_back(distance);
    last = geometry::convert(key.pt);
}

boost::optional<std::pair<cv::KeyPoint, double> > Spot::atFrame(const int index) const {
    boost::optional<std::pair<cv::KeyPoint, double> > result;
    if (start() <= index && index <= end()) {
        const auto i = index - start();
        result = std::make_pair(keypoints[i], distances[i]);
    }
    return result;
}

MatchPoints::MatchPoints() {
    index = 0;
}

void MatchPoints::match(const CapturedFrame &previous, const CapturedFrame &current) {
    index++;
    std::vector<cv::DMatch> matches;
    matchDescs(previous.detected, current.detected, &matches);

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
        found.addPoint(current.detected.keypoints[m.trainIdx], m.distance);
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
            const double od = CGAL::squared_distance(p, geometry::convert(key->first.pt));
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

int MatchPoints::sizeCurrent() const {
    return indexedSpots.size();
}
