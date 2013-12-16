#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "PixelClassifier.h"

using namespace std;
using namespace cv;

int main()
{
    cout << "Hello World!" << endl;
    namedWindow("Bonjour");
    PixelClassifier pc;
    Mat image;
    image = imread("RhobanVisionLog/log2/63.png");
    if(image.data == NULL) {
        cout << "Unable to load image" << endl;
        return -1;
    }

    imshow("Bonjour", image);

    pc.setImage(image);


    Mat imageNew;
    pc.generateImageFromClass(imageNew);
    namedWindow("new");
    imshow("new", imageNew);
    waitKey();
    return 0;
}

