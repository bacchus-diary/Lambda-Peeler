#include "center_slit.hpp"

cv::Ptr<cv::Feature2D> feature = cv::AKAZE::create();
cv::BFMatcher matcher(feature->defaultNorm(), true);

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

    changeRate = pow(vs.front().squared_length() / vs.back().squared_length(), 0.5 / vs.size());

    std::cout << "Changing rate of distance: " << changeRate << std::endl;
}

int NeighborSpot::getLastIndex() {
    return spotA.end();
}

geometry::Point_2 NeighborSpot::getLastPoint() {
    return spotA.getLastPoint();
}

double NeighborSpot::getChangeRate() {
    return changeRate;
}

geometry::Vector_2 &NeighborSpot::getDistance() {
    return aveDistance;
}

geometry::Line_2 CenterSlit::findCenter() {
    std::vector<NeighborSpot> dist;
    spots.eachCurrentSpot([&](const Spot &spot) {
        const auto n = spots.nearest(spot);
        if (!n) return;
        const auto neighbor = *n;
        dist.push_back(NeighborSpot(spot, neighbor));
    });
    std::cout << "Distributions of movement: " << dist.size() << std::endl;

    double p[] = {0.0, 0.0, 0.0};
    dlevmar_dif([](double *p, double *hx, int m, int n, void *adata) {
        std::vector<NeighborSpot> dist = *(std::vector<NeighborSpot> *)adata;

        // TODO Calculate distance

    }, p, NULL, 3, 3, 300, NULL, NULL, NULL, NULL, &dist);

    geometry::Line_2 centerV(p[0], p[1], p[2]);
    std::cout << "Center Vertical Line: " << centerV << std::endl;
    return centerV;
}

CenterSlit::CenterSlit(): spots(matcher), moved(0, 0), sizeOfFrame(0) {
    std::cout << "Initializing empty CenterSlit..." << std::endl;
}

cv::Mat CenterSlit::getMarged() {
    return marged;
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
        marged = frame;
    } else {
        spots.match(previous, current);
        takeTime("Matched.");
        const auto hvec = spots.movement();
        takeTime("Took movement.");
        const auto th = frame.cols / 100;
        std::cout << "Points movements (" << th << "): " << hvec << std::endl;
        if (th < sqrt(hvec.squared_length())) {
            findCenter();
        }
    }
    previous = current;
    sizeOfFrame++;
    takeTime("Finish.");
}

int CenterSlit::getSizeOfFrame() {
    return sizeOfFrame;
}
