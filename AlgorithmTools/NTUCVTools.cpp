#include "NTUCVTools.h"

double NTUCVTools::getSimilarity(const Mat A, const Mat B)
{
  if (A.rows > 0 && A.rows == B.rows && A.cols > 0 && A.cols == B.cols)
  {
    double errorL2;
    // Calculate the L2 relative error between images.
    errorL2 = norm(A, B, CV_L2);

    // Convert to a reasonable scale, since L2 error is summed across all pixels of the image.
    double similarity = errorL2 / (double)(A.rows * A.cols);
    return similarity;
  }
  else
  {
    // Images have a different size
    return -1; // Return a bad value
  }
}

double NTUCVTools::templateMachingSimple(Mat img_src, Mat tmp, Point2f& pointCenter, double threshhold)
{
  int img_cols = img_src.cols;
  int img_rows = img_src.rows;

  /// Create the result matrix
  int result_cols = img_cols - tmp.cols + 1;
  int result_rows = img_rows - tmp.rows + 1;

  cv::Mat result = cv::Mat(result_rows, result_cols, CV_32FC1);

  /// Do the Matching and Normalize
  cv::matchTemplate(img_src, tmp, result, cv::TM_CCORR_NORMED);
  double score1 = result.at<float>(0, 0);

  cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

  /// Localizing the best match with minMaxLoc
  double minVal;
  double maxVal;
  cv::Point minLoc;
  cv::Point maxLoc;
  cv::Point matchLoc;

  cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

  matchLoc = maxLoc;

  int ptx = matchLoc.x;
  int pty = matchLoc.y;
  int r_w = tmp.cols;
  int r_h = tmp.rows;

  cv::Mat mat_ROI;
  img_src(cv::Rect(ptx, pty, r_w, r_h)).copyTo(mat_ROI);
  pointCenter = Point2f(ptx + r_w / 2.0, pty + r_h / 2.0); // Show me what you got

  if (score1 > threshhold)
  {

    ///  Source image to display

    return getSimilarity(tmp, mat_ROI);
  }

  return -1.0;
}

Mat NTUCVTools::rotateImage(const Mat src, double angle, bool flag, Point2f pc)
{
  cv::Mat dst;

  // get rotation matrix for rotating the image around its center in pixel coordinates
  cv::Point2f center;
  if (pc != Point2f(-1, -1))
    center = pc;
  else
    center = Point2f(static_cast<float>((src.cols - 1) / 2.0), static_cast<float>((src.rows - 1) / 2.0));

  cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
  // determine bounding rectangle, center not relevant
  // adjust transformation matrix
  if (!flag)
  {
    cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), src.size(), angle).boundingRect2f();

    rot.at<double>(0, 2) += static_cast<double>(bbox.width) / 2.0 - src.cols / 2.0;
    rot.at<double>(1, 2) += static_cast<double>(bbox.height) / 2.0 - src.rows / 2.0;
    cv::warpAffine(src, dst, rot, bbox.size());
  }
  else
  {
    cv::warpAffine(src, dst, rot, src.size());
  }

  return dst;
}

std::vector<Point2f> NTUCVTools::rotatePoints(Size size_src, const std::vector<Point2f> roi_points, float angle)
{
  std::vector<cv::Point2f> points_out;

  // get rotation matrix for rotating the image around its center in pixel coordinates
  cv::Point2f center(static_cast<float>(roi_points[0].x + (roi_points[1].x - roi_points[0].x) / 2.0),
                     static_cast<float>(roi_points[0].y + (roi_points[3].y - roi_points[0].y) / 2.0));
  cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);

  // adjust transformation matrix

  std::vector<cv::Point2f> points = {
      {float(center.x - size_src.width / 2.0), float(center.y - size_src.height / 2.0)},
      {float(center.x + size_src.width / 2.0), float(center.y - size_src.height / 2.0)},
      {float(center.x + size_src.width / 2.0), float(center.y + size_src.height / 2.0)},
      {float(center.x - size_src.width / 2.0), float(center.y + size_src.height / 2.0)}}; /// Show me what you got

  cv::transform(points, points_out, rot);
  return points_out;
}

int NTUCVTools::CircleFitByLevenbergMarquardtFull(vector<Point2f>& points, NTUCVTools::Circle& circleIni, double LambdaIni,
                                                  NTUCVTools::Circle& circle)

