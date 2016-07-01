
#include <iostream>
#include <sstream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/videoio.hpp"

#include "CompareKeys_stub.h"

using namespace cv;
using namespace std;

Ptr<BRISK> detector = BRISK::create();
BFMatcher matcher(detector->defaultNorm());

extern "C" {
    void loadVideo(char *videoFile) {
        printf("Capturing video: %s\n", videoFile);

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
            printf("Detecting points at frame[%d]\n", i);
            detector->detectAndCompute(frame, noArray(), keypoints, desc);
            printf("Detected points: %d\n", keypoints.size());
            hsCompareKeypoints(i);
            i++;
        }
        capture.release();
    }
}
