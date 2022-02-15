//***********************************************************************
// Project		    : GeoMatch
// Author           : Shiju P K
// Email			: shijupk@gmail.com
// Created          : 10-01-2010
//
// File Name		: GeoMatch.cpp
// Last Modified By : Shiju P K
// Last Modified On : 13-07-2010
// Description      : class to implement edge based template matching
//
// Copyright        : (c) All rights reserved.
//***********************************************************************

//#include "pch.h"
#include "GeoMatch.h"
#include <iostream>

GeoMatch::GeoMatch(void)
{
  noOfCordinates = 0; // Initilize  no of cppodinates in model points
  modelDefined = false;
}

int GeoMatch::CreateGeoMatchModel(Mat templateArr, double minContrast, double maxContrast)
{

    Mat gx;		//Matrix to store X derivative
    Mat gy;		//Matrix to store Y derivative
    Mat nmsEdges;		//Matrix to store temp restult
    Size Ssize;

    /*
    // Convert IplImage to Matrix for integer operations
    CvMat srcstub, *src = (CvMat*)templateArr;
    src = cvGetMat( src, &srcstub );
    if(CV_MAT_TYPE( src->type ) != CV_8UC1)
    {
        return 0;
    }
    */

    Mat src(templateArr);  //Above step replicated
    // set width and height
    Ssize.width = src.cols;  //src->width;
    Ssize.height = src.rows; //src->height;
    modelHeight = src.rows;  //src->height;		//Save Template height
    modelWidth = src.cols;   //src->width;		//Save Template width

    noOfCordinates = 0;											//initialize
    cordinates =  new Point[modelWidth * modelHeight];		//Allocate memory for coorinates of selected points in template image

    edgeMagnitude = new double[modelWidth * modelHeight];		//Allocate memory for edge magnitude for selected points
    edgeDerivativeX = new double[modelWidth * modelHeight];			//Allocate memory for edge X derivative for selected points
    edgeDerivativeY = new double[modelWidth * modelHeight];			////Allocate memory for edge Y derivative for selected points


    // Calculate gradient of Template
    gx = Mat( Ssize.height, Ssize.width, CV_16SC1 );		//create Matrix to store X derivative
    gy = Mat( Ssize.height, Ssize.width, CV_16SC1 );		//create Matrix to store Y derivative


    Sobel( src, gx, CV_16S, 1, 0, 3 );		//gradient in X direction
    Sobel( src, gy, CV_16S, 0, 1, 3 );	//gradient in Y direction

    std::cout << "Sobel Done" << std::endl;

    nmsEdges = Mat(Ssize.height, Ssize.width, CV_32F);		//create Matrix to store Final nmsEdges
    const short* _sdx;
    const short* _sdy;
    double fdx,fdy;
    double MagG, DirG;
    double MaxGradient = -99999.99;
    double direction;
    int *orients = new int[Ssize.height * Ssize.width];
    // count variable;
    int count = 0,
        i,
        j;

    double **magMat;
    CreateDoubleMatrix(magMat ,Ssize);
    std::cout << "Create Double Mat Done" << std::endl;
    for( i = 1; i < Ssize.height - 1; i++ )
    {
        for( j = 1; j < Ssize.width - 1; j++ )
        {
            //_sdx = (short*)(gx->data.ptr + gx->step * i);
            //_sdy = (short*)(gy->data.ptr + gy->step * i);

            // read x, y derivatives
            //	fdx = _sdx[j]; fdy = _sdy[j];
            fdx = gx.at<short>(i, j);
            fdy = gy.at<short>(i, j);


            MagG = sqrt((float)(fdx * fdx) + (float)(fdy * fdy)); //Magnitude = Sqrt(gx^2 +gy^2)
            direction = atan2f((float)fdy, (float)fdx);//cvFastArctan((float)fdy,(float)fdx);	 //Direction = invtan (Gy / Gx)
            magMat[i][j] = MagG;

            if(MagG > MaxGradient)
                MaxGradient = MagG; // get maximum gradient value for normalizing.

            // get closest angle from 0, 45, 90, 135 set
            if ( (direction > 0 && direction < 22.5) || (direction > 157.5 && direction < 202.5) || (direction > 337.5 && direction < 360)  )
                direction = 0;
            else if ( (direction > 22.5 && direction < 67.5) || (direction > 202.5 && direction < 247.5)  )
                direction = 45;
            else if ( (direction > 67.5 && direction < 112.5)||(direction > 247.5 && direction < 292.5) )
                direction = 90;
            else if ( (direction > 112.5 && direction < 157.5)||(direction > 292.5 && direction < 337.5) )
                direction = 135;
            else
                direction = 0;

            orients[count] = (int)direction;
            count++;
        }
    }
    std::cout << "Ist fdx fdy Done" << std::endl;
    count = 0; // init count
    // non maximum suppression
    double leftPixel, rightPixel;

    for( i = 1; i < Ssize.height - 1; i++ )
    {
        for( j = 1; j < Ssize.width - 1; j++ )
        {
            switch ( orients[count] )
            {
            case 0:
                leftPixel  = magMat[i][j - 1];
                rightPixel = magMat[i][j + 1];
                break;
            case 45:
                leftPixel  = magMat[i - 1][j + 1];
                rightPixel = magMat[i + 1][j - 1];
                break;
            case 90:
                leftPixel  = magMat[i - 1][j];
                rightPixel = magMat[i + 1][j];
                break;
            case 135:
                leftPixel  = magMat[i - 1][j - 1];
                rightPixel = magMat[i + 1][j + 1];
                break;
            }
            // compare current pixels value with adjacent pixels
            if ((magMat[i][j] < leftPixel) || (magMat[i][j] < rightPixel))
                nmsEdges.at<float>(i, j) = 0.0f;//(nmsEdges->data.ptr + nmsEdges->step*i)[j]=0;
            else
                nmsEdges.at<float>(i, j) = (uchar)(magMat[i][j] / MaxGradient * 255);//(nmsEdges->data.ptr + nmsEdges->step*i)[j]=(uchar)(magMat[i][j]/MaxGradient*255);

            count++;
        }
    }
    std::cout << "Non max Done" << std::endl;

    int RSum = 0,CSum = 0;
    int curX, curY;
    int flag = 1;

    //Hysterisis threshold
    for( i = 1; i < Ssize.height - 1; i++ )
    {
        for( j = 1; j < Ssize.width; j++ )
        {
            //_sdx = (short*)(gx->data.ptr + gx->step*i);
            //_sdy = (short*)(gy->data.ptr + gy->step*i);
            //fdx = _sdx[j]; fdy = _sdy[j];

            //std::cout << gx.elemSize1();
            fdx = gx.at<short>(i, j);
            fdy = gy.at<short>(i, j);


            MagG = sqrt(fdx * fdx + fdy * fdy); //Magnitude = Sqrt(gx^2 +gy^2)
            DirG = atan2f((float)fdy, (float)fdx);  //cvFastArctan((float)fdy, (float)fdx);	 //Direction = tan(y/x)

            ////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]= MagG;
            flag = 1;
            double val = nmsEdges.at<float>(i, j);
            //std::cout << "nmsEdge float Done" << std::endl;
            //if(((double)((nmsEdges->data.ptr + nmsEdges->step*i))[j]) < maxContrast)
            if( val < maxContrast)
            {
                //if(((double)((nmsEdges->data.ptr + nmsEdges->step*i))[j])< minContrast)
                if(val < minContrast)
                {

                    //(nmsEdges->data.ptr + nmsEdges->step*i)[j] = 0;
                    nmsEdges.at<float>(i, j) = 0;
                    flag = 0; // remove from edge
                    ////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]=0;
                }
                else
                {   // if any of 8 neighboring pixel is not greater than max contraxt remove from edge
                    /*if( (((double)((nmsEdges->data.ptr + nmsEdges->step*(i-1)))[j-1]) < maxContrast)	&&
                        (((double)((nmsEdges->data.ptr + nmsEdges->step*(i-1)))[j]) < maxContrast)		&&
                        (((double)((nmsEdges->data.ptr + nmsEdges->step*(i-1)))[j+1]) < maxContrast)	&&
                        (((double)((nmsEdges->data.ptr + nmsEdges->step*i))[j-1]) < maxContrast)		&&
                        (((double)((nmsEdges->data.ptr + nmsEdges->step*i))[j+1]) < maxContrast)		&&
                        (((double)((nmsEdges->data.ptr + nmsEdges->step*(i+1)))[j-1]) < maxContrast)	&&
                        (((double)((nmsEdges->data.ptr + nmsEdges->step*(i+1)))[j]) < maxContrast)		&&
                        (((double)((nmsEdges->data.ptr + nmsEdges->step*(i+1)))[j+1]) < maxContrast)	)
                    */
                    if ( (nmsEdges.at<float>(i - 1, j - 1) < maxContrast) &&
                        (nmsEdges.at<float>(i - 1, j) < maxContrast) &&
                        (nmsEdges.at<float>(i - 1, j + 1) < maxContrast) &&
                        (nmsEdges.at<float>(i, j - 1) < maxContrast) &&
                        (nmsEdges.at<float>(i, j + 1) < maxContrast) &&
                        (nmsEdges.at<float>(i + 1, j - 1) < maxContrast) &&
                        (nmsEdges.at<float>(i + 1, j) < maxContrast) &&
                        (nmsEdges.at<float>(i + 1, j + 1) < maxContrast)
                        )
                    {
                        nmsEdges.at<float>(i, j) = 0;
                        flag = 0;
                        //(nmsEdges->data.ptr + nmsEdges->step*i)[j]=0;
                        //flag = 0;
                        ////((uchar*)(imgGDir->imageData + imgGDir->widthStep*i))[j]=0;
                    }
                }

            }

            // save selected edge information
            curX = i;	curY = j;
            if(flag != 0)
            {
                if(fdx != 0 || fdy != 0)
                {
                    // Row sum and column sum for center of gravity
                    RSum = RSum + curX;
                    CSum = CSum + curY;

                    cordinates[noOfCordinates].x = curX;
                    cordinates[noOfCordinates].y = curY;
                    edgeDerivativeX[noOfCordinates] = fdx;
                    edgeDerivativeY[noOfCordinates] = fdy;

                    //handle divide by zero
                    if(MagG != 0)
                        edgeMagnitude[noOfCordinates] = 1/MagG;  // gradient magnitude
                    else
                        edgeMagnitude[noOfCordinates] = 0;

                    noOfCordinates++;
                }
            }
        }
    }

    centerOfGravity.x = RSum / noOfCordinates; // center of gravity
    centerOfGravity.y = CSum / noOfCordinates ;	// center of gravity

    // change coordinates to reflect center of gravity
    for(int m = 0; m < noOfCordinates; m++)
    {
        int temp;

        temp = cordinates[m].x;
        cordinates[m].x = temp - centerOfGravity.x;
        temp = cordinates[m].y;
        cordinates[m].y = temp - centerOfGravity.y;
    }

    ////cvSaveImage("Edges.bmp",imgGDir);

    // free alocated memories
    delete[] orients;
    ////cvReleaseImage(&imgGDir);
    gx.release();
    gy.release();
    nmsEdges.release();

    ReleaseDoubleMatrix(magMat ,Ssize.height);

    modelDefined = true;
    return 1;
}

