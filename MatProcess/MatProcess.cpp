#include <opencv2/opencv.hpp>
#include <math.h>
#include <numeric> 
#include <cmath>

using namespace cv;
using namespace std;

vector<double> line_angles; //

//显示缩放图像
void ShowImg(string winName, Mat src, int scale = 4)
{
	if (src.cols <= 0 || src.rows <= 0)
		return;

	Mat dst = src.clone();

	resize(dst, dst, Size(src.cols / scale, src.rows / scale));
	//line(dst, Point2f((dst.cols - 1) / 2.0, 0), Point2f((dst.cols - 1) / 2.0, dst.rows - 1), Scalar(0, 0, 255));
	imshow(winName, dst);
}

//dim：划分的柱条的数目，range：取值范围，必须保证range1>src.max()
vector<int> GetHist(vector<double> src, const int dim, double* range)
{
	double wide = (range[1] - range[0]) / dim;
	vector<int> dst(dim);
	for (int i = 0; i < src.size(); i++)
	{
		int index = (src[i] - range[0]) / wide;
		dst[index]++;
	}

	return dst;
}

//按直方图滤出频率最高区间的所有值
vector<double> GetHistFreq(vector<int> hist, vector<double> src, double* range)
{
	vector<double> res;
	int dims = hist.size();
	int maxPos = max_element(hist.begin(), hist.end()) - hist.begin();   //vector最大值下标
	int maxVal = hist[maxPos]; //vector最大值
	int range0 = range[0] + (range[1] - range[0]) * maxPos / dims;
	int range1 = range[0] + (range[1] - range[0]) * (maxPos + 1) / dims;
	for (int i = 0; i < src.size(); i++)
	{
		if (src[i] >= range0 && src[i] <= range1)
		{
			res.push_back(src[i]);
		}
	}
	return res;
}


//按照给定角度顺时针旋转图像
Mat RotateImg(Mat src, double angle)
{
	Point2f center((src.cols - 1) / 2.0, (src.rows - 1) / 2);
	Mat rot_matrix = getRotationMatrix2D(center, -angle, 1.0); //计算旋转矩阵

	Mat dst;
	warpAffine(src, dst, rot_matrix, src.size());

	return dst;
}


//获取图像的直线检测结果
vector<Vec4i> GetLinesMat(Mat src)
{
	Mat dst;
	cvtColor(src, dst, COLOR_BGR2GRAY);
	ShowImg("灰度图", dst);

	//equalizeHist(dst, dst);
	//ShowImg("均衡化", dst);

	threshold(dst, dst, 50, 255, THRESH_BINARY_INV);
	ShowImg("二值图", dst);

	Sobel(dst, dst, -1, 1, 0, 5);
	ShowImg("梯度图", dst);

	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 50, 0);

	Mat lineImg = Mat(Size(1920, 1200), CV_8UC1, Scalar(0));
	for (size_t i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		line(lineImg, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255), 1);
	}
	ShowImg("直线检测结果", lineImg);

	return lines;
}


//获取顺时针旋转角度，范围-90~90
double GetImgAngle(Mat src, vector<Vec4i> lines)
{
	vector<double> angles;
	for (size_t i = 0; i < lines.size(); i++)
	{
		Point2f pt1(lines[i][0], lines[i][1]), pt2(lines[i][2], lines[i][3]);

		double angle = (atan2((pt2.y - pt1.y), (pt2.x - pt1.x)) + CV_PI) * 180 / CV_PI; //到原点的角度，范围0~360
		angle = (angle > 180) ? angle - 180 : angle; //到原点的角度，范围0~180

		angles.push_back(90 - angle); //顺时针旋转的角度
	}

	vector<int> hist_vec;
	int dims = 180;
	double range[2]{ -90,90 };
	hist_vec = GetHist(angles, dims, range);
	vector<double> angle_freq = GetHistFreq(hist_vec, angles, range);
	double angle_avr = 0;
	for (size_t i = 0; i < angle_freq.size(); i++) {
		angle_avr += angle_freq[i] / angle_freq.size();
	}

	return angle_avr;
}

