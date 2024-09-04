#ifndef __MATPROCESS_H
#define __MATPROCESS_H

#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void ShowImg(string winName, Mat src, int scale = 4);
Mat RotateImg(Mat src, double angle);

vector<Vec4i> GetLinesMat(Mat src);
double GetImgAngle(Mat src, vector<Vec4i> lines);
int* GetROI_TopBottom(vector<Vec4i> lines, double angle);
int* GetROI_LeftRight(int* roi_tb, Mat src);
vector<Mat> GetNumROI(Mat src, int* tb, int* lr);

#endif