/*                                     <------------------ Input ------------------->  <-- Output -->

       Geometric circle fit to a given set of data points (in 2D)

       Input:  data     - the class of data (contains the given points):

           data.n   - the number of data points
           data.X[] - the array of X-coordinates
           data.Y[] - the array of Y-coordinates

               circleIni - parameters of the initial circle ("initial guess")

           circleIni.a - the X-coordinate of the center of the initial circle
           circleIni.b - the Y-coordinate of the center of the initial circle
           circleIni.r - the radius of the initial circle

           LambdaIni - the initial value of the control parameter "lambda"
                       for the Levenberg-Marquardt procedure
                       (common choice is a small positive number, e.g. 0.001)

       Output:
           integer function value is a code:
                      0:  normal termination, the best fitting circle is
                          successfully found
                      1:  the number of outer iterations exceeds the limit (99)
                          (indicator of a possible divergence)
                      2:  the number of inner iterations exceeds the limit (99)
                          (another indicator of a possible divergence)
                      3:  the coordinates of the center are too large
                          (a strong indicator of divergence)

           circle - parameters of the fitting circle ("best fit")

           circle.a - the X-coordinate of the center of the fitting circle
           circle.b - the Y-coordinate of the center of the fitting circle
           circle.r - the radius of the fitting circle
           circle.s - the root mean square error (the estimate of sigma)
           circle.i - the total number of outer iterations (updating the parameters)
           circle.j - the total number of inner iterations (adjusting lambda)

       Algorithm:  Levenberg-Marquardt running over the full parameter space (a,b,r)

       See a detailed description in Section 4.5 of the book by Nikolai Chernov:
       "Circular and linear regression: Fitting circles and lines by least squares"
       Chapman & Hall/CRC, Monographs on Statistics and Applied Probability, volume 117, 2010.

        Nikolai Chernov,  February 2014
*/
{
  int code, i, iter, inner, IterMAX = 99;

  double factorUp = 10., factorDown = 0.04, lambda, ParLimit = 1.e+6;
  double dx, dy, ri, u, v;
  double Mu, Mv, Muu, Mvv, Muv, Mr, UUl, VVl, Nl, F1, F2, F3, dX, dY, dR;
  double epsilon = 3.e-8;
  double G11, G22, G33, G12, G13, G23, D1, D2, D3;

  Circle Old, New;

  //       starting with the given initial circle (initial guess)

  New = circleIni;

  //       compute the root-mean-square error via function Sigma; see Utilities.cpp

  New.s = Sigma(points, New);

  //       initializing lambda, iteration counters, and the exit code

  lambda = LambdaIni;
  iter = inner = code = 0;

NextIteration:

  Old = New;
  if (++iter > IterMAX)
  {
    code = 1;
    goto enough;
  }

  //       computing moments

  Mu = Mv = Muu = Mvv = Muv = Mr = 0.;

  for (i = 0; i < points.size(); i++)
  {
    dx = points[i].x - Old.a;
    dy = points[i].y - Old.b;
    ri = sqrt(dx * dx + dy * dy);
    u = dx / ri;
    v = dy / ri;
    Mu += u;
    Mv += v;
    Muu += u * u;
    Mvv += v * v;
    Muv += u * v;
    Mr += ri;
  }

  Mu /= static_cast<double>(points.size());
  Mv /= static_cast<double>(points.size());
  Muu /= static_cast<double>(points.size());
  Mvv /= static_cast<double>(points.size());
  Muv /= static_cast<double>(points.size());
  Mr /= static_cast<double>(points.size());

  //       computing matrices

  double meanX, meanY;

  meansOfPoints(points, meanX, meanY);

  F1 = Old.a + Old.r * Mu - meanX;
  F2 = Old.b + Old.r * Mv - meanY;
  F3 = Old.r - Mr;

  Old.g = New.g = sqrt(F1 * F1 + F2 * F2 + F3 * F3);

try_again:

  UUl = Muu + lambda;
  VVl = Mvv + lambda;
  Nl = 1.0 + lambda;

  //         Cholesly decomposition

  G11 = sqrt(UUl);
  G12 = Muv / G11;
  G13 = Mu / G11;
  G22 = sqrt(VVl - G12 * G12);
  G23 = (Mv - G12 * G13) / G22;
  G33 = sqrt(Nl - G13 * G13 - G23 * G23);

  D1 = F1 / G11;
  D2 = (F2 - G12 * D1) / G22;
  D3 = (F3 - G13 * D1 - G23 * D2) / G33;

  dR = D3 / G33;
  dY = (D2 - G23 * dR) / G22;
  dX = (D1 - G12 * dY - G13 * dR) / G11;

  if ((abs(dR) + abs(dX) + abs(dY)) / (1.0 + Old.r) < epsilon)
    goto enough;

  //       updating the parameters

  New.a = Old.a - dX;
  New.b = Old.b - dY;

  if (abs(New.a) > ParLimit || abs(New.b) > ParLimit)
  {
    code = 3;
    goto enough;
  }

  New.r = Old.r - dR;

  if (New.r <= 0.)
  {
    lambda *= factorUp;
    if (++inner > IterMAX)
    {
      code = 2;
      goto enough;
    }
    goto try_again;
  }

  //       compute the root-mean-square error via function Sigma; see Utilities.cpp

  New.s = Sigma(points, New);

  //       check if improvement is gained

  if (New.s < Old.s) //   yes, improvement
  {
    lambda *= factorDown;
    goto NextIteration;
  }
  else //   no improvement
  {
    if (++inner > IterMAX)
    {
      code = 2;
      goto enough;
    }
    lambda *= factorUp;
    goto try_again;
  }

  //       exit

enough:

  Old.i = iter;  // total number of outer iterations (updating the parameters)
  Old.j = inner; // total number of inner iterations (adjusting lambda)

  circle = Old;

  return code;
}