//获取条形码区域的上下边界
int* GetROI_TopBottom(vector<Vec4i> lines, double angle)
{
	//旋转直线检测结果图
	Mat src = Mat(Size(1920, 1200), CV_8UC1, Scalar(0));
	for (size_t i = 0; i < lines.size(); i++){
		Vec4i l = lines[i];
		line(src, Point(l[0], l[1]), Point(l[2], l[3]),  Scalar(255), 1);
	}
	Mat dst = RotateImg(src, angle);
	//ShowImg("直线检测结果1", dst);

	//统计每行白像素点数
	vector<int> point_nums(dst.rows);
	uchar* p;
	for (int i = 0; i < dst.rows; i++) {
		p = dst.ptr<uchar>(i);
		for (int j = 0; j < dst.cols; j++) {
			if (int(p[j]) > 0) {
				point_nums[i]++;
			}
		}
	}
	
	//求条形码行序数的上下界限
	const int countNum = 30; //每行像素点数大于countNum时开始计continNum
	const int continNum = 10; //continNum表示连续几行像素点数大于countNum时视为边界
	int top = 0, bottom = 0, count_top = 0, count_bottom = 0;
	for (int i = 0; i < point_nums.size(); i++) {
		if (point_nums[i] >= countNum) {
			count_top++;		                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
		}
		if (point_nums[i] < countNum) {
			count_top = 0;
		}
		if (count_top >= continNum) {
			top = i - count_top;
			break;
		}
	}
	for (int i = point_nums.size() - 1; i > 0; i--) {
		if (point_nums[i] >= countNum) {
			count_bottom++;
		}
		if (point_nums[i] < countNum) {
			count_bottom = 0;
		}
		if (count_bottom >= continNum) {
			bottom = i + count_bottom;
			break;
		}
	}

	//显示上下界限
	line(dst, Point(0, top), Point(dst.cols - 1, top), Scalar(255), 5);
	line(dst, Point(0, bottom), Point(dst.cols - 1, bottom), Scalar(255), 5);
	//ShowImg("直线检测结果", dst);
	
	return new int[2]{ top,bottom };
}

