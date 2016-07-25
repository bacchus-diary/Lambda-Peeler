#ifndef CENTER_SLIT_H
#define CENTER_SLIT_H

#include <iostream>
#include <chrono>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "../util/geometry.hpp"
#include "match_points.hpp"

class NeighborSpot {
private:
    Spot spotA, spotB;
    geometry::Vector_2 aveDistance;
    double changeRate;
public:
    NeighborSpot(const Spot &a, const Spot &b);

    int getLastIndex();
    geometry::Point_2 getLastPoint();
};

class CenterSlit {
private:
    cv::Mat marged;
    Detected previous;
    cv::Vec2d moved;
    MatchPoints spots;
    int sizeOfFrame;

    geometry::Line_2 findCenter(const cv::Mat &frame, const geometry::Direction_2 &horizon);
public:
    CenterSlit();
    void addFrame(const cv::Mat &frame);
    cv::Mat getMarged();
    int getSizeOfFrame();
};

#endif
