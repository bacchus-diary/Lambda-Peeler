namespace geometry {
    struct Line {
        cv::Point2f point;
        cv::Vec2f direction;
        friend std::ostream& operator<< (std::ostream &out, const Line &line) {
            return out << "point:" << line.point << ", direction:" << line.direction;
        }
    };

    cv::Vec2f normVec(const cv::Vec2f src);
    cv::Vec2f rotateVec(const float angle, const cv::Vec2f src);
}
