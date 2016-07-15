#ifndef CENTER_SLIT_H
#define CENTER_SLIT_H

#include <iostream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "../util/geometry.hpp"
#include "match_points.hpp"

struct Detected {
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat desc;
};

class CenterSlit {
private:
    cv::Mat marged;
    Detected previous;
    cv::Vec2d moved;
public:
    CenterSlit();
    void addFrame(const cv::Mat &frame);
    cv::Mat getMarged();
};

#endif
