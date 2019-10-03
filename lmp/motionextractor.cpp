#include "motionextractor.h"

motionExtractor::motionExtractor()
{
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne l'histogramme de B bin de la matrice passee en parametre
 * @param values (Mat) : matrice de mouvement
 * @return vecteur de double correspondant a l'histogramme de direction
 */

vector<double> motionExtractor::drawCurveOnHistogram(Mat values)
{
    vector<double> distribution;

    /// Establish the number of bins
    int histSize = _bins;
    float range[] = {0 , 360 } ;

    const float* histRange = { range };

    Mat b_hist;

    /// Compute the histograms:
    calcHist( &values, 1, 0, Mat(), b_hist, 1, &histSize, &histRange);

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
 * @brief initialiser les parametres pour construire le vecteur
 * @param lambda (int) : taille en pourcentage des regions a analyser
 * @param delta (double) : taux de recouvrement entre deux regions (compris entre 0 et 1)
 * @param batta (double) : coefficient de bhattacharrya pour estimer la similitude entre deux histogrammes de direction (compris entre 0 et 1)
 * @param ecart (int) : intensite minimale accepte pour caracteriser une direction principale
 * @param mean (int) : etalement maximal accepte pour caracterisuer une direction principale
 * @param vari (int) : variation maximale accepte entre deux bins successifs (V = 5 equivaut a une pentes comprise entre 0% et 95%)
 */

void motionExtractor::patchParam(int lambda, int delta, double batta, int ecart, int mean, int vari)
{
    _lambda = lambda;
    _delta = delta;
    _batta = batta;
    _ecart = ecart;
    _mean = mean;
    _vari = vari;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief extraire le mouvement a partir d'un epicentre
 * @param flux (Mat) : matrice du flux optique
 * @param mask (Mat) : masque permettant de mettre a 0 le flux en dehors des landmarks
 * @param bins (int) : nombre de bin pour creer les histogrammes
 * @param center (Point2f) : position de l'epicentre sur l'image
 * @param iteration (int) : nombre d'itération de l'analyse autour de l'épicentre
 * @return retourne un vecteur de double correspondant à l'histogramme de direction
 */

vector<double> motionExtractor::OurExtractMotion(Mat flux, Mat mask, int bins, Point2f center, int iteration, double min)
{
    _bins = bins;
    _allPatch.clear();
    _min = min;
    _center = center;

    _minPatch = 3;

    _coherencyMap = Mat::zeros(flux.rows,flux.cols,CV_8UC1);
    mask.copyTo(_coherencyMap);

    Patch epicentre;
    epicentre.initialize(center.x, center.y, flux, bins, iteration, _min, _lambda, _delta, _batta, _ecart, _mean, _vari);

    if(epicentre.getCoherent())
    {
        _allPatch.push_back(epicentre);
        _coherencyMap.at<uchar>(epicentre.getY(),epicentre.getX()) = 255;
        rectangle(_coherencyMap,Rect(Point(epicentre.getX()-1,epicentre.getY()-1),Point(epicentre.getX()+1,epicentre.getY()+1)),Scalar(255),-1);
        diffusionMotion(epicentre, flux);
    }

    vector<double> cumul(bins,0);

    if(_allPatch.size() > _minPatch)
    {
        for(int i = 0; i < _allPatch.size(); i++)
        {
            vector<double> distrib = _allPatch[i].getFilteredDistribution();
            for(int j = 0; j < cumul.size(); j++)
            {
                cumul[j] += distrib[j];
            }
        }
    }

    _cumul = cumul;

    return cumul;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief motionExtractor::pertinentMotion
 * @param angle
 * @param cumul
 * @return
 */

bool motionExtractor::pertinentMotion(float angle, vector<double> cumul)
{
    int bin = floor(angle / (360.0/(double)cumul.size()));
    if(cumul[bin] >= 1)
        return true;
    else
        return false;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief permet d'analyser le mouvement dans le voisinage d'un patch (8 connexes)
 * @param previousPatch (Patch) : Patch parent correspondant à l'iteration precedante
 * @param flow (Mat) : matrice du flux optique
 */

void motionExtractor::diffusionMotion(Patch previousPatch, Mat flow)
{
    if(previousPatch.getIteration() > 0)
    {
        previousPatch.setIteration(previousPatch.getIteration()-1);

        patchNeighbor(previousPatch.getX()+previousPatch.getJump(), previousPatch.getY(), flow, previousPatch);
        patchNeighbor(previousPatch.getX()+previousPatch.getJump(), previousPatch.getY()-previousPatch.getJump(), flow, previousPatch);
        patchNeighbor(previousPatch.getX(), previousPatch.getY()-previousPatch.getJump(), flow, previousPatch);
        patchNeighbor(previousPatch.getX()-previousPatch.getJump(), previousPatch.getY()-previousPatch.getJump(), flow, previousPatch);
        patchNeighbor(previousPatch.getX()-previousPatch.getJump(), previousPatch.getY(), flow, previousPatch);
        patchNeighbor(previousPatch.getX()-previousPatch.getJump(), previousPatch.getY()+previousPatch.getJump(), flow, previousPatch);
        patchNeighbor(previousPatch.getX(), previousPatch.getY()+previousPatch.getJump(), flow, previousPatch);
        patchNeighbor(previousPatch.getX()+previousPatch.getJump(), previousPatch.getY()+previousPatch.getJump(), flow, previousPatch);
    }
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief analyser le mouvement entre deux patchs voisins
 * @param X (int) : position X du centre du patch a analyser
 * @param Y (int) : position Y du centre du patch a analyser
 * @param Flow (Mat) : matrice de flux optique
 * @param previousPatch (Patch) : patch Parent de l'iteration precedante
 */

void motionExtractor::patchNeighbor(int X, int Y, Mat Flow, Patch previousPatch)
{
    if(_coherencyMap.at<uchar>(Y,X) == 0)
    {
        if(X < Flow.cols - (previousPatch.getDims()*2) && X > previousPatch.getDims()*2)
        {
            if(Y > previousPatch.getDims()*2 && Y < Flow.rows - (previousPatch.getDims()*2))
            {
                Patch childPatch;
                childPatch.initialize(X,Y,Flow, _bins,previousPatch.getIteration(), _min, _lambda, _delta, _batta, _ecart, _mean, _vari);

                if(childPatch.getCoherent(previousPatch.getDistribution()))
                {                   
                    _coherencyMap.at<uchar>(Y,X) = 255;
                    rectangle(_coherencyMap,Rect(Point(X-1,Y-1),Point(X+1,Y+1)),Scalar(255),-1);
                    if(childPatch.getCoherent())
                    {
                        diffusionMotion(childPatch, Flow);
                    }
                    _allPatch.push_back(childPatch);
                }
            }
        }
    }
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne l'histogramme de direction cumulés du LMP analysé
 * @return vecteur de double correspondant à l'histogramme de direction cumulés
 */

vector<double> motionExtractor::getOccurences()
{
    return _cumul;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief dessine le flux optique de chaque Patch contenu au sein du LMP
 * @param picture (Mat) : image sur laquelle sont dessine les directions des LMP
 */

void motionExtractor::drawFlow(Mat &picture)
{
    if(_allPatch.size() > _minPatch)
    {
        for(int i = 0; i < _allPatch.size(); i++)
        {
            _allPatch[i].drawPatch(picture,_bins);
        }
    }
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief permet de recuperer la carte de cohérence du LMP
 * cette carte a pour but de ne pas traiter des patchs deja analyse
 * @return retourne la carte de cohérence du LMP
 */

Mat motionExtractor::getCoherencyMap()
{
    return _coherencyMap;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief dessine la distribution du LMP - histogramme de B bins (couleur = direction, dimension = magnitude)
 * @param picture (Mat) : image sur laquelle on dessine
 */

void motionExtractor::drawDistribution(Mat &picture)
{
    if(_allPatch.size() > _minPatch)
    {
        Point2f p = _center;

        for(int i = 0; i < _cumul.size(); i++)
        {
            double angle = (i+1)*(360/_bins);

            double drift = 0.0005 * _cumul[i];

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
