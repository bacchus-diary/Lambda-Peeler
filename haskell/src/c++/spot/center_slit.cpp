#include "center_slit.hpp"

cv::Ptr<cv::Feature2D> feature = cv::AKAZE::create();
cv::BFMatcher matcher(feature->defaultNorm(), true);

std::pair<std::vector<MatchPoint>, Detected> matchPoints(const Detected &previous, const Detected &current) {
    std::vector<MatchPoint> result;
    Detected reduced;

    std::vector<cv::DMatch> matches;
    matcher.match(previous.desc, current.desc, matches);

    for (auto m: matches) {
        MatchPoint mp = {m
            , previous.keypoints[m.queryIdx]
            , current.keypoints[m.trainIdx]
        };
        result.push_back(mp);

        reduced.keypoints.push_back(mp.post);
        if (reduced.desc.rows < 1) {
            reduced.desc = cv::Mat(1, current.desc.cols, current.desc.type());
        } else {
            reduced.desc.resize(reduced.keypoints.size());
        }
        current.desc.row(m.trainIdx).copyTo(reduced.desc.row(reduced.desc.rows - 1));
    }

    return std::make_pair(result, reduced);
}

cv::Vec2d findDirection(const std::vector<MatchPoint> &points) {
    float x = 0;
    float y = 0;
    for (auto p: points) {
        const auto v = p.post.pt - p.pre.pt;
        x += v.x;
        y += v.y;
    }
    return cv::Vec2d(x / points.size(), y / points.size());
}

double lengthOfIntersect(const geometry::Iso_rectangle_2 &rect, const geometry::Direction_2 &direction) {
    auto ins = CGAL::intersection(rect, geometry::Line_2(geometry::center(rect), direction));
    if (ins) {
        if (const geometry::Segment_2* s = boost::get<geometry::Segment_2>(&*ins)) {
            std::cout << "Horizontal segment: " << *s << std::endl;
            return s->squared_length();
        } else {
            const geometry::Point_2* p = boost::get<geometry::Point_2>(&*ins);
            std::cerr << "No intersection line of center, but point: " << *p << std::endl;
        }
    } else {
        std::cerr << "No intersection of center." << std::endl;
    }
    return (double)NAN;
}

MatchPoint nearest(const geometry::Point_2 &p, const std::vector<MatchPoint> &points) {
    MatchPoint result;
    double d = -1;
    for (auto o: points) {
        const double od = CGAL::squared_distance(p, geometry::convert(o.pre.pt));
        if (d < 0 || od != 0 && od < d) {
            result = o;
            d = od;
        }
    }
    return result;
}

geometry::Line_2 findCenter(const cv::Mat &frame, const std::vector<MatchPoint> &points, const geometry::Direction_2 &horizon) {
    geometry::Iso_rectangle_2 rect(0, 0, frame.cols, frame.rows);

    const double width = lengthOfIntersect(rect, horizon);
    std::cout << "Width: " << width << std::endl;

    geometry::Line_2 centerV(geometry::center(rect), geometry::mkRotation(90)(horizon));

    const double ep = width / 100;
    std::cout << "Around center: " << ep << std::endl;
    for (auto mp: points) {
        const auto p = geometry::convert(mp.pre.pt);
        const auto d = CGAL::squared_distance(centerV, p);
        if (d < ep) {
            auto neighbor = nearest(p, points);
            std::cout << "Neiborgh of [" << p << "]: " << neighbor.pre.pt << std::endl;
        }
    }

    return centerV;
}

CenterSlit::CenterSlit() {
    std::cout << "Initializing empty CenterSlit..." << std::endl;
    moved = cv::Vec2d(0, 0);
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
        std::vector<MatchPoint> points;
        std::tie(points, current) = matchPoints(previous, current);
        std::cout << "Matched Points: " << points.size() << std::endl;

        const auto d = findDirection(points);
        moved += d;
        std::cout << "Direction of frame: " << d << " (total: " << moved << ")" << std::endl;

        auto c = findCenter(frame, points, geometry::Vector_2(1, 0).direction());
        std::cout << "Center: " << c << std::endl;
    }
    previous = current;
}