void NTUCVTools::meansOfPoints(vector<Point2f> points, double& meanX, double& meanY)
{
  meanX = 0.;
  meanY = 0.;
  int n = points.size();

  for (int i = 0; i < n; i++)
  {
    meanX += points[i].x;
    meanY += points[i].y;
  }
  meanX /= static_cast<double>(n);
  meanY /= static_cast<double>(n);
}

double NTUCVTools::Sigma(vector<Point2f>& points, NTUCVTools::Circle& circle)
{
  double sum = 0., dx, dy;

  for (int i = 0; i < points.size(); i++)
  {
    dx = points[i].x - circle.a;
    dy = points[i].y - circle.b;
    sum += SQR(sqrt(dx * dx + dy * dy) - circle.r);
  }
  return sqrt(sum / static_cast<double>(points.size()));
}

bool NTUCVTools::isInside(Point2f center, Point2f point, float rad)
{
  // Compare radius of circle with distance
  // of its center from given point
  if ((point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y) <= (rad * rad))
    return true;
  else
    return false;
}

bool NTUCVTools::isOutside(Point2f center, Point2f point, float rad)
{
  // Compare radius of circle with distance
  // of its center from given point
  if ((point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y) > (rad * rad))
    return true;
  else
    return false;
}

bool NTUCVTools::filterWithRadius(Mat src, Mat& dst, vector<Point2f>& points, Point2f center, float r_iner, float r_outer)
{
  dst = cv::Mat::zeros(src.rows, src.cols, CV_8U);
  for (int i = 0; i < src.rows; ++i)
  {
    for (int j = 0; j < src.cols; ++j)
    {
      if (src.at<uchar>(i, j) > 0)
      {
        if (isInside(center, Point2f(j, i), r_outer) && isOutside(center, Point2f(j, i), r_iner))
        {
          dst.at<uchar>(i, j) = 255;
          points.push_back(Point2f(j, i));
        }
      }
    }
  }
  if (points.size() == 0)
    return false;

  return true;
}

Point2f NTUCVTools::rotatePoint(const Point2f& inPoint, const Point2f& center, const double& angRad)
{
  /* return (rotate2d(inPoint - center, (angRad*M_PI) / 180.0) + center);*/

  std::vector<Point2f> pointsOut;
  std::vector<Point2f> pointsInt;
  pointsInt.push_back(inPoint);

  cv::Mat rot = cv::getRotationMatrix2D(center, angRad, 1.0);

  // adjust transformation matrix

  cv::transform(pointsInt, pointsOut, rot);
  return pointsOut[0];
}

Point2f NTUCVTools::rotate2d(const Point2f& inPoint, const double& angRad)
{
  cv::Point2f outPoint;
  // CW rotation
  outPoint.x = std::cos(angRad) * inPoint.x - std::sin(angRad) * inPoint.y;
  outPoint.y = std::sin(angRad) * inPoint.x + std::cos(angRad) * inPoint.y;
  return outPoint;
}

std::vector<Point2f> NTUCVTools::rotatePoints(const std::vector<Point2f> roi_points, Point2f center, double angle)
{

  std::vector<Point2f> pointsOut;

  cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);

  // adjust transformation matrix

  cv::transform(roi_points, pointsOut, rot);
  return pointsOut;
}

// *************************************************************
// glide a window across the image and
// create two maps: mean and standard deviation.
// *************************************************************
//#define BINARIZEWOLF_VERSION  "2.3 (February 26th, 2013)"

