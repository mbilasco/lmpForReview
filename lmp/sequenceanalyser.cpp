#include <sys/stat.h>
#include "sequenceanalyser.h"

int SequenceAnalyser::_bins, SequenceAnalyser::_maxEmo;
int SequenceAnalyser::_L, SequenceAnalyser::_Lr, SequenceAnalyser::_D, SequenceAnalyser::_P,SequenceAnalyser::_E,SequenceAnalyser::_M,SequenceAnalyser::_V;
double SequenceAnalyser::_B, SequenceAnalyser::_Dr, SequenceAnalyser::_min, SequenceAnalyser::_max;
vector<FaceROI> SequenceAnalyser::_meshs;
vector< vector<double> > SequenceAnalyser::_motionVector, SequenceAnalyser::_cumul ;
Mat SequenceAnalyser::flow, SequenceAnalyser::masque, SequenceAnalyser::frame;


SequenceAnalyser::SequenceAnalyser()
{

}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

void SequenceAnalyser::analyse(int emotion, string sPath, string sSubject, string sSequence,  int L, double Dr, double B, int E, int M, int V, int P, int bin, string ofMethod)
{
	_emotion=emotion;
	_path = sPath;
	_subject = sSubject;
	_sequence = sSequence;
	_fullpath = sPath + sSubject + "/";

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

    if (strcmp(ofMethod.c_str(),"DIS")==0)
    	optFlowExtractor = optflow::createOptFlow_DIS();
    else if (strcmp(ofMethod.c_str(),"DeepFlow")==0)
    	optFlowExtractor = optflow::createOptFlow_DeepFlow();
    else if (strcmp(ofMethod.c_str(),"SparseToDense")==0)
    	optFlowExtractor = optflow::createOptFlow_SparseToDense();
    else { //default
    	ofMethod="farneback";
    	optFlowExtractor = optflow::createOptFlow_Farneback();
	}

    sequencemanager _sequencemanager;
	_landmarks = _sequencemanager.getLandmarksOneMilliseconde(_fullpath, _sequence);
	_sequenceLength = _sequencemanager.countImage(_fullpath + _sequence);
	_data = _sequencemanager.getImgsFlowsAndLMP(_fullpath, _sequence, ofMethod, _sequenceLength);


	if (_sequenceLength<2)
	std::cerr<<"incomplete sequence : size = "<<_sequenceLength<<"\n";
	if (_sequenceLength<2)
	return ;

	runAnalysis();
}

/**
 * @brief parcours de chaque image pour analyser l'ensemble de la sequence
 */

void SequenceAnalyser::runAnalysis()
{
    Mat oldFrame, newFrame;
    vector<Point2f> oldFrameLands,newFrameLands;
    int max = _sequenceLength - 1;

    Mat newF, oldF = imread(_data[0][0]);
    _coherency = Mat::zeros(oldF.cols, oldF.rows, CV_8UC1);
    struct stat buffer;

    _cumul.clear();
    for(int i = 0; i < _num_mesh; i++)
    {
        vector<double> v(_bins,0);
        _cumul.push_back(v);
    }

    std::cerr<<"run analyses from 0 to "<<max<<" frames with emo type "<<_emotion<<"\n";
    if(_emotion > 0)
    {
        for(int i = 0; i < max-1; i++)
        {
        	std::cerr<<i<<" ";
        	newF = imread(_data[i][1]);
        	//printf("trying to load OF from %s\n",_data[i][2].c_str());
        	if (stat (_data[i][2].c_str(), &buffer) == 0) {
        		 printf("loading OF from %s\n",_data[i][2].c_str());
				 flow = optflow::readOpticalFlow(_data[i][2]);
			 } else {
				printf("computing OF from %s %s\n",_data[i][0].c_str(),_data[i][1].c_str());
				oldF=imread(_data[i][0]);
#ifdef SHOW_IMG
				imshow("old",oldF);
				imshow("new",newF);
				waitKey(1);
#endif
				cvtColor(oldF,oldF,CV_BGR2GRAY);
				cvtColor(newF,newF,CV_BGR2GRAY);
				optFlowExtractor->calc(oldF,newF,flow);
#ifdef	SAVE_OPT_FLOW
				//printf("extract OF \n");
				optflow::writeOpticalFlow(_data[i][2],flow);
#endif
        	 }
        	 extractMotion(newF,flow,_landmarks[0]);
#ifdef SAVE_LMP
				 	 saveLMP(_motionVector, _data[i][3]);
#endif
				for (int j=0; j<_motionVector.size();j++)
					for (int k=0; k<_motionVector.at(j).size(); k++)
						_cumul.at(j).at(k)+=_motionVector.at(j).at(k);
        }
    }
#ifdef SAVE_LMP
    	string seqLMP=_path + _subject + "/" + _sequence+".lmp";
    	saveLMP(_cumul,seqLMP);
#endif
}
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

void SequenceAnalyser::extractMotion(Mat inMat, Mat inFlow, vector<Point2f> landmarks)
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

void SequenceAnalyser::saveLMP(vector<vector<double>> motionVect, string filename)
{
		stringstream vaux;
		vaux<<_emotion;

		for(int i = 0; i < motionVect.size(); i++)
		{
				for(int j = 0; j < motionVect[0].size(); j++)
				{
					vaux << " ";
					vaux << motionVect[i][j];
				}
		}
		vaux << "\n";

		//printf("writing %s to %s\n",vaux.str().c_str(), filename.c_str());
		ofstream fichier(filename.c_str(), ios::out );
		if(fichier.good())
		{
				fichier << vaux.str();
				fichier.close();
		}
}

vector<float> SequenceAnalyser::getMotionVector()
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

vector<vector<double> > SequenceAnalyser::getRegionMotionVector()
{
    return _motionVector;
}

vector<vector<double> > SequenceAnalyser::getCumulatedRegionMotionVector()
{
    return _cumul;
}
