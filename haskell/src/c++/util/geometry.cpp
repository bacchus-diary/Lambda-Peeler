#include "opencv2/core.hpp"

#include "geometry.hpp"

namespace geometry {
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
