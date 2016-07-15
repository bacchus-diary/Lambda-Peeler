#ifndef MATCH_POINTS_H
#define MATCH_POINTS_H

#include <iostream>

#include "../util/geometry.hpp"

struct MatchPoint {
    cv::DMatch match;
    cv::KeyPoint pre;
    cv::KeyPoint post;
};

class MatchPoints {

};

#endif
