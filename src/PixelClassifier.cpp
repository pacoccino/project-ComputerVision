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

void PixelClassifier::filterOutOfTerrain() {
    Mat thresh(classMat.rows, classMat.cols, CV_8UC1);

    for(int x=0; x<thresh.cols; x++) {
        for(int y=0; y<thresh.rows; y++) {
            if(classMat.at<uchar>(y,x) ==  TERRAIN)
                thresh.at<uchar>(y,x) = 255;
            else
                thresh.at<uchar>(y,x) = 0;
        }
    }

    namedWindow("coucoua");
    dilate(thresh, thresh, Mat(), Point(-1,-1), 2);

    imshow("coucoua", thresh);

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
/*
    for(int x=0; x<thresh.cols; x++) {
        for(int y=0; y<thresh.rows; y++) {
            if(thresh.at<uchar>(y,x) == 0 && classMat.at<uchar>(y,x) != BUT)
                classMat.at<uchar>(y,x) = POUBELLE;
        }
    }
*/

}
