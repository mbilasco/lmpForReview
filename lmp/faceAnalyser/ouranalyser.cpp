#include "ouranalyser.h"

ourAnalyser::ourAnalyser()
{

}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief initialise la taille du vecteur de caracteristique
 * @param bin (int) : nombre de bin de l'histogramme
 * @param region (int) : nombre de region du framexork facial
 */

void ourAnalyser::initialize(int bin, int region)
{
    for(int i = 0; i < region; i++)
    {
        vector<double> tmp(bin,0);
        _histogram.push_back(tmp);
        _cumul.push_back(tmp);
    }
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief cumuler le vecteur du LMP avec le vecteur de l'image precedante
 * @param cumul
 */

void ourAnalyser::createVectorCumul(vector< vector<double> > cumul)
{
    for(int i = 0; i < cumul.size(); i++)
    {
        for(int j = 0; j < cumul[i].size(); j++)
        {
            if(cumul[i][j] > 0)
            {
                _cumul[i][j] += cumul[i][j];
            }
        }
    }
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief exporter le vecteur de caracteristique dans un fichier
 * @param emotion (int) : classe correspondant a l'emotion
 * @param name (string) : chemin et nom du fichier à sauvegarder
 * @param subjectNumber (int) : numero du sujet si l'on veut faire une leave-one-subjet-out
 */

void ourAnalyser::saveOccurenceCumulee(int emotion, string name, int subjectNumber)
{
    stringstream C,N;
    N << subjectNumber;
    C << emotion;

    int val = 0;

    //string filestream = N.str() + " " + C.str() + " ";
    string filestream = C.str() + " ";

    for(int i = 0; i < _cumul.size(); i++)
    {
        for(int j = 0; j < _cumul[0].size(); j++)
        {
            val++;
            stringstream R1,V1;
            R1 << val;
            V1 << _cumul[i][j];

            filestream += R1.str() + ":" + V1.str() + " ";
            //filestream += V1.str() + " ";
        }
    }

    ofstream fichier(name.c_str(), ios::out | ios::app);
    if(fichier)
    {
        fichier << filestream + "\n";
        fichier.close();
    }
}
