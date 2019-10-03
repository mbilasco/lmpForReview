#include "faceseparator.h"

FaceSeparator::FaceSeparator()
{

}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief creer le framework facial
 * @param landmarks
 */

void FaceSeparator::createROI(vector<Point2f> landmarks)
{
    _landmarks = landmarks;
    _meshs.clear();
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne un vecteur avec les régions du framework facial
 * @return
 */

vector<FaceROI> FaceSeparator::getMeshs()
{
    return _meshs;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne le milieu de deux points A et B
 * @param p1 (Point2f) : point A
 * @param p2 (Point2f) : point B
 * @return
 */

Point2f FaceSeparator::getMiddle(Point2f p1, Point2f p2)
{
    Point2f m;

    m.x = (p1.x + p2.x) / 2;
    m.y = (p1.y + p2.y) / 2;

    return m;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief retourne la distance entre deux points A et B
 * @param p1 (Point2f) : point A
 * @param p2 (Point2f) : point B
 * @return
 */

double FaceSeparator::distanceBetween2points(Point2f p1, Point2f p2)
{
    return sqrt(pow(p2.x - p1.x,2) + pow(p2.y - p1.y,2));
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief dessine les regions du framework facial sur une image
 * @param inMat (Mat) : image sur laquelle dessiner
 */

void FaceSeparator::drawMesh(Mat inMat)
{
    Mat img;
    inMat.copyTo(img);

    for(int i = 0; i < _meshs.size(); i++)
        _meshs[i].drawLine(img);

    imshow("meshs",img);
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief dessiner les points du framework facial delimitant les régions
 * @param inMat (Mat) : image sur laquelle dessiner
 */

void FaceSeparator::drawPoints(Mat inMat)
{
    imshow("points",inMat);
}
