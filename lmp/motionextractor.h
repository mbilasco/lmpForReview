#ifndef MOTIONEXTRACTOR_H
#define MOTIONEXTRACTOR_H

#include "lmp_common_header.h"
#include "filterMotion/patch.h"
#include <thread>
#include <mutex>

class motionExtractor
{
public:

    motionExtractor();

    vector<double> OurExtractMotion(Mat flux, Mat mask, int bins, Point2f center, int iteration, double min);
    vector<double> getOccurences();
    void drawFlow(Mat &picture);
    Mat getCoherencyMap();
    void drawDistribution(Mat &picture);

    void patchParam(int lambda, int delta, double batta, int ecart, int mean, int vari);


private:

    vector<double> drawCurveOnHistogram(Mat values);
    bool pertinentMotion(float angle, vector<double> cumul);

    void diffusionMotion(Patch previousPatch, Mat flow);
    void patchNeighbor(int X, int Y, Mat Flow, Patch previousPatch);


    int _bins, _min;

    vector<Patch> _allPatch;

    vector<double> _cumul;

    Mat _coherencyMap;

    Point2f _center;

    int _lambda, _delta, _ecart, _mean, _vari;
    double _batta;

    int _minPatch;

};

#endif // MOTIONEXTRACTOR_H