double NTUCVTools::calcLocalStats(Mat& im, Mat& map_m, Mat& map_s, int win_x, int win_y)
{

  double m, s, max_s, sum, sum_sq, foo;
  int wxh = win_x / 2;
  int wyh = win_y / 2;
  int x_firstth = wxh;
  int y_lastth = im.rows - wyh - 1;
  int y_firstth = wyh;
  double winarea = win_x * win_y;

  max_s = 0;
  for (int j = y_firstth; j <= y_lastth; j++)
  {
    // Calculate the initial window at the beginning of the line
    sum = sum_sq = 0;
    for (int wy = 0; wy < win_y; wy++)
      for (int wx = 0; wx < win_x; wx++)
      {
        foo = im.uget(wx, j - wyh + wy);
        sum += foo;
        sum_sq += foo * foo;
      }
    m = sum / winarea;
    s = sqrt((sum_sq - (sum * sum) / winarea) / winarea);
    if (s > max_s)
      max_s = s;
    map_m.fset(x_firstth, j, m);
    map_s.fset(x_firstth, j, s);

    // Shift the window, add and remove new/old values to the histogram
    for (int i = 1; i <= im.cols - win_x; i++)
    {

      // Remove the left old column and add the right new column
      for (int wy = 0; wy < win_y; ++wy)
      {
        foo = im.uget(i - 1, j - wyh + wy);
        sum -= foo;
        sum_sq -= foo * foo;
        foo = im.uget(i + win_x - 1, j - wyh + wy);
        sum += foo;
        sum_sq += foo * foo;
      }
      m = sum / winarea;
      s = sqrt((sum_sq - (sum * sum) / winarea) / winarea);
      if (s > max_s)
        max_s = s;
      map_m.fset(i + wxh, j, m);
      map_s.fset(i + wxh, j, s);
    }
  }

  return max_s;
}

void NTUCVTools::NiblackSauvolaWolfJolion(Mat im, Mat& output, BhThresholdMethod version, int winx, int winy, double k, double dR)
{

  double m, s, max_s;
  double th = 0;
  double min_I, max_I;
  int wxh = winx / 2;
  int wyh = winy / 2;
  int x_firstth = wxh;
  int x_lastth = im.cols - wxh - 1;
  int y_lastth = im.rows - wyh - 1;
  int y_firstth = wyh;
  // int mx, my;

  // Create local statistics and store them in a double matrices
  Mat map_m = Mat::zeros(im.rows, im.cols, CV_32F);
  Mat map_s = Mat::zeros(im.rows, im.cols, CV_32F);
  max_s = calcLocalStats(im, map_m, map_s, winx, winy);

  minMaxLoc(im, &min_I, &max_I);

  Mat thsurf(im.rows, im.cols, CV_32F);

  // Create the threshold surface, including border processing
  // ----------------------------------------------------
  for (int j = y_firstth; j <= y_lastth; j++)
  {

    float* th_surf_data = thsurf.ptr<float>(j) + wxh;
    float* map_m_data = map_m.ptr<float>(j) + wxh;
    float* map_s_data = map_s.ptr<float>(j) + wxh;

    // NORMAL, NON-BORDER AREA IN THE MIDDLE OF THE WINDOW:
    for (int i = 0; i <= im.cols - winx; i++)
    {
      m = *map_m_data++;
      s = *map_s_data++;

      // Calculate the threshold
      switch (version)
      {

      case BhThresholdMethod::NIBLACK:
        th = m + k * s;
        break;

      case BhThresholdMethod::SAUVOLA:
        th = m * (1 + k * (s / dR - 1));
        break;

      case BhThresholdMethod::WOLFJOLION:
        th = m + k * (s / max_s - 1) * (m - min_I);
        break;

      default:
        cerr << "Unknown threshold type in ImageThresholder::surfaceNiblackImproved()\n";
        exit(1);
      }

      // thsurf.fset(i+wxh,j,th);
      *th_surf_data++ = th;

      if (i == 0)
      {
        // LEFT BORDER
        float* th_surf_ptr = thsurf.ptr<float>(j);
        for (int i = 0; i <= x_firstth; ++i)
          *th_surf_ptr++ = th;

        // LEFT-UPPER CORNER
        if (j == y_firstth)
        {
          for (int u = 0; u < y_firstth; ++u)
          {
            float* th_surf_ptr = thsurf.ptr<float>(u);
            for (int i = 0; i <= x_firstth; ++i)
              *th_surf_ptr++ = th;
          }
        }

        // LEFT-LOWER CORNER
        if (j == y_lastth)
        {
          for (int u = y_lastth + 1; u < im.rows; ++u)
          {
            float* th_surf_ptr = thsurf.ptr<float>(u);
            for (int i = 0; i <= x_firstth; ++i)
              *th_surf_ptr++ = th;
          }
        }
      }

      // UPPER BORDER
      if (j == y_firstth)
        for (int u = 0; u < y_firstth; ++u)
          thsurf.fset(i + wxh, u, th);

      // LOWER BORDER
      if (j == y_lastth)
        for (int u = y_lastth + 1; u < im.rows; ++u)
          thsurf.fset(i + wxh, u, th);
    }

    // RIGHT BORDER
    float* th_surf_ptr = thsurf.ptr<float>(j) + x_lastth;
    for (int i = x_lastth; i < im.cols; ++i)
      // thsurf.fset(i,j,th);
      *th_surf_ptr++ = th;

    // RIGHT-UPPER CORNER
    if (j == y_firstth)
    {
      for (int u = 0; u < y_firstth; ++u)
      {
        float* th_surf_ptr = thsurf.ptr<float>(u) + x_lastth;
        for (int i = x_lastth; i < im.cols; ++i)
          *th_surf_ptr++ = th;
      }
    }

    // RIGHT-LOWER CORNER
    if (j == y_lastth)
    {
      for (int u = y_lastth + 1; u < im.rows; ++u)
      {
        float* th_surf_ptr = thsurf.ptr<float>(u) + x_lastth;
        for (int i = x_lastth; i < im.cols; ++i)
          *th_surf_ptr++ = th;
      }
    }
  }
  // cerr << "surface created" << endl;

  for (int y = 0; y < im.rows; ++y)
  {
    unsigned char* im_data = im.ptr<unsigned char>(y);
    float* th_surf_data = thsurf.ptr<float>(y);
    unsigned char* output_data = output.ptr<unsigned char>(y);
    for (int x = 0; x < im.cols; ++x)
    {
      *output_data = *im_data >= *th_surf_data ? 255 : 0;
      im_data++;
      th_surf_data++;
      output_data++;
    }
  }
}

