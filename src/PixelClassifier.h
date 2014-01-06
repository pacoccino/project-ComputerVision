#ifndef PIXELCLASSIFIER_H
#define PIXELCLASSIFIER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

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
    void computeMatrix();
    void setImage(const Mat &image);
    void generateImageFromClass(Mat &dest);
    void filterOutOfTerrain();
    void getOneClass(Mat &dest, PixelClass cl);
    void detectGoal();
    void detectBall();
    void extractBiggestConnectedComposant(Mat source, Mat dest);
};

#endif // PIXELCLASSIFIER_H
