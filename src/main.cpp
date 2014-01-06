#include <iostream>
#include <sys/time.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "PixelClassifier.h"

using namespace std;
using namespace cv;

string dir = "RhobanVisionLog/log2/";
string ext = ".png";
string prefix = "";
int start = 129;
int end = 150;
int delay = 10000000;


void fetchImages();
void process(Mat);

int main() {

    fetchImages();


    return 0;
}

void fetchImages() {

    struct timeval startTime, endTime;

    for (int i = start; i <= end;) {
        stringstream ss;
        ss << dir << prefix << i << ext;
        string file = ss.str();

        // Init
        gettimeofday(&startTime, NULL);

        Mat image;
        image = imread(file);
        if(image.data == NULL) {
            cout << "Unable to load image" << endl;
            i++;
            continue;
        }

        process(image);

        // Result
        gettimeofday(&endTime, NULL);
        cout << "Proccessing time for " << prefix << i << ext << " : "
             << (endTime.tv_sec - startTime.tv_sec) * 1000 +
                (endTime.tv_usec - startTime.tv_usec) / 1000
             << " ms" << endl;

        char k = (unsigned int)cvWaitKey(delay);
        if (k == 27) { // == ESC
            break;
        }else if(k == 81){ // <--
            if (i - 1 < start)
                continue;
            --i;
        }else{
            ++i;
        }
    }
}

void show(const char *str, Mat image, int &offset){
    namedWindow(str);
    cvMoveWindow(str, offset, 0);
    offset += image.cols ;
    imshow(str, image);
}

void process(Mat image) {
    PixelClassifier pc;
    pc.setImage(image);

    int offset = 0;

    // * * * * base image * * * * //
    show("Base image", image, offset);

    // * * * * Filter Terrain * * * * //
    Mat imageOut;
    pc.generateImageFromClass(imageOut);
    show("Filtered Terrain", imageOut, offset);



    // * * * * BALL * * * * //
    Mat ballOut = image.clone();
    Point2f center; float radius;
    bool isBallVisible = pc.detectBall(center, radius);
    if (isBallVisible){
        cout << "[BALL] Detected at " << center << " of radius " << radius << endl;
        circle(ballOut, center, (int)radius * 3,Scalar(0,0,255) , 2, 8, 0 );
        show ("ball", ballOut, offset);
    }else{
        cout << "[BALL] Not detected" << endl;
    }

    // * * * * GOAL * * * * //
    Mat goalOut = image.clone();
    vector<Point> goal;
    Point goalCenter;

    if(pc.detectGoal(goal, goalCenter)) {
        circle(goalOut, goalCenter, 1 * 3,Scalar(0,255) , 2, 8, 0 );
        cout << "[GOAL] Detected at " << goalCenter << endl;
        show("Goal", goalOut, offset);

        pc.positionFromGoal(goal);
    }
    else {
        cout << "Goal not detected" << endl;
    }
}
