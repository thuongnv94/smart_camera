#ifndef MATCHDESCRIPTOR_H
#define MATCHDESCRIPTOR_H
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class MatchDescriptor
{
public:
    MatchDescriptor();

    float AjustementRtSafe(vector<Point2d>& c, float& alphaMin, float& phiMin, float& sMin);
    float Ajustement(vector<Point2d>& c, float& alphaMin, float& phiMin, float& sMin);
    void falpha(float x, float* fn, float* df);
    void InitFrequence();
    float rtsafe(float x1, float x2, float xacc);
    float Distance(complex<float> r, float alpha);

    vector<Point2d> sContour;
    vector<complex<float>> b;
    vector<complex<float>> a;
    vector<float> frequence;
    vector<float> rho, psi;
    double pi;
    int nbDesFit;
};

#endif // MATCHDESCRIPTOR_H
