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

        void setIntercept(cv::Point2f p);
        void setVec(cv::Vec2f v);
        void setSlope(float s);

        float getSlope();
        cv::Point2f getIntercept();
        cv::Vec2f getVec();

        friend std::ostream& operator<< (std::ostream &out, const Line &line) {
            if (std::isinf(line.slope)) {
                return out << "x=" << line.intercept.x;
            } else {
                out << "y=(" << line.slope << ")x";
                if (line.intercept.y != 0) {
                    out << (line.intercept.y < 0 ? "-" : "+");
                    out << "(" <<  std::abs(line.intercept.y) << ")";
                }
                return out;
            }
        }
    };

    cv::Vec2f normVec(const cv::Vec2f src);
    cv::Vec2f rotateVec(const float angle, const cv::Vec2f src);
}
