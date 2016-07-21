#ifndef CENTER_SLIT_H
#define CENTER_SLIT_H

#include <iostream>
#include <chrono>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "../util/geometry.hpp"
#include "match_points.hpp"

class CenterSlit {
private:
    cv::Mat marged;
    Detected previous;
    cv::Vec2d moved;
    MatchPoints spots;
public:
    CenterSlit();
    void addFrame(const cv::Mat &frame);
    cv::Mat getMarged();
};

#endif
