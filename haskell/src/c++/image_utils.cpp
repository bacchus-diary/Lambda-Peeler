
#include "opencv2/core.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;

Mat rotateByAngle(Mat src, double angle) {
    Point2f center(src.cols / 2.0, src.rows / 2.0);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);
    Rect bbox = RotatedRect(center, src.size(), angle).boundingRect();

    rot.at<double>(0, 2) += bbox.width / 2.0 - center.x;
    rot.at<double>(1, 2) += bbox.height / 2.0 - center.y;

    Mat result;
    warpAffine(src, result, rot, bbox.size());
    return result;
}
