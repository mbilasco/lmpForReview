#ifndef DATASETANALYSER_H
#define DATASETANALYSER_H

#include "lmp_common_header.h"

#include "faceSeparator/ourroi.h"
#include "faceSeparator/faceroi.h"
#include "motionextractor.h"
#include "faceAnalyser/ouranalyser.h"
#include "sequencemanager.h"

class SequenceAnalyser
{
public:
    SequenceAnalyser();

    void analyse(int emotion, string sPath, string sSubject, string sSequence, int L, double Dr, double B, int E, int M, int V, int P, int bin, string ofMethod);

    /**
     * @brief execute the lmp extractor code in several threads, one per region
     * @param i (int) : number of the region in the facial segmentation model
     */
    static void extractRegionMotion(int i)
    {
        motionExtractor _localMotion;

        _meshs[i].drawLine(frame);

        _localMotion.patchParam(_Lr, _D, _B, _E, _M, _V);

        vector<double> motion = _localMotion.OurExtractMotion(flow, masque, _bins, _meshs[i].getMiddle(), _P, _meshs[i].getMin());

        //if(_affichage)
            _localMotion.drawDistribution(frame);
            //_localMotion.drawFlow(frame);
        _motionVector[i] = motion;
    }

    void extractMotion(Mat inMat, Mat flow, vector<Point2f> landmarks);
    void runAnalysis();
    Mat getFrame();
    vector<float> getMotionVector();
    vector<vector<double> > getRegionMotionVector();
    void saveLMP(vector<vector<double>> cumul, string filename);
    vector< vector<double> > getCumulatedRegionMotionVector();


private:
    vector< vector<Point2f> > _landmarks;
    vector< vector<string> > _data;

    Ptr<DenseOpticalFlow> optFlowExtractor;

    string _path, _subject, _sequence, _fullpath, _emo_cfg, _lmp_dir_suffix;
    string _nameSave;

    int _emotion;
    int _subjectNumber,_sequenceLength;

    ourROI _ourSeparator;
    ourAnalyser _ourAnalyse;

    Mat _coherency;

    int _num_mesh;

    static int _bins,_maxEmo;
    static int _L, _Lr, _D, _P, _E, _M, _V;
    static double _B, _Dr, _min, _max;
    static vector<FaceROI> _meshs;
    static vector< vector<double> > _motionVector;
    static Mat flow, masque, frame;
    static vector< vector<double> > _cumul;


};

#endif // DATASETANALYSER_H
