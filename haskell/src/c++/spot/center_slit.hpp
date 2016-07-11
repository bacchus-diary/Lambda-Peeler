#ifndef CENTER_SLIT_H
#define CENTER_SLIT_H

struct Detected {
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat desc;
};

class CenterSlit {
private:
    cv::Mat marged;
    Detected previous;
    cv::Point2f moved;
public:
    CenterSlit();
    void addFrame(cv::Mat frame);
    cv::Mat getMarged();
};

#endif
