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
            auto result = found->second;
            indexedSpots.erase(found);
            return result;
        } else {
            return Spot(index, previous.keypoints[preIndex]);
        }
    };

    std::map<int, Spot> next;
    for (auto m: matches) {
        auto found = findSpot(m.queryIdx);
        found.addPoint(current.keypoints[m.trainIdx]);
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

geometry::Vector_2 MatchPoints::movement() const {
    int total = 0;
    double x = 0, y = 0;
    if (!spots.empty() || !indexedSpots.empty()) {
        eachSpot([&](const Spot &s) {
            const auto m = s.movement();
            x += m.x();
            y += m.y();
            total++;
        });
        x /= total;
        y /= total;
    }
    std::cout << "Ave of movements (" << total << "): " << x << ", " << y << std::endl;
    return geometry::Vector_2(x, y);
}

boost::optional<Spot> MatchPoints::nearest(const geometry::Point_2 &p) const {
    boost::optional<Spot> result;
    double d = -1;
    eachSpot([&](const Spot &s) {
        const double od = CGAL::squared_distance(p, s.lastPoint());
        if (d < 0 || od != 0 && od < d) {
            result = s;
            d = od;
        }
    });
    return result;
}

void MatchPoints::eachSpot(const std::function<void(Spot)> func) const {
    for (auto s: spots) {
        func(s);
    }
    auto itr = indexedSpots.begin();
    while (itr != indexedSpots.end()) {
        func(itr->second);
        ++itr;
    }
}
