#include "faceanalyser.h"

faceAnalyser::faceAnalyser()
{
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

void faceAnalyser::createVector(vector<vector<double> > facialMotion, Mat flow, vector<FaceROI> meshs, Mat frame)
{
    //nothing
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne le vecteur de caracteristique
 * @return
 */

vector< vector<double> > faceAnalyser::getVector()
{
    return _meanVectorSequence;
}
