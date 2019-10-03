#include "datasetanalyser.h"

int DatasetAnalyser::_bins, DatasetAnalyser::_maxEmo;
int DatasetAnalyser::_L, DatasetAnalyser::_Lr, DatasetAnalyser::_D, DatasetAnalyser::_P,DatasetAnalyser::_E,DatasetAnalyser::_M,DatasetAnalyser::_V;
double DatasetAnalyser::_B, DatasetAnalyser::_Dr, DatasetAnalyser::_min, DatasetAnalyser::_max;
vector<FaceROI> DatasetAnalyser::_meshs;
vector< vector<double> > DatasetAnalyser::_motionVector;
Mat DatasetAnalyser::flow, DatasetAnalyser::masque, DatasetAnalyser::frame;


DatasetAnalyser::DatasetAnalyser()
{

}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

void DatasetAnalyser::initialize(int L, double Dr, double B, int E, int M, int V, int P, int bin)
{
    _num_mesh = 25; //nombre de region du visage
    _ourAnalyse.initialize(bin, _num_mesh);

    _L = L;
    _B = B;
    _Dr = Dr;
    _E = E;
    _M = M;
    _V = V;
    _P = P;
    _bins = bin;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

void DatasetAnalyser::extractMotion(Mat inMat, Mat inFlow, vector<Point2f> landmarks)
{   
    _motionVector.clear();

    for(int i = 0; i < _num_mesh; i++)
    {
        vector<double> v(_bins,0);
        _motionVector.push_back(v);
    }

    frame = inMat;
    flow = inFlow;

    _ourSeparator.createROI(landmarks);

    _meshs = _ourSeparator.getMeshs();

    _Lr = _ourSeparator.getPatchSize(_L);
    //_D = _L - (_Dr * _Lr);
    _D = 1;

    masque = _ourSeparator.delimitateFace(flow);

    thread *tt = new thread[_num_mesh - 1];

    for(int i = 0; i < _num_mesh - 1; ++i)
    {
        tt[i] = thread(extractRegionMotion,i);
    }

    for(int i = _num_mesh - 1; i < _num_mesh; ++i)
    {
        extractRegionMotion(i);
    }

    for(int i = 0; i < _num_mesh - 1; ++i)
    {
        tt[i].join();
    }

    delete [] tt;
}


Mat DatasetAnalyser::getFrame()
{
    return frame;
}

vector<float> DatasetAnalyser::getMotionVector()
{
    vector<float> simpleVector(_motionVector.size()*_motionVector[0].size(),0);
    int cpt = 0;

    for(int i = 0; i < _motionVector.size(); i++)
    {
        for(int j = 0; j < _motionVector[0].size(); j++)
        {
            simpleVector[cpt] += _motionVector[i][j];
            cpt++;
        }
    }

    return simpleVector;
}

vector<vector<double> > DatasetAnalyser::getRegionMotionVector()
{
    return _motionVector;
}