vector<Point2f> NTUCVTools::createPoinsFromCennterAndSize(Point2f center, float width, float height)
{
  // Show me what you got
}

void NTUCVTools::autoThreshold(Mat src, Mat& dst, BhThresholdMethod method)
{

  int winx = 0, winy = 0;
  float optK = 0.5;
  bool didSpecifyK = false;

  // Treat the window size
  if (winx == 0 || winy == 0)
  {
    // cerr << "Input size: " << src.cols << "x" << src.rows << endl;
    winy = (int)(2.0 * src.rows - 1) / 3;
    winx = (int)src.cols - 1 < winy ? src.cols - 1 : winy;
    // if the window is too big, than we asume that the image
    // is not a single text box, but a document page: set
    // the window size to a fixed constant.
    if (winx > 100)
      winx = winy = 40;
    // cerr << "Setting window size to [" << winx << "," << winy << "].\n";
  }

  // Threshold
  dst = Mat(src.rows, src.cols, CV_8U);

  NiblackSauvolaWolfJolion(src, dst, method, winx, winy, optK, 128);
}

void NTUCVTools::drawArrowLine(QImage& image, Point2f pt1, Point2f pt2, Scalar color, int thickness)
{
  QColor qcolor = scalar2qcolor(color);
  const double tipSize = norm(pt1 - pt2) * 0.1; // Factor to normalize the size of the tip depending on the length of the arrow

  QPainter painter(&image);
  painter.setPen(QPen(qcolor, thickness));

  painter.drawLine(point2f2qpoint2f(pt1), point2f2qpoint2f(pt2));

  const double angle = atan2((double)pt1.y - pt2.y, (double)pt1.x - pt2.x);

  Point p(cvRound(pt2.x + tipSize * cos(angle + CV_PI / 4)), cvRound(pt2.y + tipSize * sin(angle + CV_PI / 4)));

  painter.drawLine(point2f2qpoint2f(p), point2f2qpoint2f(pt2));

  p.x = cvRound(pt2.x + tipSize * cos(angle - CV_PI / 4));
  p.y = cvRound(pt2.y + tipSize * sin(angle - CV_PI / 4));
  painter.drawLine(point2f2qpoint2f(p), point2f2qpoint2f(pt2));
}

QColor NTUCVTools::scalar2qcolor(Scalar color)
{
  return QColor(color[2], color[1], color[0]); // swap RGB-->BGR
}