double GeoMatch::FindGeoMatchModel(Mat srcarr, double minScore, double greediness, Point* resultPoint)
{
  Mat Sdx, Sdy;

  double resultScore = 0;
  double partialSum = 0;
  double sumOfCoords = 0;
  double partialScore=0;
  // const short* _Sdx;
  // const short* _Sdy;
  int i, j, m; // count variables
  double iTx, iTy, iSx, iSy;
  double gradMag;
  int curX, curY;

  double** matGradMag; // Gradient magnitude matrix

  Mat src(srcarr);
  // bilateralFilter(srcarr, src, CVBIL_D, CVBIL_SIG_COLOR, CVBIL_SIG_SPACE);

  // source image size
  Size Ssize;
  Ssize.width = src.cols;
  Ssize.height = src.rows;

  CreateDoubleMatrix(matGradMag, Ssize); // create image to save gradient magnitude  values

  Sdx = Mat(Ssize.height, Ssize.width, CV_16SC1); // X derivatives
  Sdy = Mat(Ssize.height, Ssize.width, CV_16SC1); // y derivatives

  Sobel(src, Sdx, CV_16S, 1, 0, 3); // find X derivatives
  Sobel(src, Sdy, CV_16S, 0, 1, 3); // find Y derivatives

  // stoping criterias to search for model
  double normMinScore = minScore / noOfCordinates;                                           // precompute minumum score
  double normGreediness = ((1 - greediness * minScore) / (1 - greediness)) / noOfCordinates; // precompute greedniness

  for (i = 0; i < Ssize.height; i++)
  {
    for (j = 0; j < Ssize.width; j++)
    {
      iSx = Sdx.at<short>(i, j);
      iSy = Sdy.at<short>(i, j);

      gradMag = sqrt((iSx * iSx) + (iSy * iSy)); // Magnitude = Sqrt(dx^2 +dy^2)

      if (gradMag != 0.0)                 // hande divide by zero
        matGradMag[i][j] = 1 / gradMag; // 1/Sqrt(dx^2 +dy^2)
      else
        matGradMag[i][j] = 0;
    }
  }
  for (i = 0; i < Ssize.height; i++)
  {
    for (j = 0; j < Ssize.width; j++)
    {
      partialSum = 0; // initilize partialSum measure
      for (m = 0; m < noOfCordinates; m++)
      {
        curX = i + cordinates[m].x; // template X coordinate
        curY = j + cordinates[m].y; // template Y coordinate
        iTx = edgeDerivativeX[m];   // template X derivative
        iTy = edgeDerivativeY[m];   // template Y derivative

        if (curX < 0 || curY < 0 || curX > Ssize.height - 1 || curY > Ssize.width - 1)
          continue;

        iSx = Sdx.at<short>(curX, curY); // CHECK IF curX AND curY NEED TO BE SWITCHED
        iSy = Sdy.at<short>(curX, curY);

        if ((iSx != 0.0 || iSy != 0.0) && (iTx != 0.0 || iTy != 0.0))
        {
          // partial Sum  = Sum of(((Source X derivative* Template X drivative) + Source Y derivative * Template Y derivative)) / Edge
          // magnitude of(Template)* edge magnitude of(Source))
          partialSum = partialSum + ((iSx * iTx) + (iSy * iTy)) * (edgeMagnitude[m] * matGradMag[curX][curY]);
        }

        sumOfCoords = m + 1;
        partialScore = partialSum / sumOfCoords;
        // check termination criteria
        // if partial score score is less than the score than needed to make the required score at that position
        // break serching at that coordinate.
        if (partialScore < (MIN((minScore - 1) + normGreediness * sumOfCoords, normMinScore * sumOfCoords)))
          break;
      }
      if (partialScore > resultScore)
      {
        //  std::cout << "Partial Score is :" << partialScore << std::endl;
        resultScore = partialScore; //  Match score
        resultPoint->x = j;         // result coordinate X
        resultPoint->y = i;         // result coordinate Y
        //  std::cout << "Result Point is :" << resultPoint->x << resultPoint->y << std::endl;
      }
    }
  }

  // free used resources and return score
  ReleaseDoubleMatrix(matGradMag, Ssize.height);
  Sdx.release();
  Sdy.release();

  return resultScore;
}

