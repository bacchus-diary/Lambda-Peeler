#ifndef CENTER_SLIT_H
#define CENTER_SLIT_H

#include <iostream>
#include <sstream>
#include <chrono>

#include <levmar.h>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "../util/geometry.hpp"
#include "match_points.hpp"

#include "Statistics_stub.h"

class NeighborSpot {
private:
    Spot spotA, spotB;
    geometry::Vector_2 aveDistance, aveMovement;
    double changeRate;
public:
    NeighborSpot(const Spot &a, const Spot &b);

    int getLastIndex() const;
    geometry::Point_2 getLastPoint() const;
    double getChangeRate() const;
    const geometry::Vector_2 &getDistance() const;
    const geometry::Vector_2 &getMovement() const;

    friend std::ostream &operator<<(std::ostream &s, NeighborSpot &o) {
        s << "NeighborSpot([" << o.getMovement() << "], [" << o.getLastPoint() << "])";
        return s;
    }
};

class CenterSlit {
private:
    cv::Mat margedFrame;
    Detected previous;
    cv::Vec2d moved;
    MatchPoints spots;
    int sizeOfFrame;

    geometry::Line_2 findCenter(const cv::Mat &frame);
public:
    CenterSlit();
    void addFrame(const cv::Mat &frame);
    cv::Mat getMarged();
    int getSizeOfFrame();
};

#endif