Scalar NTUCVTools::qcolor2scalar(QColor color)
{
  int r, g, b;
  color.getRgb(&r, &g, &b);
  return cv::Scalar(b, g, r); // swap RGB-->BGR
}

QPointF NTUCVTools::point2f2qpoint2f(Point2f point) { return QPointF(point.x, point.y); }

void NTUCVTools::drawPolygon(QImage& image, vector<Point2f> pts, Scalar color, int thickness)
{
  QColor qcolor = scalar2qcolor(color);

  QPainter painter(&image);
  painter.setPen(QPen(qcolor, thickness));

  QPolygonF poly;
  for (int i = 0; i < pts.size(); ++i)
  {
    poly << point2f2qpoint2f(pts[i]);
  }

  painter.drawPolygon(poly);
}

void NTUCVTools::drawText(QImage& image, QString text, Point2f pt, double angle, Scalar color, int fontsize, Point2f shift, int thickness)
{
  QColor qcolor = scalar2qcolor(color);

  QFont font = QApplication::font();
  QPainter painter(&image);
  painter.setPen(QPen(qcolor, thickness));

  font.setPixelSize(fontsize);
  painter.setFont(font);
  painter.rotate(angle);
  painter.drawText(point2f2qpoint2f(pt) + point2f2qpoint2f(shift), text);
}

Rect NTUCVTools::transformRect(Rect rect, float delta_x, float delta_y)
{
  Rect rec = rect;
  rec.x += delta_x;
  rect.y += delta_y;
  return rec;
}

vector<Point2f> NTUCVTools::getPointsFromCenter(Point2f center, Size size)
{
  float halftWidth = size.width / 2.f;
  float halftHeight = size.height / 2.f;
  return {{center.x - halftWidth, center.y - halftHeight},
          {center.x + halftWidth, center.y - halftHeight},
          {center.x + halftWidth, center.y + halftHeight},
          {center.x - halftWidth, center.y + halftHeight}};
}

vector<Point2f> NTUCVTools::getPointsFromRect(Rect rect)
{
  return {rect.tl(), Point2f(rect.tl().x, rect.br().y), rect.br(), Point2f(rect.br().x, rect.tl().y)};
}

void NTUCVTools::drawPoint(QImage& image, Point2f pt, Scalar color, int thickness)
{
  QColor qcolor = scalar2qcolor(color);

  QPainter painter(&image);
  painter.setPen(QPen(qcolor, thickness));

  painter.drawPoint(point2f2qpoint2f(pt));
}

