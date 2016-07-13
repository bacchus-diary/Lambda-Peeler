#include <iostream>

#include "opencv2/core.hpp"

#include "geometry.hpp"

namespace geometry {
    Line::Line() {
        set(0.0f);
    }
    Line::Line(float slope_) {
        set(slope_);
    }
    Line::Line(cv::Vec2f d) {
        set(d);
    }
    Line::Line(float intercept_y, float slope_) {
        set(slope_, cv::Point2f(0, intercept_y));
    }
    Line::Line(cv::Point2f p, cv::Vec2f d) {
        set(d, p);
    }
    Line::Line(cv::Point2f p, float slope_) {
        set(slope_, p);
    }

    void Line::set(cv::Vec2f d, cv::Point2f p) {
        set(d[1] / d[0], p);
    }
    void Line::set(float slope_, cv::Point2f p) {
        float x = 0, y = 0;
        if (1 < std::abs(slope_)) {
            x = (y - p.y) / slope_ + p.x;
        } else {
            y = slope_ * (x - p.x) + p.y;
        }
        intercept = cv::Point2f(x, y);
        slope = slope_;
    }

    void Line::setPoint(cv::Point2f p) {
        set(slope, p);
    }
    void Line::setVec(cv::Vec2f v) {
        set(v, intercept);
    }
    void Line::setSlope(float s) {
        set(s, intercept);
    }

    float Line::getSlope() {
        return slope;
    }
    cv::Point2f Line::getIntercept() {
        return cv::Point2f(intercept);
    }
    cv::Vec2f Line::getVec() {
        if (1 < std::abs(slope)) {
            return cv::Vec2f(1/slope, 1);
        } else {
            return cv::Vec2f(1, slope);
        }
    }

    cv::Vec2f rotateVec(const float angle, const cv::Vec2f src) {
        const auto g = angle * M_PI / 180;
        const float c = std::cos(g);
        const float s = std::sin(g);
        return normVec(cv::Matx22f(c, -s, s, c) * src);
    }

    cv::Vec2f normVec(const cv::Vec2f src) {
        const float d = std::abs(src[0]);
        return cv::Vec2f(src[0] / d, src[1] / d);
    }
}
