#ifndef DATASETANALYSER_H
#define DATASETANALYSER_H

#include "../global_header.h"

#include "faceSeparator/ourroi.h"
#include "faceSeparator/faceroi.h"
#include "motionextractor.h"
#include "faceAnalyser/ouranalyser.h"

class DatasetAnalyser
{
public:
    DatasetAnalyser();

    void initialize(int L, double Dr, double B, int E, int M, int V, int P, int bin);

    /**
     * @brief code execute dans les thread pour extraire le lmp dans une region
     * @param indice (int) : numero de la region du framework facial
     */
    static void extractRegionMotion(int indice)
    {
        motionExtractor _localMotion;

        _meshs[indice].drawLine(frame);

        _localMotion.patchParam(_Lr, _D, _B, _E, _M, _V);

        vector<double> motion = _localMotion.OurExtractMotion(flow, masque, _bins, _meshs[indice].getMiddle(), _P, _meshs[indice].getMin());

        //if(_affichage)
            _localMotion.drawDistribution(frame);
            //_localMotion.drawFlow(frame);

        _motionVector[indice] = motion;
    }

    void extractMotion(Mat inMat, Mat flow, vector<Point2f> landmarks);

    Mat getFrame();
    vector<float> getMotionVector();
    vector<vector<double> > getRegionMotionVector();

private:
    ourROI _ourSeparator;
    ourAnalyser _ourAnalyse;

    int _num_mesh;

    static int _bins,_maxEmo;
    static int _L, _Lr, _D, _P, _E, _M, _V;
    static double _B, _Dr, _min, _max;
    static vector<FaceROI> _meshs;
    static vector< vector<double> > _motionVector;
    static Mat flow, masque, frame;
};

#endif // DATASETANALYSER_H
