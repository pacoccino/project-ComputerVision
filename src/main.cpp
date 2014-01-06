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
int start = 3;
int end = 50;
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
    pc.filterOutOfTerrain();
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
    vector<Point> goal;
    Point goalCenter;
    bool isGoalVisible = pc.detectGoal(goal, goalCenter);
    Mat goalOut = image.clone();
    if (isGoalVisible){
        circle(goalOut, goalCenter, 10, Scalar(255,0,0), 10);
        show("Goal", goalOut, offset);
    }
}

int test() {
    Mat src, dst;

    /// Load image
    src = imread("RhobanVisionLog/log1/1.png", CV_LOAD_IMAGE_COLOR);

    if( !src.data )
    { return -1; }

    /// Separate the image in 3 places ( B, G and R )
    vector<Mat> bgr_planes;
    split( src, bgr_planes );

    /// Establish the number of bins
    int histSize = 180;

    /// Set the ranges ( for B,G,R) )
    float range[] = { 0, 180 } ;
    const float* histRange = { range };

    bool uniform = true; bool accumulate = false;

    Mat b_hist, g_hist, r_hist;

    /// Compute the histograms:
    calcHist( &bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );

    // Draw the histograms for B, G and R
    int hist_w = 512; int hist_h = 400;
    int bin_w = cvRound( (double) hist_w/histSize );

    Mat histImage( hist_h, hist_w, CV_8UC3, Scalar( 0,0,0) );

    /// Normalize the result to [ 0, histImage.rows ]
    normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );
    normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat() );

    /// Draw for each channel
    for( int i = 1; i < histSize; i++ )
    {
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ) ,
              Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
              Scalar( 255, 0, 0), 2, 8, 0  );
        /*line( histImage, Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ) ,
              Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
              Scalar( 0, 255, 0), 2, 8, 0  );
        line( histImage, Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ) ,
              Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
              Scalar( 0, 0, 255), 2, 8, 0  );
    */
    }

    /// Display
    namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE );
    imshow("calcHist Demo", histImage );

    waitKey(0);


    return 0;
}



/*
int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "[ERROR] Invalid argument." << endl;
        return 0;
    }

    Mat src = imread(argv[1], CV_LOAD_IMAGE_COLOR);

    mapDetection(src);
}

void mapDetection(Mat src) {
    Mat imgThresh, imgFg, imgBg;

    cvtColor(src, src, CV_HSV2BGR);
    inRange(src, Scalar(100, 100, 100), Scalar(255, 255, 255), imgThresh);

    erode(imgThresh, imgFg, Mat(), Point(-1, -1), 2);
    dilate(imgThresh, imgBg, Mat(), Point(-1, -1), 3);

    namedWindow("src");
    imshow("src", src);

    namedWindow("thresh");
    imshow("thresh", imgThresh);

    namedWindow("Fg");
    imshow("Fg", imgFg);

    namedWindow("Bg");
    imshow("Bg", imgBg);

    waitKey();

}



  353 96 86
  356 91 87
  355 78 65
  352 98 92
  358 87 86
  352 98 92

*/
