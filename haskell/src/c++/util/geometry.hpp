namespace geometry {
    class Line {
    private:
        cv::Point2f intercept;
        float slope;
        void set(cv::Vec2f d, cv::Point2f p = cv::Point2f(0, 0));
        void set(float slope, cv::Point2f p = cv::Point2f(0, 0));

    public:
        Line();
        Line(cv::Vec2f vec);
        Line(float slope);
        Line(float intercept_y, float slope);
        Line(cv::Point2f p, cv::Vec2f d);
        Line(cv::Point2f p, float slope);

        void setPoint(cv::Point2f p);
        void setVec(cv::Vec2f v);
        void setSlope(float s);

        float getSlope();
        cv::Point2f getIntercept();
        cv::Vec2f getVec();

        friend std::ostream& operator<< (std::ostream &out, const Line &line) {
            const auto adda = [&out](const float a) {
                if (a != 0) {
                    out << (a < 0 ? "-" : "+") << "(" <<  std::abs(a) << ")";
                }
            };
            if (1 < std::abs(line.slope)) {
                out << "x=(" << (1 / line.slope) << ")y";
                adda(line.intercept.x - line.intercept.y / line.slope);
            } else {
                out << "y=(" << line.slope << ")x";
                adda(line.intercept.y - line.intercept.x * line.slope);
            }
            return out;
        }
    };

    cv::Vec2f normVec(const cv::Vec2f src);
    cv::Vec2f rotateVec(const float angle, const cv::Vec2f src);
}
