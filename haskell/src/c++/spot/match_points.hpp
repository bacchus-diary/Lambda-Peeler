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
    std::vector<double> distances;
    geometry::Point_2 first;
    geometry::Point_2 last;
public:
    Spot(const int startIndex, const cv::KeyPoint &key);
    int start() const;
    int end() const;
    int size() const;

    const geometry::Point_2 &getLastPoint() const;
    geometry::Vector_2 getAveMovement(int c = 0) const;
    double getAveDistance(int c = 0) const;

    void addPoint(const cv::KeyPoint &key, const double distance);
    boost::optional<std::pair<cv::KeyPoint, double> > atFrame(const int index) const;
};

class MatchPoints {
private:
    int index;
    std::vector<Spot> spots;
    std::map<int, Spot> indexedSpots;
public:
    MatchPoints();
    void match(const CapturedFrame &previous, const CapturedFrame &current);
    boost::optional<Spot> nearest(const Spot &spot) const;
    void eachSpot(const std::function<void(Spot)> func) const;
    void eachCurrentSpot(const std::function<void(Spot)> func) const;
    int sizeCurrent() const;
};

#endif
