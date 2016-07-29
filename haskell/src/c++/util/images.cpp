#include "images.hpp"

cv::Mat rotateByAngle(const cv::Mat &src, const double angle) {
    cv::Point2d center(src.cols / 2.0, src.rows / 2.0);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    cv::Rect bbox = cv::RotatedRect(center, src.size(), angle).boundingRect();

    rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
    rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;

    cv::Mat result;
    cv::warpAffine(src, result, rot, bbox.size());
    return result;
}

void writeImage(const cv::Mat &img, const std::string &name, const int index) {
    std::stringstream path;
    path << "resources/out/" << name;
    if (0 <= index) {
        path << "-" << index;
    }
    path << ".jpg";
    cv::imwrite(path.str(), img);
}
