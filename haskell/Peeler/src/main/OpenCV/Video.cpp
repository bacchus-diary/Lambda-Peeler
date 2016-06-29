
#include <iostream>
#include <sstream>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/video.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace cv;
using namespace std;

extern "C" {
    void loadVideo(char *videoFile) {
        initModule_nonfree();
        Ptr<FeatureDetector> detector = FeatureDetector::create("STAR");
        Ptr<DescriptorExtractor> extractor = DescriptorExtractor::create("BRIEF");

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
            cerr << "Detecting points at frame[" << i << "]" << endl;
            detector->detect(frame, keypoints);
            extractor->compute(frame, keypoints, desc);
            cerr << "Detected points: " << keypoints.size() << endl;
            i++;
        }
        capture.release();
    }
}
