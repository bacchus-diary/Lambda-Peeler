#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <iostream>

#include <CGAL/Cartesian.h>
#include <CGAL/Point_2.h>
#include <CGAL/Vector_2.h>
#include <CGAL/Direction_2.h>
#include <CGAL/Line_2.h>
#include <CGAL/Ray_2.h>
#include <CGAL/Segment_2.h>
#include <CGAL/Triangle_2.h>
#include <CGAL/Iso_rectangle_2.h>
#include <CGAL/Circle_2.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Aff_transformation_2.h>
#include <CGAL/intersections.h>

#include <opencv2/core.hpp>

namespace geometry {
    typedef CGAL::Cartesian<double> K;

    typedef CGAL::Point_2<K> Point_2;
    typedef CGAL::Line_2<K> Line_2;
    typedef CGAL::Ray_2<K> Ray_2;
    typedef CGAL::Segment_2<K> Segment_2;
    typedef CGAL::Bbox_2 Bbox_2;
    typedef CGAL::Iso_rectangle_2<K> Iso_rectangle_2;
    typedef CGAL::Triangle_2<K> Triangle_2;
    typedef CGAL::Circle_2<K> Circle_2;
    typedef CGAL::Direction_2<K> Direction_2;
    typedef CGAL::Vector_2<K> Vector_2;
    typedef CGAL::Aff_transformation_2<K> Aff_transformation_2;

    double toRadian(double angle);

    Point_2 convert(const cv::Point2d &a);
    cv::Point2d convert(const Point_2 &a);

    Vector_2 convert(const cv::Vec2d &a);
    cv::Vec2d convert(const Vector_2 &a);

    Aff_transformation_2 mkRotation(double angle);

    Point_2 centerOf(const Iso_rectangle_2 &rect);
}

#endif
