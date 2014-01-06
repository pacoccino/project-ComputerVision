#ifndef PIXELCLASSIFIER_H
#define PIXELCLASSIFIER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#include "Tools.h"

using namespace cv;
using namespace std;
using namespace Tools;

enum PixelClass { BUT, BALLE, LIGNE, TERRAIN, POUBELLE };

class PixelClassifier
{

    Mat sourceImage;
    Mat classMat;

    PixelClass getClass(uchar hue, uchar saturation, uchar value);
    bool isInRange(char source, char dest, char range = 10);
    void setColorFromClass(Vec3b &dest, char src);

public:
    PixelClassifier();
    void setImage(const Mat &image);
    void computeMatrix();
    void generateImageFromClass(Mat &dest);

    void getOneClass(Mat &dest, PixelClass cl);
    void filterOutOfTerrain(Mat &dest);
    bool detectGoal(vector<Point> &goalCorners, Point &center);
    bool detectBall(Point2f &outputCenter, float &outputRadius);
    void filterGoal(Mat &dest);
    void preprocess();
};

#endif // PIXELCLASSIFIER_H
