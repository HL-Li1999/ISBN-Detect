#include "MatProcess.h"
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;


int main()
{
	Mat src = imread("data/60.jpg");
	ShowImg("原图", src);

	//【1】直线检测
	vector<Vec4i> lines = GetLinesMat(src);

	//【2】旋转图像
	double angle = GetImgAngle(src, lines);
	Mat rotate_img = RotateImg(src, angle);
	ShowImg("旋转结果", rotate_img);
	
	//【3】截取条形码区域
	int* tb = GetROI_TopBottom(lines, angle);
	int* lr = GetROI_LeftRight(tb, rotate_img);
	vector<Mat> num_mat = GetNumROI(rotate_img, tb, lr);
	for (int i = 0; i < num_mat.size(); i++) {
		namedWindow(to_string(i));
		if (i < num_mat.size() / 2)
			moveWindow(to_string(i), 500, i * 100);
		else
			moveWindow(to_string(i), 800, (i - num_mat.size() / 2) * 100);
		ShowImg(to_string(i), num_mat[i],1);
		imwrite(to_string(i) + "_0019" + ".jpg", num_mat[i]);
	}

	while (1){
		waitKey(10000);
	}
}