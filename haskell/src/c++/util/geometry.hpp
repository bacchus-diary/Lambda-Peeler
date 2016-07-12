namespace geometry {
    class Line {
    public:
        Line();
        Line(cv::Vec2f vec);
        Line(float slope);
        Line(float intercept_y, float slope);
        Line(cv::Point2f p, cv::Vec2f d);
        Line(cv::Point2f p, float slope);

        cv::Point2f intercept;
        float slope;

        cv::Vec2f vec();

        friend std::ostream& operator<< (std::ostream &out, const Line &line) {
            return out << "intercept:" << line.intercept << ", slope:" << line.slope;
        }
    };

    cv::Vec2f normVec(const cv::Vec2f src);
    cv::Vec2f rotateVec(const float angle, const cv::Vec2f src);
}
