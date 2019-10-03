#ifndef CSVMANAGER_H
#define CSVMANAGER_H

#include "lmp_common_header.h"
#include <sys/types.h>
#include <dirent.h>

class sequencemanager
{
public:
	sequencemanager();

    vector< vector<Point2f> > getLandmarksOneMilliseconde(string path, string sequence);
    int getExpressionAnnot(string csvFile, string subject, string sequence);
    vector< vector<string> > getImgsFlowsAndLMP(string path, string file, string method, int nb);
    int countImage(string file);

private:


    string _path, _file, _completPath, _method;
    vector< vector<int> > _annotation;
    vector<int> _class;
};

#endif // CSVMANAGER_H
