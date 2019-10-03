#include "ourroi.h"

ourROI::ourROI()
{
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief creation du framework facial
 * @param landmarks (68 points donnes par les annotations)
 */

void ourROI::createROI(vector<Point2f> landmarks)
{
    _landmarks = landmarks;
    _estimatedPoints.clear();
    _meshs.clear();

    //calculer les points supplementaires pour le maillage
    estimatePoints();

    //construire le maillage
    estimateROIs();
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief calculer la dimension d'une region en fonction du pourcentage lambda
 * @param L (int) : taille en pourcentage d'une region
 * @return la dimension de la region correspondant au pourcentage fixe
 */

int ourROI::getPatchSize(int L)
{
    Point2f p1 = _landmarks[0];
    Point2f p2 = _landmarks[16];

    double D = sqrt(pow(p2.x - p1.x,2) + pow(p2.y - p1.y,2));

    double pp = (D * L) / 100.0;

    return pp;
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief dessine les points sur une image
 * @param inMat (Mat) : image sur laquelle dessiner
 */

void ourROI::drawPoints(Mat inMat)
{
    for(int i = 0; i < _landmarks.size(); i++)
        circle(inMat,_landmarks[i],1,Scalar(255,0,0),2);

    for(int i = 0; i < _estimatedPoints.size(); i++)
        circle(inMat,_estimatedPoints[i],1,Scalar(0,0,255),2);

    imshow("points",inMat);
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief estimer de nouveaux points a partir des landmarks
 * afin de créer des regions dans des zones dénuées de landmarks (joues, front, ...)
 */

void ourROI::estimatePoints()
{
    double noseSize = distanceBetween2points(_landmarks[27], _landmarks[33]);
    noseSize = noseSize/4.0;

    Point2f pA(_landmarks[17].x, _landmarks[17].y - noseSize); _estimatedPoints.push_back(pA);
    Point2f pB(_landmarks[19].x, _landmarks[19].y - noseSize); _estimatedPoints.push_back(pB);
    Point2f pC(_landmarks[21].x, _landmarks[21].y - noseSize); _estimatedPoints.push_back(pC);

    Point2f pD(_landmarks[22].x, _landmarks[22].y - noseSize); _estimatedPoints.push_back(pD);
    Point2f pE(_landmarks[24].x, _landmarks[24].y - noseSize); _estimatedPoints.push_back(pE);
    Point2f pF(_landmarks[26].x, _landmarks[26].y - noseSize); _estimatedPoints.push_back(pF);

    Point2f pG = getMiddle(_landmarks[1],_landmarks[41]); _estimatedPoints.push_back(pG);

    Point2f pJ = getMiddle(_landmarks[46],_landmarks[15]);
    Point2f pK = getMiddle(_landmarks[2],_landmarks[29]);
    Point2f pL = getMiddle(_landmarks[29],_landmarks[14]);

    Point2f pH = getMiddle(pK,_landmarks[39]); _estimatedPoints.push_back(pH);
    Point2f pI = getMiddle(pL,_landmarks[42]); _estimatedPoints.push_back(pI);

    _estimatedPoints.push_back(pJ);
    _estimatedPoints.push_back(pK);
    _estimatedPoints.push_back(pL);

    Point2f pM = getMiddle(_landmarks[3],_landmarks[30]); _estimatedPoints.push_back(pM);
    Point2f pN = getMiddle(_landmarks[30],_landmarks[13]); _estimatedPoints.push_back(pN);

    Point2f pO = getMiddle(_landmarks[4],_landmarks[48]); _estimatedPoints.push_back(pO);
    Point2f pP = getMiddle(_landmarks[6],_landmarks[59]); _estimatedPoints.push_back(pP);
    Point2f pQ = getMiddle(_landmarks[10],_landmarks[55]); _estimatedPoints.push_back(pQ);
    Point2f pR = getMiddle(_landmarks[12],_landmarks[54]); _estimatedPoints.push_back(pR);
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief estimation des 25 régions du framework facial en fonction des landmarks et des nouveaux points estimés
 */

void ourROI::estimateROIs()
{
    for(int i = 0; i < 25; i++) //25
    {
        FaceROI fr;
        _meshs.push_back(fr);
    }

    double v0 = 0;
    double v1 = 0;

    vector<Point2f> v;

    v = { _estimatedPoints[0],_estimatedPoints[1],_landmarks[19],_landmarks[18],_landmarks[17] }; _meshs[0].init(v,v1);
    v = { _estimatedPoints[1],_estimatedPoints[2],_landmarks[21],_landmarks[20],_landmarks[19] };_meshs[1].init(v,v0);
    v = { _estimatedPoints[2],_estimatedPoints[3],_landmarks[22],_landmarks[27],_landmarks[21] };_meshs[2].init(v,v0);
    v = { _estimatedPoints[3],_estimatedPoints[4],_landmarks[24],_landmarks[23],_landmarks[22] };_meshs[3].init(v,v0);
    v = { _estimatedPoints[4],_estimatedPoints[5],_landmarks[26],_landmarks[25],_landmarks[24] };_meshs[4].init(v,v1);
    v = { _landmarks[17],_landmarks[36],_landmarks[41],_estimatedPoints[6],_landmarks[1],_landmarks[0] };_meshs[5].init(v,v1);
    v = { _landmarks[17],_landmarks[18],_landmarks[19],_landmarks[20],_landmarks[21],_landmarks[39],_landmarks[38],_landmarks[37],_landmarks[36] };_meshs[6].init(v,v1);
    v = { _landmarks[21],_landmarks[27],_landmarks[29],_estimatedPoints[7],_landmarks[39] };_meshs[7].init(v,v1);
    v = { _landmarks[27],_landmarks[22],_landmarks[42],_estimatedPoints[8],_landmarks[29] };_meshs[8].init(v,v1);
    v = { _landmarks[22],_landmarks[23],_landmarks[24],_landmarks[25],_landmarks[26],_landmarks[45],_landmarks[44],_landmarks[43],_landmarks[42] };_meshs[9].init(v,v1);
    v = { _landmarks[45],_landmarks[26],_landmarks[16],_landmarks[15],_estimatedPoints[9],_landmarks[46] };_meshs[10].init(v,v1);
    v = { _landmarks[41],_landmarks[40],_landmarks[39],_estimatedPoints[7],_estimatedPoints[10],_estimatedPoints[6] };_meshs[11].init(v,v1);
    v = { _landmarks[42],_landmarks[47],_landmarks[46],_estimatedPoints[9],_estimatedPoints[11],_estimatedPoints[8] };_meshs[12].init(v,v1);
    v = { _landmarks[1],_estimatedPoints[6],_estimatedPoints[10],_estimatedPoints[12],_estimatedPoints[14],_landmarks[4],_landmarks[3],_landmarks[2] };_meshs[13].init(v,v1);
    v = { _estimatedPoints[7],_landmarks[29],_landmarks[30],_estimatedPoints[12],_estimatedPoints[10] };_meshs[14].init(v,v1);
    v = { _landmarks[29],_estimatedPoints[8],_estimatedPoints[11],_estimatedPoints[13],_landmarks[30] };_meshs[15].init(v,v1);
    v = { _estimatedPoints[9],_landmarks[15],_landmarks[14],_landmarks[13],_landmarks[12],_estimatedPoints[17],_estimatedPoints[13],_estimatedPoints[11] };_meshs[16].init(v,v1);
    v = { _landmarks[4],_landmarks[48],_landmarks[59],_landmarks[6],_landmarks[5] };_meshs[17].init(v,v1);
    v = { _estimatedPoints[12],_landmarks[49],_landmarks[59],_estimatedPoints[15],_estimatedPoints[14] };_meshs[18].init(v,v0);
    v = { _estimatedPoints[12],_landmarks[30],_landmarks[51],_landmarks[50],_landmarks[49] };_meshs[19].init(v,v1);
    v = { _landmarks[30],_estimatedPoints[13],_landmarks[53],_landmarks[52],_landmarks[51] };_meshs[20].init(v,v1);
    v = { _estimatedPoints[13],_estimatedPoints[17],_estimatedPoints[16],_landmarks[55],_landmarks[53] };_meshs[21].init(v,v0);
    v = { _landmarks[54],_landmarks[12],_landmarks[11],_landmarks[10],_landmarks[55] };_meshs[22].init(v,v1);
    v = { _landmarks[59],_landmarks[58],_landmarks[57],_landmarks[8],_landmarks[7],_landmarks[6] };_meshs[23].init(v,v1);
    v = { _landmarks[57],_landmarks[56],_landmarks[55],_landmarks[10],_landmarks[9],_landmarks[8] };_meshs[24].init(v,v1);
}

/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------- */

/**
 * @brief creer le masque facial en fonction des landmakrs délimitant le visage (contour exterieur)
 * @param inMat (Mat) : image de référence pour connaitre la taille
 * @return le masque correspondant au visage
 */

Mat ourROI::delimitateFace(Mat inMat)
{
    Mat mask = Mat::zeros(inMat.size(), CV_8UC1);

    vector<Point> tmp;

    for(int i = 0; i <= 16; i++)
        tmp.push_back(_landmarks[i]);

    tmp.push_back(_landmarks[26]);
    tmp.push_back(_estimatedPoints[5]);
    tmp.push_back(_estimatedPoints[4]);
    tmp.push_back(_estimatedPoints[1]);
    tmp.push_back(_estimatedPoints[0]);
    tmp.push_back(_landmarks[17]);

    fillConvexPoly(mask,tmp,Scalar(255));

    mask = cv::Mat::ones(mask.size(), mask.type()) * 255 - mask;

    tmp.clear();

    for(int i = 36; i <= 41; i++)
        tmp.push_back(_landmarks[i]);

    fillConvexPoly(mask,tmp,Scalar(255));

    tmp.clear();

    for(int i = 42; i <= 47; i++)
        tmp.push_back(_landmarks[i]);

    fillConvexPoly(mask,tmp,Scalar(255));

    tmp.clear();

    for(int i = 60; i <= 66; i++)
        tmp.push_back(_landmarks[i]);

    fillConvexPoly(mask,tmp,Scalar(255));

    tmp.clear();

    return mask;
}
