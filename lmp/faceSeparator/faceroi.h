#ifndef FACEROI_H
#define FACEROI_H

#include "../../global_header.h"

class FaceROI
{
public:
    FaceROI();

    Mat estimateMask(Mat inMat);
    vector<Point2f> getPoints();
    Point2f getMiddle();
    void drawLine(Mat &inMat);
    double getMin();

    void init(vector<Point2f> vect, double min);

    double AngleBetweenThreePoints(Point2f pointA, Point2f pointB, Point2f pointC);

    double distance(Point2f p1, Point2f p2);

    vector<double> getInformation();
    vector<double> getInformation2();

private:
    vector<Point2f> _points;
    Point2f _middle;
    Mat _mask;
    double _min;
};

#endif // FACEROI_H
