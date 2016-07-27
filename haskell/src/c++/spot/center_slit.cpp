#include "center_slit.hpp"

cv::Ptr<cv::Feature2D> feature = cv::AKAZE::create();
cv::BFMatcher matcher(feature->defaultNorm(), true);

const auto COLOR_BLUE = cv::Scalar(255, 0, 0);
const auto COLOR_RED = cv::Scalar(0, 0, 255);

void writeImage(const cv::Mat &img, const std::string &name, const int index) {
    std::stringstream path;
    path << "resources/out/" << name << "-" << index << ".jpg";
    cv::imwrite(path.str(), img);
}

boost::optional<double> lengthOfIntersect(const geometry::Iso_rectangle_2 &rect, const geometry::Line_2 &line) {
    boost::optional<double> result;
    auto ins = CGAL::intersection(rect, line);
    if (ins) {
        if (const geometry::Segment_2* s = boost::get<geometry::Segment_2>(&*ins)) {
            std::cout << "Horizontal segment: " << *s << std::endl;
            result = sqrt(s->squared_length());
        } else {
            const geometry::Point_2* p = boost::get<geometry::Point_2>(&*ins);
            std::cerr << "No intersection line of center, but point: " << *p << std::endl;
        }
    } else {
        std::cerr << "No intersection of center." << std::endl;
    }
    return result;
}

NeighborSpot::NeighborSpot(const Spot &a, const Spot &b): spotA(a), spotB(b) {
    int frameIndex = getLastIndex();
    const auto getPt = [&frameIndex](const Spot &s) {
        boost::optional<geometry::Point_2> result;
        const auto key = s.atFrame(frameIndex);
        if (key) {
            result = geometry::convert(key->pt);
        }
        return result;
    };
    geometry::Vector_2 va(0, 0);
    std::vector<geometry::Vector_2> vs;
    boost::optional<geometry::Point_2> ptA, ptB;
    while ((ptA = getPt(spotA)) && (ptB = getPt(spotB))) {
        const auto v = *ptB - *ptA;
        va = va + v;
        vs.push_back(v);
        frameIndex--;
    }
    aveDistance = va / vs.size();
    aveMovement = (vs.back() - vs.front()) / vs.size();
    changeRate = pow(vs.front().squared_length() / vs.back().squared_length(), 0.5 / vs.size());
}

int NeighborSpot::getLastIndex() const {
    return spotA.end();
}

geometry::Point_2 NeighborSpot::getLastPoint() const {
    return spotA.getLastPoint();
}

double NeighborSpot::getChangeRate() const {
    return changeRate;
}

const geometry::Vector_2 &NeighborSpot::getDistance() const {
    return aveDistance;
}

const geometry::Vector_2 &NeighborSpot::getMovement() const {
    return aveMovement;
}

geometry::Line_2 CenterSlit::findCenter(const cv::Mat &frame) {
    std::vector<NeighborSpot> distTmp;
    spots.eachCurrentSpot([&](const Spot &spot) {
        const auto n = spots.nearest(spot);
        if (!n) return;
        const auto neighbor = *n;

        distTmp.push_back(NeighborSpot(spot, neighbor));
    });
    const auto toValue = [](const NeighborSpot &ns) {
        return sqrt(ns.getMovement().squared_length());
    };
    double tmpValues[distTmp.size()];
    int index = 0;
    for (auto ns: distTmp) {
        tmpValues[index++] = toValue(ns);
    }
    const auto ac = (double*)hsAroundCenter(0.9, distTmp.size(), tmpValues);
    std::cout << "Arround Center: " << ac[0] << ", " << ac[1] << std::endl;

    std::vector<NeighborSpot> dist;
    for (auto ns: distTmp) {
        const auto v = toValue(ns);
        if (ac[0] <= v && v <= ac[1]) {
            dist.push_back(ns);

            const auto lp = ns.getLastPoint();
            line(frame, geometry::convert(lp), geometry::convert(lp - ns.getMovement()), COLOR_BLUE, 2);
        }
    }
    std::cout << "Sorted distributions of movement: " << dist.size() << std::endl;

    double p[] = {1.0, 0.0, -(frame.cols / 2.0)};
    dlevmar_dif([](double *p, double *hx, int m, int n, void *adata) {
        const auto dist = *(std::vector<NeighborSpot> *)adata;
        geometry::Line_2 centerV(p[0], p[1], p[2]);

        double h0 = 0, h1 = 0, h2 = 0;
        for (auto ns: dist) {
            const auto pt = ns.getLastPoint();
            const auto rate = ns.getChangeRate();
            const auto rt = isnan(rate) ? 1 : rate - 1;
            const auto r2 = pow(rt, 2);

            // h0
            const double s = (centerV.has_on_positive_side(pt) ? -1 : 1) * rt;
            if (s < 0) {
                h0 += r2;
            }

            // h1
            h1 += CGAL::squared_distance(centerV, pt) * r2;

            // h2
            const auto v = centerV.to_vector();
            const auto m = ns.getMovement();
            h2 += pow(v * m, 2) / v.squared_length();
        }
        hx[0] = h0;
        hx[1] = h1;
        hx[2] = h2;
        return;
    }, p, NULL, 3, 3, 300, NULL, NULL, NULL, NULL, &dist);

    geometry::Line_2 centerV(p[0], p[1], p[2]);
    std::cout << "Center Vertical Line: " << centerV << std::endl;
    return centerV;
}

CenterSlit::CenterSlit(): spots(matcher), moved(0, 0), sizeOfFrame(0) {
    std::cout << "Initializing empty CenterSlit..." << std::endl;
}

cv::Mat CenterSlit::getMarged() {
    return margedFrame;
}

void CenterSlit::addFrame(const cv::Mat &frame) {
    const auto startTime = std::chrono::system_clock::now();
    const auto takeTime = [&startTime](const std::string &msg) {
        const auto d = std::chrono::system_clock::now() - startTime;
        const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(d);
        std::cout << "CenterSlit::addFrame: " << t.count() << "ms" << " : " << msg << std::endl;
    };

    printf("Detecting points at frame[%d]\n", sizeOfFrame);
    Detected current;
    current.detectAndCompute(frame, feature);
    takeTime("Detected.");
    std::cout << "Detected keypoints=" << current.keypoints.size() << std::endl;
    if (previous.desc.rows < 1) {
        margedFrame = frame;
    } else {
        spots.match(previous, current);
        takeTime("Matched.");
        const auto hvec = spots.movement();
        takeTime("Took movement.");
        const auto th = frame.cols / 1000;
        std::cout << "Points movements (" << th << "): " << hvec << std::endl;
        if (th < sqrt(hvec.squared_length())) {
            const auto line = findCenter(frame);
            geometry::Iso_rectangle_2 rect(0, 0, frame.cols, frame.rows);
            const auto ins = CGAL::intersection(rect, line);
            if (ins) {
                if (const geometry::Segment_2* s = boost::get<geometry::Segment_2>(&*ins)) {
                    const auto a = geometry::convert(s->source());
                    const auto b = geometry::convert(s->target());
                    std::cout << "Line: from " << a << " to " << b << std::endl;
                    cv::line(frame, a, b, COLOR_RED, 2);
                    writeImage(frame, "lined", sizeOfFrame);
                }
            }
        }
    }
    previous = current;
    sizeOfFrame++;
    takeTime("Finish.");
}

int CenterSlit::getSizeOfFrame() {
    return sizeOfFrame;
}
