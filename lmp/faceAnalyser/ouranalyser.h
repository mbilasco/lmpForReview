#ifndef OURANALYSER_H
#define OURANALYSER_H

#include "faceanalyser.h"

class ourAnalyser : public faceAnalyser
{
public:
    ourAnalyser();


    void createVectorCumul(vector< vector<double> > cumul);


    void saveOccurenceCumulee(int emotion, string name, int subjectNumber);

    void initialize(int bin, int region);

private:
    vector< vector<double> > _histogram, _cumul;
};

#endif // OURANALYSER_H
