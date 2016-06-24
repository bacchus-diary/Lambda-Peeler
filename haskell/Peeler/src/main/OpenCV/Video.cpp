
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

struct Detected {
    vector<KeyPoint> keypoints;
    Mat desc;
};

struct MatchPoints {
    vector<DMatch> matches;
    vector<Point2f> pre;
    vector<Point2f> post;
};

Ptr<cv::xfeatures2d::SURF> detector = cv::xfeatures2d::SURF::create(400);
BFMatcher matcher(detector->defaultNorm());

MatchPoints matchPoints(Detected previous, Detected current, float distanceRate = 0.6f) {
    MatchPoints result;
    vector< vector< DMatch > > matches;
    matcher.knnMatch(previous.desc, current.desc, matches, 2);
    for (size_t i = 0; i < matches.size(); i++) {
        DMatch m0 = matches[i][0], m1 = matches[i][1];
        if (m0.distance <= m1.distance * distanceRate) {
            result.matches.push_back(m0);
            result.pre.push_back(previous.keypoints[m0.queryIdx].pt);
            result.post.push_back(current.keypoints[m0.trainIdx].pt);
        }
    }
    return result;
}

extern "C" {
    void loadVideo(char *videoFile) {
        VideoCapture capture(videoFile);
        if (!capture.isOpened()) {
            cerr << "Unable to open video file: " << videoFile << endl;
            exit(EXIT_FAILURE);
        }

        Mat frame;
        Detected previous, current;

        int i = 0;
        while (i < 10 && capture.read(frame)) {
            detector->detectAndCompute(frame, noArray(), current.keypoints, current.desc);
            printf("Detected points: %d\n", current.keypoints.size());
            if (previous.keypoints.size() > 0) {
                printf("Comparing previous frame...\n");
                MatchPoints mp = matchPoints(previous, current);
                compareKeypoints(i);
            }
            previous = current;
            i++;
        }
        capture.release();
    }
}
