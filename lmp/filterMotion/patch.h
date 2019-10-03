#ifndef PATCH_H
#define PATCH_H

#include <iostream>
#include <vector>
#include <string>

#include <cmath>
#include <ctime>
#include <cstdio>
#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/optflow.hpp>

using namespace std;
using namespace cv;

class Patch
{
public:
    Patch();

    void initialize(int x, int y, Mat flow, int bins, int iteration, double min, int lambda, int delta, double batta, int ecart, int mean, int vari);

    bool getCoherent();
    bool getCoherent(vector<double> distribution);

    int getIteration();
    void setIteration(int ite);

    int getX();
    int getY();
    int getJump();
    vector<double> getDistribution();
    vector<double> getFilteredDistribution();
    int getDims();

    void divergence(vector<int>* tmpFocus, int focus, vector<int> norma, int pas, int variance);

    vector<double> drawCurveOnHistogram(Mat values);
    void computeCoherentMotion(vector< vector<double> > fiveHistograme);

    void drawPatch(Mat &picture, int bin);

private:
    void computeLocalDistribution(Mat flux);
    void normalizeDistribution();

    int _dims = 20;//15
    int _jump = 15;//10

    double _batta;
    int _ecart, _mean, _vari;

    double _min;
    int _iterationCount;

    int _X, _Y, _bins;
    Mat _flow;

    vector<double> _distribution, _filteredDistribution;
    vector<int> _normaDistribution;
};

#endif // PATCH_H
