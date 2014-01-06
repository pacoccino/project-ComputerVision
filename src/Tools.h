#ifndef TOOLS_H
#define TOOLS_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


namespace Tools
{
    float distance(cv::Point p1, cv::Point p2);
    int getNearerPoint(std::vector<cv::Point> list, cv::Point p);
    std::vector<cv::Point> *extractBiggestConnectedComposant(cv::Mat source, cv::Mat dest);
};

#endif // TOOLS_H
