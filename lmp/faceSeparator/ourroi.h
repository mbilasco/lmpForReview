#ifndef OURROI_H
#define OURROI_H

#include "../../global_header.h"
#include "faceseparator.h"

class ourROI : public FaceSeparator
{
public:
    ourROI();

    void createROI(vector<Point2f> landmarks);
    void drawPoints(Mat inMat);
    Mat delimitateFace(Mat inMat);
    int getPatchSize(int L);

private:
    void estimatePoints();
    void estimateROIs();

    vector<Point2f> _estimatedPoints;
};

#endif // OURROI_H
