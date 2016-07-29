#ifndef UTIL_IMAGES_H
#define UTIL_IMAGES_H

#include <sstream>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

cv::Mat rotateByAngle(const cv::Mat &src, const double angle);
void writeImage(const cv::Mat &img, const std::string &name, const int index = -1);

#endif
