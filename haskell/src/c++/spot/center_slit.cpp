#include "center_slit.hpp"

const auto COLOR_BLUE = cv::Scalar(255, 0, 0);
const auto COLOR_GREEN = cv::Scalar(0, 255, 0);
const auto COLOR_RED = cv::Scalar(0, 0, 255);

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
            result = geometry::convert(key->first.pt);
        }
        return result;
    };
    boost::optional<geometry::Point_2> ptA, ptB;
    while ((ptA = getPt(spotA)) && (ptB = getPt(spotB))) {
        relations.push_back(*ptB - *ptA);
        frameIndex--;
    }
}

int NeighborSpot::getLastIndex() const {
    return spotA.end();
}

int NeighborSpot::size() const {
    return relations.size();
}

geometry::Point_2 NeighborSpot::getLastPoint() const {
    return spotA.getLastPoint();
}

double NeighborSpot::getAveChangeRate(int c) const {
    const auto a = relations[0];
    const auto result = getAveChange(c) * a / a.squared_length();
    return isnan(result) ? 0 : -result;
}

geometry::Vector_2 NeighborSpot::getAveChange(int c) const {
    if (c < 1) {
        c = size();
    } else {
        c = std::min(size(), c);
    }
    return (relations[0] - relations[c - 1]) / c;
}

geometry::Vector_2 NeighborSpot::getAveMovement(int c) const {
    return spotA.getAveMovement(c);
}

double NeighborSpot::getScore() const {
    const auto m = sqrt(getAveChange().squared_length());
    const auto a = spotA.getAveDistance();
    const auto b = spotB.getAveDistance();
    return m * a * b;
}

void CenterSlit::nearestNeighbor(std::vector<NeighborSpot> *dist, const std::function<bool(Spot)> pref) {
    std::vector<Spot> vecSpots;
    spots.eachCurrentSpot([&](const Spot &spot) {
        vecSpots.push_back(spot);
    });
    std::vector<NeighborSpot> distTmp;
    while (!vecSpots.empty()) {
        const auto spot = vecSpots.back();
        vecSpots.pop_back();
        if (pref == NULL || pref(spot)) {
            const auto p = spot.getLastPoint();
            double d = -1;
            int index = -1;
            for (int i = 0; i < vecSpots.size(); ++i) {
                const double od = CGAL::squared_distance(p, vecSpots[i].getLastPoint());
                if (1 <= od && (od < d || d < 0)) {
                    d = od;
                    index = i;
                }
            }
            if (0 <= index) {
                distTmp.push_back(NeighborSpot(spot, vecSpots[index]));
                vecSpots.erase(vecSpots.begin() + index);
            }
        }
    }
    if (distTmp.size() > 0) {
        double tmpValues[distTmp.size()];
        int index = 0;
        for (auto ns: distTmp) {
            tmpValues[index++] = ns.getScore();
        }
        std::cout << "Filtering spots: " << distTmp.size() << std::endl;
        const auto ac = (double*)hsAroundCenter(0.9, distTmp.size(), tmpValues);
        std::cout << "Arround Center: " << ac[0] << ", " << ac[1] << std::endl;

        for (auto ns: distTmp) {
            const auto v = ns.getScore();
            if (ac[0] <= v && v <= ac[1]) {
                dist->push_back(ns);
            }
        }
    }
}

