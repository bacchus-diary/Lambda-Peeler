#ifndef MATCH_POINTS_H
#define MATCH_POINTS_H

#include <iostream>

#include <boost/optional.hpp>
#include <opencv2/opencv.hpp>

#include "../util/geometry.hpp"

class Detected {
public:
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat desc;

    void sortAndReduce(const double rate);
};

class Spot {
private:
    int startFrame;
    std::vector<cv::KeyPoint> keypoints;
    geometry::Point_2 first;
    geometry::Point_2 last;
public:
    Spot(const int startIndex, const cv::KeyPoint &key);
    int start() const;
    int end() const;

    const geometry::Point_2 &lastPoint() const;
    geometry::Vector_2 movement() const;

    void addPoint(const cv::KeyPoint &key);
    boost::optional<cv::KeyPoint> atFrame(const int index) const;
};

class MatchPoints {
private:
    int index;
    cv::BFMatcher matcher;
    std::vector<Spot> spots;
    std::map<int, Spot> indexedSpots;
public:
    MatchPoints(const cv::BFMatcher &_matcher);
    void match(const Detected &previous, const Detected &current);
    geometry::Vector_2 movement() const;
    boost::optional<Spot> nearest(const Spot &spot) const;
    void eachSpot(const std::function<void(Spot)> func) const;
};

#endif
