#include <iostream>
#include <sstream>
#include <chrono>

#include "levmar.h"

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/videoio.hpp"

#include "spot/center_slit.hpp"
#include "util/filesystem.hpp"
#include "util/images.hpp"
#include "CompareKeys_stub.h"

using namespace cv;
using namespace std;

extern "C" {
    void loadVideo(char *videoFile) {
        printf("Capturing video: %s\n", videoFile);
        const auto startTime = std::chrono::system_clock::now();
        const auto byMS = [](std::chrono::system_clock::time_point t) {
            const auto d = std::chrono::system_clock::now() - t;
            return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
        };

        VideoCapture capture(videoFile);
        if (!capture.isOpened()) {
            cerr << "Unable to open video file: " << videoFile << endl;
            exit(EXIT_FAILURE);
        }
        const auto timeCapture = byMS(startTime);

        CenterSlit slit;
        Mat frame;

        int i = 0;
        while (i < 10 && capture.read(frame)) {
            printf("Detecting points at frame[%d]\n", i);
            slit.addFrame(rotateByAngle(frame, -90.0));
            i++;
            std::cout << std::endl;
        }
        const auto timeLoad = byMS(startTime);
        cout << "Finish load video. capture: " << timeCapture << "ms, " << (timeLoad - timeCapture)/i << "ms/frame" << endl;
        capture.release();
        imwrite("resources/out/slit_marged.jpg", slit.getMarged());
    }
}
