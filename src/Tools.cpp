#include "Tools.h"

Tools::Tools()
{
}

float Tools::distance(Point p1, Point p2) {
    float dist;
    dist = sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
    return dist;
}

int Tools::getNearerPoint(vector<Point> list, Point p) {

    float dist, maxDist = 100000;
    int election = -1;

    for( size_t i = 0; i < list.size(); i++ )
    {
        dist = Tools::distance(p, list[i]);
        if(dist < maxDist) {
            maxDist = dist;
            election = i;
        }
    }

    return election;
}
