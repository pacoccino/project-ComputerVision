#include "Tools.h"

using namespace cv;
using namespace std;
using namespace Tools;

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


std::vector< cv::Point > *Tools::extractBiggestConnectedComposant(Mat source, Mat dest){
    //Mat img = source.clone();
    //Mat out = source.clone();
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(source, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_TC89_KCOS);
    int id = -1;
    float area = 0;
    for ( size_t i=0; i < contours.size(); ++i )
    {
        float currentArea = contourArea(contours.at(i));
        if (currentArea > area){
            area = currentArea;
            id = i;
        }
    }

    dest.setTo(0);
    if (id>=0){
        drawContours(dest, contours, id, Scalar(255), CV_FILLED);

        vector< Point > *out = new vector< Point >();
        *out = contours[id];
        return out;
    }

    return NULL;
}
