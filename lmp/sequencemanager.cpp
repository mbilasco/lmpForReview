#include "sequencemanager.h"
#include <sys/stat.h>

sequencemanager::sequencemanager()
{
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief return a list containing the path to each pair of consecutive image
 * 			in the sequence along with the optical flow
 * @param path (string) : path to the subject folder
 * @param file (string) : path to the sequence in the subject folder to be analysed
 * @param method (string) : optical flow method to use
 * @param nb (int) number of images in the folder
 * @return return a list containing the path to each pair of consecutive image
 * 			in the sequence along with the optical flow and the lmp filenames
 */

vector< vector<string> > sequencemanager::getImgsFlowsAndLMP(string path, string file, string method, int nb)
{
    string oldFrame = path + file + "/img_001.png";
    string newFrame, flow, lmp;
    vector<string> tmp;
    vector< vector<string> > paths;
    stringstream numb,ind;
    string flowDir= path + file + "_flow_"+ method;
    string lmpDir= path + file + "_lmp";
    struct stat buffer;

#ifdef SAVE_OPT_FLOW
    //printf("%s for storing optical flow data available ?\n",flowDir.c_str());

    if (stat (flowDir.c_str(), &buffer) != 0) {
    	string mkdirCmd="mkdir -p "+flowDir;
    	system(mkdirCmd.c_str());
    	printf("creating %s for recording optical flow data\n",flowDir.c_str());
    }
#endif
#ifdef SAVE_LMP
    //printf("%s for storing lmp data available ?\n",lmpDir.c_str());

    if (stat (lmpDir.c_str(), &buffer) != 0) {
    	string mkdirCmd="mkdir -p "+lmpDir;
    	system(mkdirCmd.c_str());
    	printf("creating %s for recording lmp data\n",lmpDir.c_str());
    }
#endif

    char buff[2000];
    for(int i = 1; i < nb; i++)
    {
        numb.str(string());
        ind.str(string());
        numb << i+1;
        ind << i;

        sprintf(buff,"%s%s/img_%03d.png",path.c_str(),file.c_str(),i+1);
        newFrame=buff;
        tmp.push_back(oldFrame);
        tmp.push_back(newFrame);

        sprintf(buff,"%s%s_flow_%s/flow_%03d.flo",path.c_str(),file.c_str(),method.c_str(),i);
        flow=buff;
        tmp.push_back(flow);

        sprintf(buff,"%s%s_lmp/lmp_%03d.csv",path.c_str(),file.c_str(),i);
        lmp=buff;
        tmp.push_back(lmp);

        paths.push_back(tmp);
        tmp.clear();

        oldFrame = newFrame;
    }

    return paths;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief counts the number of images in a folder
 * @param file (string) : path to the folder to analyze
 * @return (int i) number of images
 */

int sequencemanager::countImage(string file)
{
    DIR *dp;
    int i = 0;
    struct dirent *ep;

    dp = opendir (file.c_str());

    if (dp != NULL)
    {
        while ((ep = readdir (dp)))
        {
            i++;
        }

        (void) closedir (dp);
    }
    else
        perror ("Couldn't open the directory");

    i -= 2;

    return i;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief load the landmark annotations for a sequence
 * @param path (string) : path to the annotation file
 * @param sequence (string) : name of the folder corresponding to the analysed folder
 * @return vecteur de points (68pts par images), avec n images
 */

vector< vector<Point2f> > sequencemanager::getLandmarksOneMilliseconde(string path, string sequence)
{
    string sName = path + "/omlands.csv";
    string attribute;
    vector<Point2f> linePoints;
    vector< vector<Point2f> > landmarks;

    ifstream myStream(sName.c_str());

    printf("loading landmarks from %s ",sName.c_str());

    if (!myStream.good())
    {
    	printf("Unable to load landmarks stream \n");
    	return landmarks;
    }

    while(!myStream.eof())
    {
        linePoints.clear();

        getline(myStream, attribute, ';');

        printf(".");

        if(attribute.c_str() == sequence)
        {

            for(int i = 0; i < 67; i++)
            {
                getline(myStream, attribute, ';');
                float px = atof(attribute.c_str()) / 2.0;
                getline(myStream, attribute, ';');
                float py = atof(attribute.c_str()) / 2.0;
                linePoints.push_back(Point(px,py));
            }

            getline(myStream, attribute, ';');
            float px = atof(attribute.c_str()) / 2.0;
            getline(myStream, attribute, '\n');
            float py = atof(attribute.c_str()) / 2.0;
            linePoints.push_back(Point(px,py));

            landmarks.push_back(linePoints);
        }
        else
        {
            getline(myStream, attribute, '\n');
        }
    }

    myStream.close();
    printf("loaded\n");
    return landmarks;
}
