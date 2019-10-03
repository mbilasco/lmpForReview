#include "faceroi.h"

FaceROI::FaceROI()
{
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief initialiser la region
 * @param vect (vecteur de point2f) : contient les points delimitant la region
 * @param min (double) : pas utile
 */

void FaceROI::init(vector<Point2f> vect, double min)
{
    for(int i = 0; i < vect.size(); i++)
        _points.push_back(vect[i]);
    _min = min;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief calculer le masque correspondant à la region
 * @param inMat (Mat) : image que l'on analyse
 * @return masque des pixels au sein de la region
 */

Mat FaceROI::estimateMask(Mat inMat)
{
    vector<Point> tmp;

    for(int i = 0; i < _points.size(); i++)
        tmp.push_back((Point) _points[i]);

    Mat mask = Mat::zeros(inMat.rows, inMat.cols, CV_8UC1);
    fillConvexPoly(mask,tmp,Scalar(255,255,255));

    return mask;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief recuperer les points delimitant la region
 * @return
 */

vector<Point2f> FaceROI::getPoints()
{
    return _points;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne le milieu de la region
 * @return
 */

Point2f FaceROI::getMiddle()
{
    double X = 0, Y = 0;

    for(int i = 0; i < _points.size(); i++)
    {
        X += _points[i].x;
        Y += _points[i].y;
    }

    X = X / (double)_points.size();
    Y = Y / (double)_points.size();

    _middle.x = X;
    _middle.y = Y;

    return _middle;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne la valeur minimal de magnitude au sein de la region
 * fonction inutile
 * @return
 */

double FaceROI::getMin()
{
    return _min;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief dessine les lignes de la region
 * @param inMat (Mat) : image sur laquelle dessiner
 */

void FaceROI::drawLine(Mat &inMat)
{
    for(int i = 0; i < _points.size()-1; i++)
        line(inMat,_points[i],_points[i+1],Scalar(0,0,255));

    line(inMat,_points[_points.size()-1],_points[0],Scalar(0,0,255));
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourner l'angle formé par trois points A, B et C
 * @param pointA
 * @param pointB
 * @param pointC
 * @return
 */

double FaceROI::AngleBetweenThreePoints(Point2f pointA, Point2f pointB, Point2f pointC)
{
    float a = pointB.x - pointA.x;
    float b = pointB.y - pointA.y;
    float c = pointB.x - pointC.x;
    float d = pointB.y - pointC.y;

    float atanA = atan2(a, b);
    float atanB = atan2(c, d);

    return (atanB - atanA)*180/CV_PI;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief calcule la distance entre deux points A et B
 * @param p1 : point A
 * @param p2 : point B
 * @return
 */

double FaceROI::distance(Point2f p1, Point2f p2)
{
    return sqrt(pow(p2.x - p1.x,2) + pow(p2.y - p1.y,2));
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne un vecteur double contenant toutes les informations (distances des cotés, angles de chaque coin) de la region
 * @return
 */

vector<double> FaceROI::getInformation()
{
    vector<double> tmp;

    for(int i = 0; i < _points.size()-2; i++)
        tmp.push_back(AngleBetweenThreePoints(_points[i],_points[i+1],_points[i+2]));

    tmp.push_back(AngleBetweenThreePoints(_points[0],_points[_points.size()-1],_points[_points.size()-2]));
    tmp.push_back(AngleBetweenThreePoints(_points[1],_points[0],_points[_points.size()-1]));

    for(int i = 0; i < _points.size()-1; i++)
        tmp.push_back(distance(_points[i],_points[i+1]));

    tmp.push_back(distance(_points[0],_points[_points.size()-1]));

    return tmp;
}

vector<double> FaceROI::getInformation2()
{
    vector<double> tmp;

    Point2f mid = getMiddle();

    //distance middle to corner
    for(int i = 0; i < _points.size(); i++)
        tmp.push_back(abs(sqrt(pow(mid.x-_points[i].x,2)+pow(mid.y-_points[i].y,2))));

    //angles between two corners
    for(int i = 0; i < _points.size()-1; i++)
        tmp.push_back(AngleBetweenThreePoints(_points[i],mid,_points[i+1]));

    tmp.push_back(AngleBetweenThreePoints(_points[_points.size()-1],mid,_points[0]));

    return tmp;
}
