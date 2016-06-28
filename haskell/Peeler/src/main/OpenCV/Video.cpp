
#include <iostream>
#include <sstream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/videoio.hpp"

using namespace cv;
using namespace std;

Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(400);
BFMatcher matcher(detector->defaultNorm());

extern "C" {
    void loadVideo(char *videoFile) {
        VideoCapture capture(videoFile);
        if (!capture.isOpened()) {
            cerr << "Unable to open video file: " << videoFile << endl;
            exit(EXIT_FAILURE);
        }

        Mat frame;
        vector<KeyPoint> keypoints;
        Mat desc;

        int i = 0;
        while (i < 10 && capture.read(frame)) {
            detector->detectAndCompute(frame, noArray(), keypoints, desc);
            printf("Detected points: %d\n", keypoints.size());
            i++;
        }
        capture.release();
    }
}