geometry::Line_2 CenterSlit::findCenter(const cv::Mat &frame) {
    const auto startTime = std::chrono::system_clock::now();
    const auto takeTime = [&startTime](const std::string &msg) {
        const auto d = std::chrono::system_clock::now() - startTime;
        const auto t = std::chrono::duration_cast<std::chrono::milliseconds>(d);
        std::cout << "CenterSlit::findCenter: " << t.count() << "ms" << " : "<< msg << std::endl;
    };

    std::vector<NeighborSpot> dist;
    nearestNeighbor(&dist);
    takeTime("Filtered distributions of movement: " + std::to_string(dist.size()));

    const int frameCount = 10;
    double p[] = {1.0, 0.0, -(frame.cols / 2.0)};/*
    dlevmar_dif([](double *p, double *hx, int m, int n, void *adata) {
        const auto dist = *(std::vector<NeighborSpot> *)adata;
        geometry::Line_2 centerV(p[0], p[1], p[2]);
        const auto virtical = centerV.to_vector();
        const auto horizon = virtical.perpendicular(CGAL::CLOCKWISE);

        double h0 = 0, h1 = 0, h2 = 0;
        for (auto ns: dist) {
            const auto pt = ns.getLastPoint();
            const auto cr = ns.getAveChangeRate(frameCount);
            const auto m = ns.getAveChange(frameCount);
            const auto mV = virtical * m / sqrt(virtical.squared_length());
            const auto mH = horizon * m / sqrt(horizon.squared_length());
            const auto mV2 = pow(mV, 2);
            const auto mH2 = pow(mH, 2);

            // h0
            if ((centerV.has_on_positive_side(pt) ? 1 : -1) * cr < 0) {
                h0 += mH2;
            }

            // h1
            h1 += mH2 / CGAL::squared_distance(centerV, pt);

            // h2
            h2 += mV2;
        }
        hx[0] = h0;
        hx[1] = h1;
        hx[2] = h2;
        return;
    }, p, NULL, 3, 3, 300, NULL, NULL, NULL, NULL, &dist);*/

    geometry::Line_2 centerV(p[0], p[1], p[2]);
    takeTime("Center Vertical Line: " + std::to_string(p[0]) + ", " + std::to_string(p[1]) + ", " + std::to_string(p[2]));
    return centerV;
}

CenterSlit::CenterSlit(): spots(), moved(0, 0), sizeOfFrame(0) {
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

    const auto drawLine = [&](const geometry::Line_2 &line) {
        geometry::Iso_rectangle_2 rect(0, 0, frame.cols, frame.rows);
        const auto ins = CGAL::intersection(rect, line);
        if (ins) {
            if (const geometry::Segment_2* s = boost::get<geometry::Segment_2>(&*ins)) {
                const auto a = geometry::convert(s->source());
                const auto b = geometry::convert(s->target());
                std::cout << "Center Line: from " << a << " to " << b << std::endl;
                cv::line(frame, a, b, COLOR_BLUE, 2);
                writeImage(frame, "lined", sizeOfFrame);
            }
        }
    };

    printf("Detecting points at frame[%d]\n", sizeOfFrame);
    CapturedFrame current(frame);
    takeTime("Detected.");
    if (previous.isEmpty()) {
        margedFrame = frame;
    } else {
        spots.match(previous, current);
        const auto line = findCenter(frame);
        marge(line, frame);
        drawLine(line);
    }
    previous = current;
    sizeOfFrame++;
    takeTime("Finish.");
}

void CenterSlit::marge(const geometry::Line_2 &center, const cv::Mat &frame) {
    std::vector<NeighborSpot> dist;
    nearestNeighbor(&dist);
    geometry::Vector_2 total(0, 0);
    for (auto ns: dist) {
        total = total + ns.getAveMovement(blockSize);
    }
    total = total / dist.size();
    std::cout << "Average of movements: " << sqrt(total.squared_length()) << std::endl;
    const int moveX = total.x();
    const int moveY = total.y();

    const auto sizeX = margedFrame.cols - moveX;
    const auto sizeY = margedFrame.rows - abs(moveY);
    cv::Mat nextMarged(sizeY, sizeX, margedFrame.type());

    const auto cropBaseY = moveY < 0 ? -moveY : 0;
    const cv::Range rangeY(cropBaseY, cropBaseY + nextMarged.rows);
    const cv::Range rangeX(0, std::min(sizeX, margedFrame.cols));
    cv::Mat cropSrc(margedFrame, rangeY, rangeX);
    cv::Mat cropDst(nextMarged, cv::Range::all(), rangeX);
    cropSrc.copyTo(cropDst);

    baseline = std::max(0, baseline + moveY);

    for (int dstY = 0; dstY < sizeY; ++dstY) {
        for (int dstX = sizeX - frame.cols; dstX < sizeX; ++dstX) {
            const auto srcY = baseline + dstY;
            const auto srcX = dstX - sizeX + frame.cols;
            if (center.x_at_y(srcY) <= srcX) {
                nextMarged.at<cv::Vec3b>(cv::Point(dstX, dstY)) = frame.at<cv::Vec3b>(cv::Point(srcX, srcY));
            }
        }
    }

    margedFrame = nextMarged;
    std::cout << "Marged frame: " << margedFrame.cols << " x " << margedFrame.rows << std::endl;
    writeImage(margedFrame, "slit_marged", sizeOfFrame);
}

int CenterSlit::getSizeOfFrame() {
    return sizeOfFrame;
}
