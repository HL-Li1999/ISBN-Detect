#pragma once
#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

vector<vector<double>> GetMatirxData(string fileName);
vector<double> GetProbability(vector<vector<double>> W, Mat img);
vector<int> GetRecoResult(Mat src);