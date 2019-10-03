#include "patch.h"

Patch::Patch()
{
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief initialiser les parametres du patch
 * @param x (int) : position X du centre du patch sur l'image
 * @param y (int) : position y du centre du patch sur l'image
 * @param flow (Mat) : matrice du flux optique
 * @param bins (int) : nombre de bin caracterisant l'histogramme
 * @param iteration (int) : le compteur d'iteration en cours au sein du LMP
 * @param lambda (int) : dimension du patch
 * @param delta (int) : taux de recouvrement entre deux regions (compris entre 0 et 1)
 * @param batta (double) : coefficient de bhattacharrya pour estimer la similitude entre deux histogrammes de direction (compris entre 0 et 1)
 * @param ecart (int) : intensite minimale accepte pour caracteriser une direction principale
 * @param mean (int) : etalement maximal accepte pour caracterisuer une direction principale
 * @param vari (int) : variation maximale accepte entre deux bins successifs (V = 5 equivaut a une pentes comprise entre 0% et 95%)
 */

void Patch::initialize(int x, int y, Mat flow, int bins, int iteration, double min, int lambda, int delta, double batta, int ecart, int mean, int vari)
{
    _X = x;
    _Y = y;
    _flow = flow;
    _bins = bins;
    _iterationCount = iteration;

    _dims = lambda;
    _jump = delta;
    _batta = batta;
    _ecart = ecart;
    _mean = mean;
    _vari = vari;

    _distribution.clear();

    Rect ROI;
    ROI.x = x - (lambda/2);
    ROI.y = y - (lambda/2);
    ROI.width = lambda;
    ROI.height = lambda;

    computeLocalDistribution(flow(ROI));
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief calculer la distribution locale au sein du patch
 * @param flux (Mat) : matrice de flux optique
 */

void Patch::computeLocalDistribution(Mat flux)
{
    vector< vector<double> > fiveMagnitudeHistogram;

    Mat flow = cv::Mat::zeros(flux.size(), flux.type());
    flux.copyTo(flow);

    Mat flowTab[2];
    Mat angle,magnitude;
    split(flow,flowTab);
    cartToPolar((flowTab[0]),-(flowTab[1]),magnitude,angle);
    Mat hsvTab[3];
    hsvTab[0] = (angle*180/M_PI);
    hsvTab[1] = Mat::ones(flow.rows,flow.cols,CV_32FC1);
    hsvTab[2] = magnitude;

    double min, max;
    max = 10.0;

    for(double i = 0; i < max; i+=0.2)
    {
        Mat M = Mat::ones(hsvTab[0].size(), hsvTab[0].type());
        Mat Mi = M * -1;

        for ( int y = 0; y < flow.cols; y++ )
        {
            for ( int x = 0; x < flow.rows; x++ )
            {
                if(i == 0.0)
                    min = 0.01;
                else
                    min = i;

                if(hsvTab[2].at<float>(y,x) >= min && hsvTab[2].at<float>(y,x) < max)
                    Mi.at<float>(y,x) = hsvTab[0].at<float>(y,x);
            }
        }

        fiveMagnitudeHistogram.push_back(drawCurveOnHistogram(Mi));
    }

    computeCoherentMotion(fiveMagnitudeHistogram);

    normalizeDistribution();

    for(int i = 0; i < _bins; i++)
        _filteredDistribution.push_back(0);
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne l'histogramme de B bin de la matrice passee en parametre
 * @param values (Mat) : matrice de mouvement
 * @return vecteur de double correspondant a l'histogramme de direction
 */

vector<double> Patch::drawCurveOnHistogram(Mat values)
{
    vector<double> distribution;

    /// Establish the number of bins
    int histSize = _bins;
    float range[] = {0 , 360 } ;

    const float* histRange = { range };

    Mat b_hist;

    int hist_h = 100;

    /// Compute the histograms:
    calcHist( &values, 1, 0, Mat(), b_hist, 1, &histSize, &histRange);

    normalize(b_hist, b_hist, 0, hist_h, NORM_MINMAX, -1, Mat() );

    for( int i = 0; i < histSize-1; i++ )
    {
        if(b_hist.at<float>(i) <= 10)
            b_hist.at<float>(i) = 0;
    }

    /// Draw for each channel
    for( int i = 0; i < histSize; i++ )
    {
        distribution.push_back(b_hist.at<float>(i));
    }

    return distribution;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief calculer l'occurence du mouvement en fonction des trois couches de magnitudes 0-33-66-100
 * @param fiveMagnitudeHistogram : contient la distribution des directions pour chaque couche de magnitude
 */

void Patch::computeCoherentMotion(vector< vector<double> > fiveMagnitudeHistogram)
{ 
    double compteur;

    for(int i = 0; i < fiveMagnitudeHistogram[0].size(); i++) //i = bins
    {
        compteur = 0;
        vector<int> tmp(3,0);

        for(int j = 0; j < fiveMagnitudeHistogram.size(); j++)
        {
            if(fiveMagnitudeHistogram[j][i] > 66)
                tmp[0]++;
            else if(fiveMagnitudeHistogram[j][i] > 33)
                tmp[1]++;
            else if(fiveMagnitudeHistogram[j][i] > 0)
                tmp[2]++;
        }

        compteur = (tmp[0] * 100) + (tmp[1] * 10) + (tmp[2] * 1);

        _distribution.push_back(compteur);
    }
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief Patch::divergence
 * @param tmpFocus
 * @param focus
 * @param norma
 * @param pas
 * @param variance
 */

void Patch::divergence(vector<int>* tmpFocus, int focus, vector<int> norma, int pas, int variance)
{
    focus+=pas;

    if(focus<0)
        focus = norma.size()-1;
    else if(focus>norma.size())
        focus = 0;

    if(norma[focus]>=variance)
    {
        tmpFocus->push_back(focus);
        if(tmpFocus->size() < _mean)
            divergence(tmpFocus,focus,norma,pas,variance);
    }
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief normaliser l'histogramme de direction du patch
 */

void Patch::normalizeDistribution()
{
    double maxi = *max_element(_distribution.begin(), _distribution.end());

    for(int i = 0; i < _distribution.size();i++)
    {
        if(i >= 0)
            _normaDistribution.push_back((_distribution[i] * 100.0) / maxi);
        else
            _normaDistribution.push_back(0);
    }
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief allow to know if the local distribution is consistente for spread the motion
 * at least one main direction found in the filtered histogram
 * @return True if the local motion is coherent
 */

bool Patch::getCoherent()
{
    double maxi = *max_element(_distribution.begin(), _distribution.end());

    if(maxi >= _ecart)
    {
        vector< vector<int> > indices;

        for(int i = 0; i < _distribution.size(); i++ )
        {
            vector<int> tmp;
            tmp.push_back(_distribution[i]);
            tmp.push_back(i);
            indices.push_back(tmp);
        }

        sort(indices.begin(),indices.end());

        int k = 3;

        int variance = _vari;

        for(int i = 1; i < k+1; i++)
        {
            if(_normaDistribution[indices[indices.size()-i][1]] >= variance)
            {
                vector<int> tmpFocus;

                divergence(&tmpFocus,indices[indices.size()-i][1], _normaDistribution, -1, variance);

                divergence(&tmpFocus, indices[indices.size()-i][1], _normaDistribution, +1, variance);

                if(tmpFocus.size() >= 0 && tmpFocus.size() < _mean)
                {
                    for(int f = 0; f < tmpFocus.size(); f++)
                    {
                        _filteredDistribution[tmpFocus[f]] = _distribution[tmpFocus[f]];
                    }
                    _filteredDistribution[indices[indices.size()-i][1]] = _distribution[indices[indices.size()-i][1]];
                }
            }
        }

        bool state = false;

        for(int i = 0; i < _filteredDistribution.size(); i++)
        {
            if(_filteredDistribution[i] > 0)
            {
                state = true;
                break;
            }
        }

        if(state)
        {
            return true;
        }
    }

    return false;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief verifie la similitude entre deux patch en calculant la distance de Bhattacharrya entre les deux distributions
 * @param distribution (vector double) : distribution d'un patch voisin
 * @return True si la distance est suffisament proche (en desous du seuil toléré)
 */

bool Patch::getCoherent(vector<double> distribution)
{
    vector<float> H1(distribution.begin(), distribution.end());
    vector<float> H2(_distribution.begin(), _distribution.end());

    double cpt = compareHist(H1,H2,CV_COMP_BHATTACHARYYA);

    if(cpt <= _batta)
        return true;
    else
        return false;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne la distribution du mouvement au sein du patch (LMR)
 * @return
 */

vector<double> Patch::getDistribution()
{
    return _distribution;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne la distribution filtré du mouvement au sein du patch (LMR)
 * @return
 */

vector<double> Patch::getFilteredDistribution()
{
    return _filteredDistribution;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne l'ecart entre deux régions
 * @return
 */

int Patch::getJump()
{
    return _jump;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne l'indice de l'itération en cours au sein du LMP
 * @return
 */

int Patch::getIteration()
{
    return _iterationCount;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief mettre à jour l'iteration
 * @param ite
 */

void Patch::setIteration(int ite)
{
    _iterationCount = ite;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief recuperer la position X du centre du patch
 * @return
 */

int Patch::getX()
{
    return _X;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief recuperer la position Y du centre du patch
 * @return
 */

int Patch::getY()
{
    return _Y;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief recuperer la dimension du patch
 * @return
 */

int Patch::getDims()
{
    return _dims;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief dessine la distribution du mouvement au sein du patch
 * @param picture (Mat) : image sur laquelle dessiner
 * @param bin (int) : nombre de bin de l'histogramme
 * @return
 */

void Patch::drawPatch(Mat &picture, int bin)
{
    //if we want to draw the patch
    //Rect _patchROI = Rect(_X-_dims,_Y-_dims,_dims*2,_dims*2);
    //rectangle(picture,_patchROI,Scalar(0,255,0));

    vector<double> norma;

    double maxi = *max_element(_filteredDistribution.begin(), _filteredDistribution.end());

    for(int i = 0; i < _filteredDistribution.size();i++)
    {
        if(i > 0)
            norma.push_back((_filteredDistribution[i] * 100.0) / maxi);
        else
            norma.push_back(0);
    }

    Point2f p = Point2f(_X,_Y);

    for(int i = 0; i < _filteredDistribution.size(); i++)
    {
        if(_filteredDistribution[i] > 0)
        {
            double angle = (i+1)*(360/bin);

            double drift = 0.01 * _filteredDistribution[i];

            double x = drift*cos(angle*CV_PI/180.0);
            double y = drift*sin(angle*CV_PI/180.0);

            Point2f q = Point2f(p.x + x, p.y - y);

            if(q.x < picture.cols-1 && q.y < picture.rows-1)
            {
                double val,R,G,B;

                if(angle >= 270) //B-G
                {
                    val = abs(angle-270);
                    G = 3 * val;
                    B = 255 - G;
                    R = 0;
                }
                else if(angle >= 180) //R-B
                {
                    val = abs(angle-180);
                    G = 0;
                    B = 3 * val;
                    R = 255-B;
                }
                else if(angle >= 90) //J-R
                {
                    val = abs(angle-90);
                    G = 255 - (3 * val);
                    B = 0;
                    R = 255;
                }
                else //G-J
                {
                    val = angle;
                    G = 255;
                    B = 0;
                    R = 3 * val;
                }

                line(picture, p, q, Scalar(B, G, R ),2);
            }
        }
    }
}
