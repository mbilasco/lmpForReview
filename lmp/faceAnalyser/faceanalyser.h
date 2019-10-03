#ifndef FACEANALYSER_H
#define FACEANALYSER_H

#include "../lmp_common_header.h"
#include "../faceSeparator/faceroi.h"

class faceAnalyser
{
public:
    faceAnalyser();

    virtual void createVector(vector< vector<double> > facialMotion, Mat flow, vector<FaceROI> meshs, Mat frame);

    vector< vector<double> > getVector();

protected:
    vector<double> _meanVectorFrame;
    vector< vector<double> > _meanVectorSequence;
};

#endif // FACEANALYSER_H
