#ifndef MATCH_POINTS_H
#define MATCH_POINTS_H

#include <iostream>

#include "../util/geometry.hpp"

struct MatchPoint {
    MatchPoint() {}
    MatchPoint(cv::DMatch m, cv::KeyPoint a, cv::KeyPoint b) {
        match = m;
        pre = a;
        post = b;
    }
    cv::DMatch match;
    cv::KeyPoint pre;
    cv::KeyPoint post;
};

class MatchPoints {

};

#endif
