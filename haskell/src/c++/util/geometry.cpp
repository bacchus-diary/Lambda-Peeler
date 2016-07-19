#include "geometry.hpp"

namespace geometry {
    double toRadian(double angle) {
        return angle * M_PI / 180;
    }

    Point_2 convert(const cv::Point2d &a) {
        return Point_2(a.x, a.y);
    }
    cv::Point2d convert(const Point_2 &a) {
        return cv::Point2d(CGAL::to_double(a.x()), CGAL::to_double(a.y()));
    }

    Vector_2 convert(const cv::Vec2d &a) {
        return Vector_2(a(0), a(1));
    }
    cv::Vec2d convert(const Vector_2 &a) {
        return cv::Vec2d(CGAL::to_double(a.x()), CGAL::to_double(a.y()));
    }

    Aff_transformation_2 mkRotation(double angle) {
        auto r = toRadian(angle);
        Aff_transformation_2 result(CGAL::ROTATION, sin(r), cos(r));
        return result;
    }

    Point_2 centerOf(const Iso_rectangle_2 &rect) {
        const auto x = (rect.xmax() - rect.xmin()) / 2;
        const auto y = (rect.ymax() - rect.ymin()) / 2;
        return Point_2(x, y);
    }
}
