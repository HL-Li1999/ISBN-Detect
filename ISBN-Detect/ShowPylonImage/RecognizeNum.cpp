#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/opencv.hpp>
#include "MatProcess.h"

using namespace std;
using namespace cv;

vector<vector<double>> GetMatirxData(string fileName)
{
	vector<vector<double>> res;

	string tmp = "";
	ifstream fileStream;
	fileStream.open(fileName, ios::in);//ios::in 表示以只读的方式读取文件
	if (fileStream.fail()) {
		throw logic_error("read file fail");
	}
	else {
		while (getline(fileStream, tmp, '\n')) {
			istringstream is(tmp);

			vector<double> vec_tmp;
			string str_tmp = "";
			while (is >> str_tmp) {
				vec_tmp.push_back(stod(str_tmp));
			}
			res.push_back(vec_tmp);
		}
	}

	return res;
}

template<class T>
T MatMul(vector<vector<T>> A, vector<vector<T>> B, int row_A, int col_B)
{
	T res = 0;
	for (int i = 0; i < A[0].size(); i++) {
		res += A[row_A][i] * B[i][col_B];
	}
	return res;
}

//输入二值图像，输出该图为各类的概率
vector<double> GetProbability(vector<vector<double>> W, Mat img)
{
	const int rows = 10, cols = 2401;

	Mat tmp;
	resize(img, tmp, Size(40, 60));

	vector < vector<double>> X;
	vector<double> x(1, 1.0);
	X.push_back(x);

	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < 40; j++) {
			vector<double> x(1, tmp.at<uchar>(i, j) / 255.0);
			X.push_back(x);
		}
	}

	vector<double> P;
	double P_sum = 0;
	for (int i = 0; i < 10; i++) {
		double p = exp(MatMul(W, X, i, 0));
		P.push_back(p);
		P_sum += p;
	}

	for (int i = 0; i < 10; i++) {
		P[i] = P[i] / P_sum;
	}

	return P;
}

vector<int> GetRecoResult(Mat src)
{
	vector<vector<double>> W = GetMatirxData("w08310334.txt");
	vector<int> res;

	//【1】直线检测
	vector<Vec4i> lines = GetLinesMat(src);

	//【2】旋转图像
	double angle = GetImgAngle(src, lines);
	Mat rotate_img = RotateImg(src, angle);

	//【3】截取条形码区域
	int* tb = GetROI_TopBottom(lines, angle);
	int* lr = GetROI_LeftRight(tb, rotate_img);
	vector<Mat> num_mat = GetNumROI(rotate_img, tb, lr);
	for (int i = 0; i < num_mat.size(); i++) {
		vector<double> P = GetProbability(W, num_mat[i]);
		auto maxP = max_element(P.begin(), P.end()) - P.begin();
		res.push_back(maxP);
	}
	if (res.size() > 13)
		res.pop_back();

	return res;
}