#include <iostream>
#include <sstream>

#include "levmar.h"

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
    vector<KeyPoint> pre;
    vector<KeyPoint> post;
};

struct Spot {
    Point3d p;
    vector<Point2f> inFrame;
    int lastIndex;
};

vector<Spot> spots;
Mat cameraMatrix = Mat::eye(3, 4, CV_64FC1);
vector<MatchPoints> detectedPoints;

void addFrame(MatchPoints detected) {
    detectedPoints.push_back(detected);
    if (spots.empty()) {
        for (DMatch m: detected.matches) {
            Spot s;
            s.lastIndex = m.queryIdx;
            Point2f p = detected.pre[s.lastIndex].pt;
            s.inFrame.push_back(p);
            s.p.x = p.x;
            s.p.y = p.y;
            s.p.z = 1;
            spots.push_back(s);
        }
    }
    for (Spot s: spots) {
        bool no = true;
        for (DMatch m: detected.matches) {
            if (s.lastIndex == m.queryIdx) {
                s.lastIndex = m.trainIdx;
                s.inFrame.push_back(detected.post[s.lastIndex].pt);
                no = false;
            }
        }
        if (no) {
            s.inFrame.push_back(Point2f());
        }
    }
}

void diffCamera(double p[], double hx[], int m, int n, void *adata) {
    Mat matrix(3, 4, CV_64FC1, p);
    int index = 0;
    for (auto spot: spots) {
        Point2f last = spot.inFrame.back();
        double diff = 0;
        if (last.x != 0 || last.y != 0) {
            Point3d p = spot.p;
            Mat z = matrix * (Mat_<double>(4, 1) << p.x, p.y, p.z, 1);
            double a = z.at<double>(2, 0);
            double x = z.at<double>(0, 0) / a;
            double y = z.at<double>(1, 0) / a;
            diff = pow(last.x - x, 2.0) + pow(last.y - y, 2.0);
        }
        hx[index++] = diff;
    }
}

void adjustCamera() {
    double *d = (double *)cameraMatrix.data;
    int length = 3 * 4;
    double p[length];
    for (int i = 0; i < length; ++i) {
        p[i] = d[i];
    }
    dlevmar_dif(diffCamera, p, NULL, length, spots.size(), 100, NULL, NULL, NULL, NULL, NULL);
    cameraMatrix = Mat(3, 4, CV_64FC1, p);
    cout << "Adjusted camera matrix: " << cameraMatrix << endl;
}

void adjustSpots() {

}

Ptr<Feature2D> feature = AKAZE::create();
BFMatcher matcher(feature->defaultNorm(), true);

MatchPoints matchPoints(Detected previous, Detected current, float distanceRate = 0.6f) {
    MatchPoints result;
    result.pre = previous.keypoints;
    result.post = current.keypoints;

    vector<DMatch> matches;
    matcher.match(previous.desc, current.desc, matches);

    vector<Point2f> points1, points2;
    for (DMatch m: matches) {
        points1.push_back(previous.keypoints[m.queryIdx].pt);
        points2.push_back(current.keypoints[m.trainIdx].pt);
    }

    Mat masks, hm;
    if (!matches.empty()) {
        hm = findHomography(points1, points2, masks, RANSAC, 3.0f);
    }
    for (size_t i = 0; i < masks.rows; i++) {
        uchar *inlier = masks.ptr<uchar>(i);
        if (inlier[0] == 1) {
            result.matches.push_back(matches[i]);
        }
    }

    return result;
}

void writeMatches(Detected previous, Detected current, MatchPoints points, int index) {
    if (points.matches.size() < 1) {
        cout << "No matches." << endl;
        return;
    }
    ostringstream oss;
    oss << "resources/out/matches-" << index << ".jpg";
    string filename = oss.str();
    mkParents((char *)filename.c_str(), 0775);
    Mat out;
    drawMatches(previous.frame, previous.keypoints, current.frame, current.keypoints, points.matches, out);
    cout << "Writing img to " << filename << endl;
    imwrite(filename, out);
}

extern "C" {
    void loadVideo(char *videoFile) {
        printf("Capturing video: %s\n", videoFile);
        double f = 1000.0 / cv::getTickFrequency();
        int64 startTime = getTickCount();

        VideoCapture capture(videoFile);
        if (!capture.isOpened()) {
            cerr << "Unable to open video file: " << videoFile << endl;
            exit(EXIT_FAILURE);
        }
        double timeCapture = (getTickCount() - startTime) * f;

        Mat frame;
        Detected previous, current;

        int i = 0;
        while (i < 10 && capture.read(frame)) {
            printf("Detecting points at frame[%d]\n", i);
            current.frame = rotateByAngle(frame, -90.0);
            feature->detectAndCompute(current.frame, noArray(), current.keypoints, current.desc);
            printf("Detected points: %d\n", current.keypoints.size());
            if (current.desc.rows == 0){
                cerr << "No features described." << endl;
                exit(EXIT_FAILURE);
            }
            if (previous.keypoints.size() > 0) {
                printf("Comparing previous frame...\n");
                MatchPoints mp = matchPoints(previous, current);
                addFrame(mp);
                adjustCamera();
                adjustSpots();
                writeMatches(previous, current, mp, i);
            }
            previous = current;
            i++;
        }
        double timeLoad = (getTickCount() - startTime) * f;
        cout << "Finish load video. capture: " << timeCapture << "ms, " << (timeLoad - timeCapture)/i << "ms/frame" << endl;
        capture.release();
    }
}
