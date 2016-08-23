#ifndef CENTER_SLIT_H
#define CENTER_SLIT_H

#include <iostream>
#include <chrono>

#include <levmar.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

#include "../util/geometry.hpp"
#include "../util/images.hpp"
#include "match_points.hpp"

#include "Statistics_stub.h"

class NeighborSpot {
private:
    Spot spotA, spotB;
    std::vector<geometry::Vector_2> relations;
    double changeRate;
public:
    NeighborSpot(const Spot &a, const Spot &b);

    int getLastIndex() const;
    int size() const;
    geometry::Point_2 getLastPoint() const;

    double getAveChangeRate(int c = 0) const;
    geometry::Vector_2 getAveChange(int c = 0) const;
    geometry::Vector_2 getAveMovement(int c = 0) const;

    double getScore() const;

    friend std::ostream &operator<<(std::ostream &s, NeighborSpot &o) {
        s << "NeighborSpot([" << o.getAveChange() << "], [" << o.getLastPoint() << "])";
        return s;
    }
};

class CenterSlit {
private:
    cv::Mat margedFrame;
    CapturedFrame previous;
    double baseline = 0;
    cv::Vec2d moved;
    MatchPoints spots;
    int sizeOfFrame;

    geometry::Line_2 findCenter(const cv::Mat &frame);
    void marge(const geometry::Line_2 &center, const cv::Mat &frame);
    void nearestNeighbor(std::vector<NeighborSpot> *dist, const std::function<bool(Spot)> pref = NULL);
public:
    const int blockSize = 20;
    CenterSlit();
    void addFrame(const cv::Mat &frame);
    cv::Mat getMarged();
    int getSizeOfFrame();
};

#endif
