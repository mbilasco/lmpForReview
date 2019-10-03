#ifndef FACESEPARATOR_H
#define FACESEPARATOR_H

#include "../lmp_common_header.h"
#include "faceroi.h"

class FaceSeparator
{
public:
    FaceSeparator();

    vector<FaceROI> getMeshs();
    Point2f getMiddle(Point2f p1, Point2f p2);
    void drawMesh(Mat inMat);

    double distanceBetween2points(Point2f p1, Point2f p2);

    virtual void createROI(vector<Point2f> landmarks);
    virtual void drawPoints(Mat inMat);

protected:
    vector<FaceROI> _meshs;
    vector<Point2f> _landmarks;

};

#endif // FACESEPARATOR_H
