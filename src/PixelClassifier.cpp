#include "PixelClassifier.h"

PixelClassifier::PixelClassifier()
{

}

void PixelClassifier::setImage(const Mat &image) {
    // set image in class and compute its class Matrix
    // Image must be in HSV

    sourceImage = image.clone();
    preprocess();
    computeMatrix();
    Mat terrainFiltered, goalFiltered;
    filterOutOfTerrain(terrainFiltered);
    filterGoal(goalFiltered);

    classMat.setTo(POUBELLE);
    for(int x=0; x<classMat.cols; x++) {
        for(int y=0; y<classMat.rows; y++) {
            classMat.at<char>(y,x) = goalFiltered.at<char>(y,x);
            if(terrainFiltered.at<char>(y,x) != POUBELLE)
                classMat.at<char>(y,x) = terrainFiltered.at<char>(y,x);
        }
    }

}

void PixelClassifier::preprocess() {

}

void PixelClassifier::computeMatrix() {
    // generate a class matrix from HSV matrix
    if(sourceImage.data == NULL)
        return;

    classMat.create(sourceImage.rows, sourceImage.cols, CV_8UC1);

    uchar hue, saturation, value;

    for(int x=0; x<sourceImage.cols; x++) {
        for(int y=0; y<sourceImage.rows; y++) {
            hue = sourceImage.at<Vec3b>(y,x)[0];
            saturation = sourceImage.at<Vec3b>(y,x)[1];
            value = sourceImage.at<Vec3b>(y,x)[2];
            classMat.at<char>(y,x) = (char)getClass(hue, saturation, value);
        }
    }
}

void PixelClassifier::generateImageFromClass(Mat &dest) {
    // generate an image from class matrix
    dest.create(classMat.rows, classMat.cols, CV_8UC3);

    for(int x=0; x<dest.cols; x++) {
        for(int y=0; y<dest.rows; y++) {
            setColorFromClass(dest.at<Vec3b>(y,x), classMat.at<char>(y,x));
        }
    }
}

bool PixelClassifier::isInRange(char source, char dest, char range) {
    // tells if a value is around another value mod 180
    int largeSource = source;
    int largeDest = dest;

    if(largeDest < (largeSource + range)%180 && largeSource < (largeDest + range)%180 )
        return true;
    return false;
}

PixelClass PixelClassifier::getClass(uchar hue, uchar saturation, uchar value) {
    // return the class of a color


    if(isInRange(hue, 20) && saturation > 100 && value > 150) // orange
        return BALLE;
    if(isInRange(hue, 40) && saturation > 100  && value > 100) // jaune
        return BUT;
    if(isInRange(hue, 60) && saturation > 100 && value > 100) // vert
        return TERRAIN;
    if(value > 150) // Blanc
        return LIGNE;
    return POUBELLE;
}

void PixelClassifier::setColorFromClass(Vec3b &dest, char src) {
    switch(src) {
    case BUT: // jaune
        dest[0] = 0;
        dest[1] = 255;
        dest[2] = 255;
        break;
    case BALLE: // orange
        dest[0] = 0;
        dest[1] = 165;
        dest[2] = 255;
        break;
    case LIGNE: // blanc
        dest[0] = 255;
        dest[1] = 255;
        dest[2] = 255;
        break;
    case TERRAIN: // vert
        dest[0] = 0;
        dest[1] = 255;
        dest[2] = 0;
        break;
    case POUBELLE:
        dest[0] = 0;
        dest[1] = 0;
        dest[2] = 0;
        break;
    }
}

void PixelClassifier::getOneClass(Mat &dest, PixelClass cl) {
    dest.create(classMat.rows, classMat.cols, CV_8UC1);


    for(int x=0; x<dest.cols; x++) {
        for(int y=0; y<dest.rows; y++) {
            if(classMat.at<uchar>(y,x) == cl)
                dest.at<uchar>(y,x) = 255;
            else
                dest.at<uchar>(y,x) = 0;
        }
    }

}


// WARNING! Must filter terrain out before
bool PixelClassifier::detectBall(Point2f &outputCenter, float &outputRadius) {
    Mat ballTresh;
    getOneClass(ballTresh, BALLE);

    Mat out = sourceImage.clone();

    vector< Point > *contour;
    contour = extractBiggestConnectedComposant(ballTresh, ballTresh);

    outputCenter = Point2f(-1,-1);
    outputRadius = -1;

    bool ballVisible = (contour != NULL);
    if (ballVisible){
        vector<Point> poly;

        approxPolyDP( Mat(*contour), poly, 3, true );
        minEnclosingCircle( (Mat)poly, outputCenter, outputRadius);

        // // for debugging display :
        //circle(ballTresh, outputCenter, (int)outputRadius,Scalar(255,255,255) , 2, 8, 0 );
    }
    return ballVisible;
}