//获取条形码的左右边界
int* GetROI_LeftRight(int* roi_tb,  Mat src)
{
	Mat dst, roi;

	//根据上下边界获取ROI区域
	cvtColor(src, dst, COLOR_BGR2GRAY);
	//equalizeHist(dst, dst);
	
	int top = roi_tb[0], bottom = roi_tb[1];
	roi = dst(Rect(0, roi_tb[0], src.cols, roi_tb[1] - roi_tb[0]));
	//ShowImg("ROI区域灰度图", roi);
	threshold(roi, roi, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
	//ShowImg("ROI区域二值图", roi);

	////膨胀
	//Mat element = getStructuringElement(MORPH_RECT, Size(10, 10));
	//dilate(roi, dst, element);
	//ShowImg("ROI膨胀结果", dst);

	//统计每列黑像素点数
	vector<int> black_nums(roi.cols);
	vector<int> white_nums(roi.cols);
	uchar* p;
	for (int i = 0; i < roi.rows; i++) {
		p = roi.ptr<uchar>(i);
		for (int j = 0; j < roi.cols; j++) {
			if (int(p[j]) == 0)
				black_nums[j]++;
			else
				white_nums[j]++;
		}
	}

	//记录全黑列、全白列连续数
	vector<int> black_cols(roi.cols);
	vector<int> white_cols(roi.cols);
	for (int i = 1; i < roi.cols; i++) {
		//全黑列
		if (black_nums[i] >= 0.8 * roi.rows)
			black_cols[i] = black_cols[i - 1] + 1;

		//全白列
		if (white_nums[i] >= 0.8 * roi.rows)
			white_cols[i] = white_cols[i - 1] + 1;
	}
	int black_max = 0;
	for (int i = roi.cols - 1; i > 0; i--) {
		if (black_cols[i] > 0) {
			if (black_max == 0) {
				black_max = black_cols[i];
			}
			else if (black_max > 0) {
				black_cols[i] = black_max;
			}
		}
		else
		{
			black_max = 0;
		}
	}
	int white_max = 0;
	for (int i = roi.cols - 1; i > 0; i--) {
		if (white_cols[i] > 0) {
			if (white_max == 0) {
				white_max = white_cols[i];
			}
			else if (white_max > 0) {
				white_cols[i] = white_max;
			}
		}
		else
		{
			white_max = 0;
		}
	}

	//判断每个分块所属类别
	//全黑列连续列数<50或全白列连续列数<50，说明该区域可能是条形码区域
	//全黑列连续列数>200或全白列连续列数>200，说明该区域不可能是条形码区域
	int flag = 0; //指示每个分块的类别：0-不可能是条形码区域，1-黑条纹，2-白条纹
	int lineNums = 0;//白条纹数
	int left = 0, right = roi.cols-1; //左边界、右边界
	int isbn_width = 50;
	int notisbn_width = 100;
	for (int i = 1; i < roi.cols; i++) {
		//当前列为白条纹分块
		if (white_cols[i] < isbn_width && white_cols[i] > 0 && flag != 2) {
			//上一分块为不可能是条形码区域
			if (flag == 0 && lineNums < 8)//lineNums<8是为了避免{...,条形码,不可能,白条纹,..}的情况
				left = i;

			flag = 2;
		}

		//黑条纹只在白条纹之后才可能出现
		else if (black_cols[i] < isbn_width && black_cols[i] > 0 && flag == 2) {
			lineNums++;
			flag = 1;
		}

		//非ISBN码区域
		else if ((black_cols[i] > notisbn_width || white_cols[i] > notisbn_width) && flag != 0) {
			if (lineNums > 8) {
				right = i - 1;
				break;
			}
				
			flag = 0;
		}		
	}

	//if (right > left) {
	//	Mat roi1 = roi(Rect(left, 0, right - left, roi.rows - 1));
	//	ShowImg("ROI最终", roi1);
	//}
		
	return new int[2]{ left,right };
}


vector<Mat> GetNumROI(Mat src, int* tb, int* lr)
{
	//截取ISBN数字区域
	int word_size = (tb[1] - tb[0]) / 5; //一个数字的大小
	int left = lr[0] - 1.5 * word_size, right = lr[1] + 0.5 * word_size, top = tb[1], bottom = tb[1] + 1.2 * word_size;
	Mat isbn_roi = src(Rect(left, top, right - left, bottom - top));

	//预处理
	Mat dst;
	cvtColor(isbn_roi, dst, COLOR_BGR2GRAY);
	threshold(dst, dst, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
	ShowImg("数字区域", dst);
	
	//矩形框出数字区域
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS);
	vector<Rect> boundRect;
	for (size_t i = 0; i < contours.size(); i++) {
		Rect rect = boundingRect(contours[i]);

		boundRect.push_back(rect);
		rectangle(isbn_roi, rect.tl(), rect.br(), Scalar(255, 0, 0), 3);
		//if (rect.height > 10 && rect.width > 10 && rect.width < 1.5 * rect.height) {
		//	boundRect.push_back(rect);
		//	rectangle(isbn_roi, rect.tl(), rect.br(), Scalar(255, 0, 0), 3);
		//}
	}
	imshow("矩形框数字", isbn_roi);
	
	//去除非数字矩形
	vector<Rect> boundRect_Filter;
	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(dst, dst, element);//膨胀
	double area_rate = 0;
	for (size_t i = 0; i < boundRect.size(); i++) {
		int left = boundRect[i].tl().x, right = boundRect[i].br().x;
		int top = boundRect[i].tl().y, bottom = boundRect[i].br().y;
		for (int j = left; j < right; j++) {
			for (int k = top; k < bottom; k++) {
				area_rate += dst.at<uchar>(k,j);
			}
		}
		area_rate = area_rate / boundRect[i].width / boundRect[i].height / 255;
		if (area_rate < 0.9)
			boundRect_Filter.push_back(boundRect[i]);
	}



	//	数字区域按从左到右排序
	vector<Mat> mat;
	vector<int> mat_col;
	for (size_t i = 0; i < boundRect_Filter.size(); i++) {
		Mat temp = dst(boundRect_Filter[i]);
		mat.push_back(temp.clone());
		mat_col.push_back(boundRect_Filter[i].tl().x);
	}

	vector<size_t> index(mat.size());
	vector<Mat> res;
	iota(index.begin(), index.end(), 0);
	sort(index.begin(), index.end(), [&mat_col](size_t i1, size_t i2) {return mat_col[i1] < mat_col[i2]; });
	for (int i = 0; i < mat.size(); i++) {
		res.push_back(mat[index[i]]);
	}

	return res;
}