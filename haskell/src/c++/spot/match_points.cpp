#include "match_points.hpp"

Spot::Spot(const int startIndex, const cv::KeyPoint &key) {
    startFrame = startIndex;
    addPoint(key);
    first = last;
}

int Spot::start() const {
    return startFrame;
}

int Spot::end() const {
    return startFrame + keypoints.size() - 2;
}

const geometry::Point_2 &Spot::lastPoint() const {
    return last;
}

geometry::Vector_2 Spot::movement() const {
    return last - first;
}

void Spot::addPoint(const cv::KeyPoint &key) {
    keypoints.push_back(key);
    last = geometry::convert(key.pt);
}

MatchPoints::MatchPoints(const cv::BFMatcher &_matcher) {
    index = 0;
    matcher = _matcher;
}

void MatchPoints::match(const Detected &previous, const Detected &current) {
    index++;
    std::vector<cv::DMatch> matches;
    matcher.match(previous.desc, current.desc, matches);

    const auto findSpot = [&](const int preIndex) {
        auto found = indexedSpots.find(preIndex);
        if (found != indexedSpots.end()) {
            return found->second;
        } else {
            const auto s = Spot(index, previous.keypoints[preIndex]);
            spots.push_back(s);
            return s;
        }
    };

    std::map<int, Spot> next;
    for (auto m: matches) {
        auto found = findSpot(m.queryIdx);
        found.addPoint(current.keypoints[m.trainIdx]);
        next.insert(std::make_pair(m.trainIdx, found));
    }
    std::cout << "Matched spots: " << next.size() << std::endl;
    indexedSpots = next;
}

geometry::Vector_2 MatchPoints::movement() const {
    const int total = spots.size();
    double x = 0, y = 0;
    if (0 < total) {
        for (auto s: spots) {
            const auto m = s.movement();
            x += m.x();
            y += m.y();
        }
        x /= total;
        y /= total;
    }
    std::cout << "Sum of movements (" << total << "): " << x << ", " << y << std::endl;
    return geometry::Vector_2(x, y);
}

Spot MatchPoints::nearest(const geometry::Point_2 &p) const {
    Spot *result;
    double d = -1;
    for (auto s: spots) {
        const double od = CGAL::squared_distance(p, s.lastPoint());
        if (d < 0 || od != 0 && od < d) {
            *result = s;
            d = od;
        }
    }
    return *result;
}

void MatchPoints::eachSpot(const std::function<void(Spot)> func) const {
    for (auto s: spots) {
        func(s);
    }
}