void NTUCVTools::drawPolyLine(QImage& image, vector<Point> pts, Scalar color, int thickness)
{
  QColor qcolor = scalar2qcolor(color);

  QPainter painter(&image);
  painter.setPen(QPen(qcolor, thickness));

  QVector<QPoint> points;
  for (size_t i = 0; i < pts.size(); ++i)
  {
    points << QPoint(pts[i].x, pts[i].y);
  }
  painter.drawPolyline(points);
}
vector<float> NTUCVTools::calculatorHistogram(Mat src)
{
  Mat gray;
  // 8-bits unsigned, raw bayer image
  if (src.type() == CV_8UC1)
  {
    src.copyTo(gray);
    // 8-bit unsigned rgb image
  }
  else if (src.type() == CV_8UC3)
  {
    cvtColor(src, gray, COLOR_BGR2GRAY);
  }

  /// Establish the number of bins
  int histSize = 256;

  /// Set the ranges ( for B,G,R) )
  float range[] = {0, 256};
  const float* histRange = {range};
  bool uniform = true;
  bool accumulate = false;
  Mat hist;
  int channels[] = {0};

  /// Compute the histograms:
  cv::calcHist(&gray, 1, channels, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

  vector<float> data;
  for (int i = 0; i < hist.rows; i++)
  {
    data.push_back(hist.at<float>(i));
  }

  return data;
  // Draw the histograms for B, G and R
  // int hist_h = 400;

  /// Normalize the result to [ 0, histImage.rows ]
  // cv::normalize(obj.r_hist, obj.r_hist, 0, hist_h, NORM_MINMAX, -1, Mat());
}

bool NTUCVTools::getLinePointinImageBorder(const Point& p1_in, const Point& p2_in, Point& p1_out, Point& p2_out, int rows, int cols)
{
  double m = (double)(p1_in.y - p2_in.y) / (double)(p1_in.x - p2_in.x + std::numeric_limits<double>::epsilon());
  double b = p1_in.y - (m * p1_in.x);

  std::vector<cv::Point> border_point;
  double x, y;
  // test for the line y = 0
  y = 0;
  x = (y - b) / m;
  if (x > 0 && x < cols)
    border_point.push_back(cv::Point(x, y));

  // test for the line y = img.rows
  y = rows;
  x = (y - b) / m;
  if (x > 0 && x < cols)
    border_point.push_back(cv::Point(x, y));

  // check intersection with horizontal lines x = 0
  x = 0;
  y = m * x + b;
  if (y > 0 && y < rows)
    border_point.push_back(cv::Point(x, y));

  x = cols;
  y = m * x + b;
  if (y > 0 && y < rows)
    border_point.push_back(cv::Point(x, y));

  if (border_point.size() < 2)
    return false;
  p1_out = border_point[0];
  p2_out = border_point[1];
  return true;
}

bool NTUCVTools::pointBetwenPoints(Point2f center, Point2f pt0, Point2f pt1)
{
  // Compare radius of circle with distance
  // of its center from given point
  if ((pt1.x - center.x) * (pt1.x - center.x) + (pt1.y - center.y) * (pt1.y - center.y) <=
      (pt0.x - center.x) * (pt0.x - center.x) + (pt0.y - center.y) * (pt0.y - center.y))
    return true;
  else
    return false;
}

void NTUCVTools::fillterEdgeTrasition(const Mat src, const Mat edge, int select, Mat& mask)
{
  int centerX = (edge.cols - 1) / 2.f;
  int centerY = (edge.rows - 1) / 2.f;

  Point o1(centerX, centerY);
  mask = Mat::zeros(edge.rows, edge.cols, CV_8UC1);

  for (int i = 1; i < edge.rows - 1; ++i)
  {
    for (int j = 1; j < edge.cols - 1; j++)
    {
      Point ptIn, ptOut;
      if (edge.at<uchar>(i, j) != 0)
      {
        Point p1(j, i), ptou1, ptou2;
        //! [1] get plane
        double a, b, c;
        getLine(p1, o1, a, b, c);
        //! [1]
        vector<pair<double, Point>> point_pt;
        for (int m = i - 1; m <= i + 1; ++m)
        {
          for (int n = j - 1; n <= j + 1; ++n)
          {
            if (m == i && n == j)
              continue;
            double distance = getDist(a, b, c, Point(n, m));
            point_pt.push_back(make_pair(distance, Point(n, m)));
          }
        }
        std::sort(point_pt.begin(), point_pt.end(), [](const pair<double, Point>& l, const pair<double, Point>& r) {
          if (l.second != r.second)
            return l.first < r.first;

          return l.first < r.first;
        });

        if (pointBetwenPoints(o1, p1, point_pt[0].second))
        {
          ptIn = point_pt[0].second;
          ptOut = point_pt[1].second;
        }
        else
        {
          ptOut = point_pt[0].second;
          ptIn = point_pt[1].second;
        }

        switch (select)
        {
        case 0: // black to white
        {
          if (src.at<uchar>(ptIn.y, ptIn.x) > src.at<uchar>(ptOut.y, ptOut.x))
            mask.at<uchar>(i, j) = 255;
          break;
        }
        case 1: //  white to black
          if (src.at<uchar>(ptIn.y, ptIn.x) < src.at<uchar>(ptOut.y, ptOut.x))
            mask.at<uchar>(i, j) = 255;
          break;
        default:
          mask.at<uchar>(i, j) = 255;
          break;
        }
      }
    }
  }
}

void NTUCVTools::drawCircle(QImage& image, Point2f pt, float radius, Scalar color, int thickness)
{
  QColor qcolor = scalar2qcolor(color);

  QPainter painter(&image);
  painter.setPen(QPen(qcolor, thickness));

  painter.drawEllipse(point2f2qpoint2f(pt), radius, radius);
}

void NTUCVTools::getLine(Point pt1, Point pt2, double& a, double& b, double& c)
{
  // (x- p1X) / (p2X - p1X) = (y - p1Y) / (p2Y - p1Y)
  a = pt1.y - pt2.y; // Note: this was incorrectly "y2 - y1" in the original answer
  b = pt2.x - pt1.x;
  c = pt1.x * pt2.y - pt2.x * pt1.y;
}

double NTUCVTools::getDist(double a, double b, double c, Point pt) { return abs(a * pt.x + b * pt.y + c) / sqrt(a * a + b * b); }

QJsonArray NTUCVTools::toJson(const std::vector<uchar>& myVec)
{
  QJsonArray result;
  std::copy(myVec.begin(), myVec.end(), std::back_inserter(result));
  return result;
}

Mat NTUCVTools::jsonToMat(const QJsonObject object)
{
  QJsonObject jsonMat;
  QVariantMap root_map = object.toVariantMap();
  QVariantMap stat_map = root_map["image"].toMap();
  int width = stat_map["cols"].toInt();
  int heigh = stat_map["rows"].toInt();
  QJsonArray data = stat_map["data"].toJsonArray();

  vector<uchar> imageVector; //(width * heigh);
  // std::copy(data.begin(), data.end(), std::back_inserter(imageVector));
  for (int i = 0; i < data.size(); ++i)
  {
    imageVector.push_back(data[i].toInt());
  }
  Mat m = Mat(heigh, width, CV_8UC1); // initialize matrix of uchar of 1-channel where you will store vec data
  // copy vector to mat
  memcpy(m.data, imageVector.data(), imageVector.size() * sizeof(uchar));
  return m;
}

QJsonObject NTUCVTools::matToJson(const cv::Mat mat)
{
  QJsonObject jsonMat, jsonValue;
  QJsonArray dataArray;
  int width = mat.cols;
  int heigh = mat.rows;

  vector<uchar> imageVector(width * heigh);

  imageVector.assign(mat.data, mat.data + mat.total());
  dataArray = toJson(imageVector);

  jsonValue.insert("rows", QJsonValue::fromVariant(heigh));
  jsonValue.insert("cols", QJsonValue::fromVariant(width));
  jsonValue.insert("data", dataArray);

  jsonMat["image"] = jsonValue;

  return jsonMat;
}

double NTUCVTools::templateMachingFast(vector<Mat> refs, vector<Mat> tpls, Point2f& pointCenter, int maxlevel)
{
  std::vector<cv::Mat> results;

  cv::Mat ref, tpl, res;

  // Process each level
  for (int level = maxlevel; level >= 0; level--)
  {
    ref = refs[level];
    tpl = tpls[level];
    res = cv::Mat::zeros(ref.size() + cv::Size(1, 1) - tpl.size(), CV_32FC1);

    if (level == maxlevel)
    {
      // On the smallest level, just perform regular template matching
      cv::matchTemplate(ref, tpl, res, CV_TM_CCORR_NORMED);
    }
    else
    {
      // On the next layers, template matching is performed on pre-defined
      // ROI areas.  We define the ROI using the template matching result
      // from the previous layer.

      cv::Mat mask;
      cv::pyrUp(results.back(), mask);

      cv::Mat mask8u;
      mask.convertTo(mask8u, CV_8U);

      // Find matches from previous layer
      std::vector<std::vector<cv::Point>> contours;
      cv::findContours(mask8u, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

      // Use the contours to define region of interest and
      // perform template matching on the areas
      for (int i = 0; i < contours.size(); i++)
      {
        cv::Rect r = cv::boundingRect(contours[i]);

        if (r.x + r.width > res.cols)
          r.width = res.cols - r.x;

        if (r.y + r.height > res.rows)
          r.height = res.rows - r.y;

        cv::Rect roi = r + (tpl.size() - cv::Size(1, 1));
        if (roi.x + roi.width > ref.cols)
          roi.width = ref.cols - roi.x;

        if (roi.y + roi.height > ref.rows)
          roi.height = ref.rows - roi.y;

        if (roi.width > tpl.cols && roi.height > tpl.rows)
          cv::matchTemplate(ref(roi), tpl, res(r), CV_TM_CCORR_NORMED);
      }
    }
    // Only keep good matches
    cv::threshold(res, res, 0.92, 1., CV_THRESH_TOZERO);
    results.push_back(res);
  }

  /// Localizing the best match with minMaxLoc
  double minval, maxval;
  cv::Point minloc, maxloc;
  cv::minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);

  int ptx = maxloc.x;
  int pty = maxloc.y;
  int r_w = tpls.front().cols;
  int r_h = tpls.front().rows;

  pointCenter = Point2f(ptx + r_w / 2.0, pty + r_h / 2.0); // Show me what you got

  return maxval;
}

void NTUCVTools::drawPolyLine(QImage &image, vector<Point2f> pts, Scalar color, int thickness)
{
    QColor qcolor = scalar2qcolor(color);

    QPainter painter(&image);
    painter.setPen(QPen(qcolor, thickness));

    QVector<QPoint> points;
    for (size_t i = 0; i < pts.size(); ++i)
    {
      points << QPoint(pts[i].x, pts[i].y);
    }
    painter.drawPolyline(points);
}