bool PixelClassifier::detectGoal(vector<Point> &goalCorners, Point &center) {
    Mat goalThresh;
    getOneClass(goalThresh, BUT);


    int an;
    Mat element;
    an=2;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    erode(goalThresh, goalThresh, element);
    an=4;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    dilate(goalThresh, goalThresh, element);

    Mat edges;
    Canny(goalThresh, edges, 66.0, 133.0, 3);

    vector<Vec4i> lines;
    HoughLinesP(goalThresh, lines, 1, CV_PI/180, 40, 30, 10 );

    if(lines.size()>2) {
        vector<Point> points;
        for( size_t i = 0; i < lines.size(); i++ )
        {
            Vec4i l = lines[i];
            Point p;
            p.x = l[0];
            p.y = l[1];
            points.push_back(p);
            p.x = l[2];
            p.y= l[3];
            points.push_back(p);
        }
        int sumx=0, sumy=0, nb=0;
        for(size_t i = 0; i<points.size(); i++)  {
            sumx += points[i].x;
            sumy += points[i].y;
            nb++;
        }
        //Moments mom = moments(points);
        //center = Point(mom.m10/mom.m00, mom.m01/mom.m00);
        center = Point(sumx/(float)nb, sumy/(float)nb);

        int election;

        election = Tools::getNearestPoint(points, Point(0,classMat.rows));
        goalCorners.push_back(points[election]);
        election = Tools::getNearestPoint(points, Point(0,0));
        goalCorners.push_back(points[election]);
        election = Tools::getNearestPoint(points, Point(classMat.cols,0));
        goalCorners.push_back(points[election]);
        election = Tools::getNearestPoint(points, Point(classMat.cols,classMat.rows));
        goalCorners.push_back(points[election]);
        return true;
    }

    cout << "nin" << endl;
    return false;
}

void PixelClassifier::filterOutOfTerrain(Mat &dest) {
    Mat thresh;
    thresh.create(classMat.rows, classMat.cols, CV_8UC1);

    for(int x=0; x<thresh.cols; x++) {
        for(int y=0; y<thresh.rows; y++) {
            if(classMat.at<uchar>(y,x) == TERRAIN || classMat.at<uchar>(y,x) == BALLE || classMat.at<uchar>(y,x) == LIGNE)
                thresh.at<uchar>(y,x) = 255;
            else
                thresh.at<uchar>(y,x) = 0;
        }
    }

//    imshow("Before filter", thresh);

    int an;
    Mat element;

    // erode
    an=2;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    erode(thresh, thresh, element);
//    imshow("After erode", thresh);

    // Extraction de la composante connexe de surface la plus grande
    extractBiggestConnectedComposant(thresh, thresh);
//    imshow("After Connected Composant", thresh);

    // dilate
    an=5;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    dilate(thresh, thresh, element);
//    imshow("After dilatation", thresh);


    // filtrage

    dest = classMat.clone();
    for(int x=0; x<thresh.cols; x++) {
        for(int y=0; y<thresh.rows; y++) {
            if(thresh.at<uchar>(y,x) == 0 || classMat.at<uchar>(y,x) == BUT)
                dest.at<uchar>(y,x) = POUBELLE;
        }
    }
}

void PixelClassifier::filterGoal(Mat &dest) {
    Mat goalThresh;
    getOneClass(goalThresh, BUT);

    Tools::extractBiggestConnectedComposant(goalThresh, goalThresh);

    dest = classMat.clone();
    for(int x=0; x<goalThresh.cols; x++) {
        for(int y=0; y<goalThresh.rows; y++) {
            if(goalThresh.at<uchar>(y,x) == 0)
                dest.at<uchar>(y,x) = POUBELLE;
        }
    }
    /*Mat element = getStructuringElement(cv::MORPH_ELLIPSE, Size(2, 2));
    dilate(goalThresh, goalThresh, element);*/
}

void PixelClassifier::positionFromGoal(vector<Point> goal) {
    float leftHeight, rightHeight;
    leftHeight = Tools::distance(goal[0], goal[1]);
    rightHeight = Tools::distance(goal[2], goal[3]);

    float ecart = abs(leftHeight - rightHeight);
    cout << ecart << endl;
    if(leftHeight < rightHeight && ecart > 5) {
        cout << "Robot is in the right part of the terrain" << endl;
    }
    else {
        cout << "Robot is in the left part of the terrain" << endl;
    }
}