// destructor
GeoMatch::~GeoMatch(void)
{
  delete[] cordinates;
  delete[] edgeMagnitude;
  delete[] edgeDerivativeX;
  delete[] edgeDerivativeY;
}

// allocate memory for doubel matrix
void GeoMatch::CreateDoubleMatrix(double**& matrix, Size size)
{
  matrix = new double*[size.height];
  for (int iInd = 0; iInd < size.height; iInd++)
    matrix[iInd] = new double[size.width];

  return;
}
// release memory
void GeoMatch::ReleaseDoubleMatrix(double**& matrix, int size)
{
  for (int iInd = 0; iInd < size; iInd++)
    delete[] matrix[iInd];

  return;
}

void GeoMatch::getContourPoints(Mat src, std::vector<Point>& points)
{
  Mat threshold_output, thres_Gray;
  std::vector<std::vector<cv::Point>> contours;
  std::vector<Vec4i> hierarchy;
  /// Detect edges using Threshold
  // threshold(src, threshold_output,minContrast, maxContrast, THRESH_BINARY);
  bilateralFilter(src, threshold_output, 7, 50, 50);

  double CannyAccThresh = threshold(threshold_output, thres_Gray, 0, 255, THRESH_BINARY | THRESH_OTSU);
  double CannyThresh = 0.1 * CannyAccThresh;

  Canny(threshold_output, thres_Gray, CannyThresh, CannyAccThresh, 3);
  /// Find contours
  findContours(thres_Gray, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE, Point(0, 0));

  vector<vector<Point>> contours_poly(contours.size());
  vector<Point2f> centers(contours.size());
  vector<float> radius(contours.size());
  for (size_t i = 0; i < contours.size(); i++)
  {
    approxPolyDP(contours[i], contours_poly[i], 1, false);
    minEnclosingCircle(contours_poly[i], centers[i], radius[i]);
  }

  for (int i = 0; i < contours_poly.size(); ++i)
  {

    Moments moms = moments(contours_poly[i]);
    double area = moms.m00;

    // filter area
    if (area < 15)
      continue;
    else
    {

      for (int j = 0; j < contours_poly[i].size(); ++j)
      {
        points.push_back(contours_poly[i][j]);
      }
    }
  }
/*
  Mat draw;
  cvtColor(src, draw, COLOR_GRAY2BGR);
  for (int i = 0; i < contours_poly.size(); ++i)
  {

    drawContours(draw, contours_poly, i, Scalar(0, 255, 0), 1, 8, vector<Vec4i>(), 0, Point());
  }

  imshow("Contour", draw);
  waitKey(0);
*/
}

// draw contours around result image
void GeoMatch::DrawContours(Mat source, Point COG, Scalar color, int lineWidth)
{
  Point point;
  point.y = COG.x;
  point.x = COG.y;
  for (int i = 0; i < noOfCordinates; i++)
  {

    point.y = cordinates[i].x + COG.y;
    point.x = cordinates[i].y + COG.x;
    line(source, point, point, color, lineWidth);
  }
  return;
}

// draw contour at template image
void GeoMatch::DrawContours(Mat source, Scalar color, int lineWidth)
{
  Point point;
  for (int i = 0; i < noOfCordinates; i++)
  {
    point.y = cordinates[i].x + centerOfGravity.x;
    point.x = cordinates[i].y + centerOfGravity.y;
    line(source, point, point, color, lineWidth);
  }
  return;
}
