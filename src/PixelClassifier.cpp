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

bool PixelClassifier::isInRange(char source, char dest, char range) {
    // tells if a value is around another value
    int largeSource = source + 255;
    int largeDest = dest + 255;

    if( (largeDest - range) < largeSource && largeSource < (largeDest + range) )
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
    if(saturation < 60 &&  value > 150) // Blanc
        return LIGNE;
    return POUBELLE;
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

void PixelClassifier::generateImageFromClass(Mat &dest) {
    // generate an image from class matrix
    dest.create(classMat.rows, classMat.cols, CV_8UC3);

    for(int x=0; x<dest.cols; x++) {
        for(int y=0; y<dest.rows; y++) {
            setColorFromClass(dest.at<Vec3b>(y,x), classMat.at<char>(y,x));
        }
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


    Mat edges,lignes;
    Canny(goalThresh, edges, 66.0, 133.0, 3);

    namedWindow("Goal");
    imshow("Goal", edges);

    vector<Vec4i> lines;
    HoughLinesP(goalThresh, lines, 1, CV_PI/180, 40, 70, 10 );

    cout << "Nombre de lognes : " << lines.size() << endl;
    lignes.create(classMat.rows, classMat.cols, CV_8UC3);
    lignes.setTo(Scalar(0,0,0));
    vector<Point> points;
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        //line( lignes, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
        Point p;
        p.x = l[0];
        p.y = l[1];
        points.push_back(p);

        p.x = l[2];
        p.y= l[3];
        points.push_back(p);
    }

    for( size_t i = 0; i < points.size(); i++ )
    {
        circle(lignes, points[i], 2, Scalar(255,0,0), 2);
    }

    int election;
    election = Tools::getNearerPoint(points, Point(0,0));
    circle(lignes, points[election], 2, Scalar(0,0,255), 2);
    election = Tools::getNearerPoint(points, Point(classMat.cols,0));
    circle(lignes, points[election], 2, Scalar(0,0,255), 2);
    election = Tools::getNearerPoint(points, Point(0,classMat.rows));
    circle(lignes, points[election], 2, Scalar(0,0,255), 2);
    election = Tools::getNearerPoint(points, Point(classMat.cols,classMat.rows));
    circle(lignes, points[election], 2, Scalar(0,0,255), 2);

    namedWindow("lignes");
    imshow("lignes", lignes);

    waitKey();
}

void PixelClassifier::filterOutOfTerrain() {
    Mat thresh(classMat.rows, classMat.cols, CV_8UC1);

    for(int x=0; x<thresh.cols; x++) {
        for(int y=0; y<thresh.rows; y++) {
            if(classMat.at<uchar>(y,x) ==  TERRAIN || classMat.at<uchar>(y,x) == BALLE || classMat.at<uchar>(y,x) == LIGNE)
                thresh.at<uchar>(y,x) = 255;
            else
                thresh.at<uchar>(y,x) = 0;
        }
    }

    namedWindow("coucoua");

    int an;
    Mat element;

    an=5;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    erode(thresh, thresh, element);

    an=5;
    element = getStructuringElement(cv::MORPH_ELLIPSE, Size(an*2+1, an*2+1), Point(an, an) );
    dilate(thresh, thresh, element);

    imshow("coucoua", thresh);
    waitKey();
    Mat edges;
    Canny(thresh, edges, 66.0, 133.0, 3);

    dilate(edges, edges, Mat(), Point(-1,-1));
    namedWindow("coucou");
    imshow("coucou", edges);

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

    for(int x=0; x<thresh.cols; x++) {
        for(int y=0; y<thresh.rows; y++) {
            if(thresh.at<uchar>(y,x) == 0 && classMat.at<uchar>(y,x) != BUT)
                classMat.at<uchar>(y,x) = POUBELLE;
        }
    }


}
