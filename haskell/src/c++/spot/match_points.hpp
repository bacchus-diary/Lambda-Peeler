#ifndef MATCH_POINTS_H
#define MATCH_POINTS_H

#include <iostream>
#include <chrono>

#include <boost/optional.hpp>
#include <opencv2/opencv.hpp>

#include "../util/geometry.hpp"
#include "../util/images.hpp"

struct Detected {
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat desc;
};

class CapturedFrame {
private:
    cv::Mat originalFrame, contouredFrame;
    void getContoured();
public:
    Detected detected;
    CapturedFrame();
    CapturedFrame(const cv::Mat &frame);
    bool isEmpty();
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

    const geometry::Point_2 &getLastPoint() const;
    geometry::Vector_2 movement() const;

    void addPoint(const cv::KeyPoint &key);
    boost::optional<cv::KeyPoint> atFrame(const int index) const;
};

class MatchPoints {
private:
    int index;
    std::vector<Spot> spots;
    std::map<int, Spot> indexedSpots;
public:
    MatchPoints();
    void match(const CapturedFrame &previous, const CapturedFrame &current);
    geometry::Vector_2 movement() const;
    boost::optional<Spot> nearest(const Spot &spot) const;
    void eachSpot(const std::function<void(Spot)> func) const;
    void eachCurrentSpot(const std::function<void(Spot)> func) const;
};

#endif
