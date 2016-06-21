
#include <iostream>
#include <sstream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/videoio.hpp"

#include "Facade_stub.h"

using namespace cv;
using namespace std;

extern "C" {
void loadVideo(char *videoFile) {
        VideoCapture capture(videoFile);
        if (!capture.isOpened()) {
            cerr << "Unable to open video file: " << videoFile << endl;
            exit(EXIT_FAILURE);
        }
        Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(400);
        vector<cv::KeyPoint> previousKeypoints, currentKeypoints;
        Mat previousFrame, currentFrame;
        while (capture.read(currentFrame)) {
            detector->detect(currentFrame, currentKeypoints);
            printf("Detected points: %d\n", currentKeypoints.size());
            if (previousKeypoints.size() > 0) {
                printf("Comparing previous frame...\n");
                compareKeypoints(currentKeypoints.size());
            }
            previousFrame = currentFrame;
            previousKeypoints = currentKeypoints;
        }
        capture.release();
}
}
