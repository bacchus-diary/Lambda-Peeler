#include "center_slit.hpp"

cv::Ptr<cv::Feature2D> feature = cv::AKAZE::create();
cv::BFMatcher matcher(feature->defaultNorm(), true);

double lengthOfIntersect(const geometry::Iso_rectangle_2 &rect, const geometry::Direction_2 &direction) {
    auto ins = CGAL::intersection(rect, geometry::Line_2(geometry::centerOf(rect), direction));
    if (ins) {
        if (const geometry::Segment_2* s = boost::get<geometry::Segment_2>(&*ins)) {
            std::cout << "Horizontal segment: " << *s << std::endl;
            return sqrt(s->squared_length());
        } else {
            const geometry::Point_2* p = boost::get<geometry::Point_2>(&*ins);
            std::cerr << "No intersection line of center, but point: " << *p << std::endl;
        }
    } else {
        std::cerr << "No intersection of center." << std::endl;
    }
    return (double)NAN;
}

geometry::Line_2 findCenter(const cv::Mat &frame, const MatchPoints &points, const geometry::Direction_2 &horizon) {
    geometry::Iso_rectangle_2 rect(0, 0, frame.cols, frame.rows);

    const double width = lengthOfIntersect(rect, horizon);
    std::cout << "Width: " << width << std::endl;

    geometry::Line_2 centerV(geometry::centerOf(rect), geometry::mkRotation(90)(horizon));

    const double ep = width / 10;
    std::cout << "Around center: " << ep << std::endl;
    points.eachSpot([&](const Spot &spot) {
        const auto p = spot.lastPoint();
        const auto d = sqrt(CGAL::squared_distance(centerV, p));
        if (d < ep) {
            const auto neighbor = points.nearest(p);
            if (neighbor) {
                const auto v = neighbor->lastPoint() - p;
                std::cout << "Neiborgh: " << v << std::endl;
            }
        }
    });

    return centerV;
}

CenterSlit::CenterSlit(): spots(matcher), moved(0, 0) {
    std::cout << "Initializing empty CenterSlit..." << std::endl;
}

cv::Mat CenterSlit::getMarged() {
    return marged;
}

void CenterSlit::addFrame(const cv::Mat &frame) {
    Detected current;
    feature->detectAndCompute(frame, cv::noArray(), current.keypoints, current.desc);
    std::cout << "Detected keypoints=" << current.keypoints.size() << std::endl;
    if (previous.desc.rows < 1) {
        marged = frame;
    } else {
        spots.match(previous, current);
        const auto hvec = spots.movement();
        const auto th = frame.cols / 100;
        std::cout << "Points movements (" << th << "): " << hvec << std::endl;
        if (th < sqrt(hvec.squared_length())) {
            findCenter(frame, spots, hvec.direction());
        }
    }
    previous = current;
}
