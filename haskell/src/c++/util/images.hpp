
#include "opencv2/core.hpp"
#include <sstream>


cv::Mat rotateByAngle(const cv::Mat &src, const double angle);
void writeImage(const cv::Mat &img, const std::string &name, const int index = -1);
