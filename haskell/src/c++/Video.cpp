
#include <iostream>
#include <sstream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/videoio.hpp"

#include "filesystem.hpp"
#include "image_utils.hpp"
#include "CompareKeys_stub.h"

using namespace cv;
using namespace std;

struct Detected {
    Mat frame;
    vector<KeyPoint> keypoints;
    Mat desc;
};

struct MatchPoints {
    vector<DMatch> matches;
    vector<DMatch> inliers;
};

Ptr<Feature2D> feature = AKAZE::create();
BFMatcher matcher(feature->defaultNorm(), true);

MatchPoints matchPoints(Detected previous, Detected current, float distanceRate = 0.6f) {
    MatchPoints result;

    matcher.match(previous.desc, current.desc, result.matches);

    vector<Point2f> points1, points2;
    for (size_t i = 0; i < result.matches.size(); i++) {
        DMatch m = result.matches[i];
        points1.push_back(previous.keypoints[m.queryIdx].pt);
        points2.push_back(current.keypoints[m.trainIdx].pt);
    }

    Mat masks, hm;
    if (result.matches.size() > 0) {
        hm = findHomography(points1, points2, masks, RANSAC, 3.0f);
    }
    for (size_t i = 0; i < masks.rows; i++) {
        uchar *inlier = masks.ptr<uchar>(i);
        if (inlier[0] == 1) {
            result.inliers.push_back(result.matches[i]);
        }
    }

    return result;
}

void writeMatches(Detected previous, Detected current, MatchPoints points, string filename) {
    if (points.matches.size() < 1) {
        cout << "No matches." << endl;
        return;
    }
    mkParents((char *)filename.c_str(), 0775);
    Mat out;
    drawMatches(previous.frame, previous.keypoints, current.frame, current.keypoints, points.inliers, out);
    cout << "Writing img to " << filename << endl;
    imwrite(filename, out);
    cout << "Completed to write img to " << filename << endl;
}

extern "C" {
    void loadVideo(char *videoFile) {
        printf("Capturing video: %s\n", videoFile);

        VideoCapture capture(videoFile);
        if (!capture.isOpened()) {
            cerr << "Unable to open video file: " << videoFile << endl;
            exit(EXIT_FAILURE);
        }

        Mat frame;
        Detected previous, current;

        int i = 0;
        while (i < 10 && capture.read(frame)) {
            printf("Detecting points at frame[%d]\n", i);
            current.frame = rotateByAngle(frame, -90.0);
            feature->detectAndCompute(current.frame, noArray(), current.keypoints, current.desc);
            printf("Detected points: %d\n", current.keypoints.size());
            if (current.desc.rows == 0){
                cerr << "WARNING: No features found." << endl;
                exit(EXIT_FAILURE);
            }
            if (previous.keypoints.size() > 0) {
                printf("Comparing previous frame...\n");
                MatchPoints mp = matchPoints(previous, current);
                ostringstream filename;
                filename << "resources/out/matches-" << i << ".jpg";
                writeMatches(previous, current, mp, filename.str());
            }
            previous = current;
            i++;
        }
        capture.release();
    }
}
