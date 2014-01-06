#ifndef TOOLS_H
#define TOOLS_H

#include <opencv2/core/core.hpp>

using namespace cv;

class Tools
{
public:
    Tools();
    static float distance(Point p1, Point p2);
    static int getNearerPoint(vector<Point> list, Point p);
};

#endif // TOOLS_H
