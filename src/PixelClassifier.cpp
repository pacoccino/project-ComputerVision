#include "PixelClassifier.h"

PixelClassifier::PixelClassifier()
{

}

void PixelClassifier::setImage(const Mat &image) {
    // set image in class and compute its class Matrix
    // Image must be in HSV

    sourceImage = image.clone();
    computeMatrix();
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
    // tells if a value is around another value
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


// WARNING! Must filter terrain out
void PixelClassifier::detectBall() {
    Mat ballTresh;
    getOneClass(ballTresh, BALLE);

    Mat out = sourceImage.clone();

    vector< Point > *contour;
    contour = extractBiggestConnectedComposant(ballTresh, ballTresh);

    if (contour != NULL){
        vector<Point> poly;
        Point2f center;
        float radius;

        approxPolyDP( Mat(*contour), poly, 3, true );
        minEnclosingCircle( (Mat)poly, center, radius);
        circle(ballTresh, center, (int)radius,Scalar(255,255,255) , 2, 8, 0 );


        circle(out, center, (int)radius * 3,Scalar(0,0,255) , 2, 8, 0 );

        cout << "Ball at " << center << " of radius " << radius << "\n";
    }else{
        cout << "Ball not detected\n";
    }



    imshow("Ball", out);

}

void PixelClassifier::detectGoal() {
    Mat goalThresh;
    getOneClass(goalThresh, BUT);

    int an;
    Mat element;

    an=2;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    erode(goalThresh, goalThresh, element);

    an=3;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    dilate(goalThresh, goalThresh, element);

    namedWindow("Goal");
    imshow("Goal", goalThresh);


    Mat edges;
    Canny(goalThresh, edges, 66.0, 133.0, 3);

    vector<Vec2f> lines;
    HoughLines( edges, lines, 1, CV_PI/180, 50, 0, 0 );
    cout << lines.size() << endl;

    Mat lignes(classMat.rows, classMat.cols, CV_8UC3);
    for( size_t i = 0; i < lines.size(); i++ )
    {
         float rho = lines[i][0], theta = lines[i][1];
         Point pt1, pt2;
         double a = cos(theta), b = sin(theta);
         double x0 = a*rho, y0 = b*rho;
         pt1.x = cvRound(x0 + 1000*(-b));
         pt1.y = cvRound(y0 + 1000*(a));
         pt2.x = cvRound(x0 - 1000*(-b));
         pt2.y = cvRound(y0 - 1000*(a));
         line( lignes, pt1, pt2, Scalar(0,0,255), 1, CV_AA);
    }

    namedWindow("lignes");
    imshow("lignes", lignes);

    waitKey();
}

std::vector< cv::Point > *PixelClassifier::extractBiggestConnectedComposant(Mat source, Mat dest){
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

void PixelClassifier::filterOutOfTerrain() {
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

    imshow("Before filter", thresh);

    int an;
    Mat element;

    // erode
    an=2;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    erode(thresh, thresh, element);
    imshow("After erode", thresh);

    // Extraction de la composante connexe de surface la plus grande
    extractBiggestConnectedComposant(thresh, thresh);
    imshow("After Connected Composant", thresh);

    // dilate
    an=5;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    dilate(thresh, thresh, element);
    imshow("After dilatation", thresh);


    // filtrage
    for(int x=0; x<thresh.cols; x++) {
        for(int y=0; y<thresh.rows; y++) {
            if(thresh.at<uchar>(y,x) == 0 || classMat.at<uchar>(y,x) == BUT)
                classMat.at<uchar>(y,x) = POUBELLE;
        }
    }
}
