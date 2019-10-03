
#include "lmp/sequenceanalyser.h"

void writeGMDForSVM(int emotion, vector<vector<double>> motionVect, ofstream &fichier);

int main(int argc, char *argv[])
{
    vector<string> subjectFolder, expressionFolders;

    char path_str[2000];
    sprintf(path_str,"%s/",argv[9]);

    char emo_cfg[200];
    strcpy(emo_cfg,argv[10]);

    string path=path_str;

    int L,E,M,V,P,bin;
    float Dr,B;


    L=atoi(argv[1]); Dr=atof(argv[2]); B=atof(argv[3]);
    E=atoi(argv[4]); M=atoi(argv[5]); V=atoi(argv[6]);
    P=atoi(argv[7]); bin=atoi(argv[8]);
    char ofMethod[20];
    if (argc>11)
    	strcpy(ofMethod,argv[11]);
    else
    	strcpy(ofMethod,"farneback");

    string attribute;

    string csvFile=path+emo_cfg;
	ifstream myStream(csvFile.c_str());

	printf("loading expression from %s \n",csvFile.c_str());

	getline(myStream, attribute, '\n');
	string subject, sequence;
	int emotion;

	SequenceAnalyser _seqAnalyser;
	char _nameSaveCStr[2000];

#ifdef SAVE_LMP
	char _nameSave[2000];
	time_t now = time(0);
	sprintf(_nameSave,"GMD_%s_L%d_Dr%.2f_B%.2f_E%d_M%d_V%d_P%d_Bin%d_%ld.txt",emo_cfg,L,Dr,B,E,M,V,P,bin,now);
	ofstream fichier(_nameSave, ios::out );
#endif

	while(!myStream.eof())
	{
		getline(myStream, subject, ';');
		if (subject.size()==0)
			break;
		printf("%s - ",subject.c_str());

		getline(myStream, sequence, ';');
		printf("%s - ",sequence.c_str());

		getline(myStream, attribute, '\n');
		emotion = atoi(attribute.c_str());
		printf("%d\n",emotion);

		_seqAnalyser.analyse(emotion,path,subject,sequence,L,Dr,B,E,M,V,P,bin,ofMethod);
#ifdef SAVE_LMP
		writeGMDForSVM(emotion, _seqAnalyser.getCumulatedRegionMotionVector(), fichier);
#endif
    }

	//fichier.close();
    return 0;
}

void writeGMDForSVM(int emotion, vector<vector<double>> motionVect, ofstream &fichier)
{
		stringstream vaux;
		vaux<<emotion;

		for(int i = 0; i < motionVect.size(); i++)
		{
				for(int j = 0; j < motionVect[0].size(); j++)
				{
					vaux << " " ;
					vaux << ((i*motionVect[0].size())+j+1);
					vaux << ":";
					vaux << motionVect[i][j];
				}
		}
		vaux << "\n";
		fichier << vaux.str();
}
