#include <iostream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "../util/geometry.hpp"
#include "center_slit.hpp"

cv::Ptr<cv::Feature2D> feature = cv::AKAZE::create();
cv::BFMatcher matcher(feature->defaultNorm(), true);

struct MatchPoint {
    MatchPoint(cv::DMatch m, cv::KeyPoint a, cv::KeyPoint b) {
        match = m;
        pre = a;
        post = b;
    }
    cv::DMatch match;
    cv::KeyPoint pre;
    cv::KeyPoint post;
};

std::pair<std::vector<MatchPoint>, Detected> matchPoints(Detected previous, Detected current) {
    std::vector<MatchPoint> result;
    Detected reduced;

    std::vector<cv::DMatch> matches;
    matcher.match(previous.desc, current.desc, matches);

    for (auto m: matches) {
        MatchPoint mp(m
            , previous.keypoints[m.queryIdx]
            , current.keypoints[m.trainIdx]);
        result.push_back(mp);

        reduced.keypoints.push_back(mp.post);
        if (reduced.desc.rows < 1) {
            reduced.desc = cv::Mat(1, current.desc.cols, current.desc.type());
        } else {
            reduced.desc.resize(reduced.keypoints.size());
        }
        current.desc.row(m.trainIdx).copyTo(reduced.desc.row(reduced.desc.rows - 1));
    }

    return std::make_pair(result, reduced);
}

cv::Vec2f findDirection(std::vector<MatchPoint> points) {
    float x = 0;
    float y = 0;
    for (auto p: points) {
        const auto v = p.post.pt - p.pre.pt;
        x += v.x;
        y += v.y;
    }
    return cv::Vec2f(x / points.size(), y / points.size());
}

geometry::Line findCenter(const cv::Mat frame, const std::vector<MatchPoint> points, const cv::Vec2f horizon) {
    geometry::Line result;
    result.direction = geometry::rotateVec(90, horizon);
    return result;
}

CenterSlit::CenterSlit() {
    std::cout << "Initializing empty CenterSlit..." << std::endl;
    moved = cv::Vec2f(0, 0);
}

cv::Mat CenterSlit::getMarged() {
    return marged;
}

void CenterSlit::addFrame(cv::Mat frame) {
    Detected current;
    feature->detectAndCompute(frame, cv::noArray(), current.keypoints, current.desc);
    std::cout << "Detected keypoints=" << current.keypoints.size() << std::endl;
    if (previous.desc.rows < 1) {
        marged = frame;
    } else {
        std::vector<MatchPoint> points;
        std:tie(points, current) = matchPoints(previous, current);
        std::cout << "Matched Points: " << points.size() << std::endl;

        const auto d = findDirection(points);
        moved += d;
        std::cout << "Direction of frame: " << d << " (total: " << moved << ")" << std::endl;

        const auto c = findCenter(frame, points, moved);
        std::cout << "Center: (" << c << ")" << std::endl;
    }
    previous = current;
}
