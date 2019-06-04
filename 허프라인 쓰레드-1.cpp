#include"opencv2/opencv.hpp"
#include <cstdio>
#include <mutex>
#include <thread>
#include<iostream>
#include<vector>
#include<conio.h>
#include <math.h>
#include"constant.h" // ����� ���ǵǾ��ִ� �������

// ���� ������ ������ ���� ������ �� -> ���� ������ ROI������ ������ ��

using namespace std;
using namespace cv;
std::mutex mtx_lock;//lock()/unlock()�Լ��� ���� �����ڿ��� �����ϰ� ������ �� �ִ�.

Mat region_of_interest(Mat canny_image); // ROI 
Mat hough_lines(Mat ROI_image, Mat srcImage_COLOR); // ���� ��ȯ
Mat weight_image(Mat hough_image, Mat srcImage); // ���� �̹����� ���� �� �� ��ġ��

void draw_lines(Mat ROI_image, vector<Vec4i> lines, Mat srcImage_COLOR); // �� �׸��� �Լ�.
void Draw_fitLine_L(Mat line_image, Vec4f output); // fitLine�Լ����� ����� output�� ������ ��ǥ�� �׸���
void Draw_fitLine_R(Mat line_image, Vec4f output);
void print_Line_and_point(Mat line_image, vector<Point2f> Pointxy, int L); //Draw_fitLine�Լ��� ȣ���ϸ�, ��ǥ�� ���
void calc_x_y_point(Mat line_image, float vx, float vy, float x, float y, int L); // ����� x���� y ���� ���ϱ�
void transform_point(Mat line_image);//���� �����ȵ�.

vector<Point2f> four_points;

int main()
{
	VideoCapture inputVideo("ssuderive.mp4");
	Mat srcImage_GRAY;
	Mat srcImage_COLOR;
	Mat srcImage;
	Mat hough_image;
	Mat canny_image;
	Mat ROI_image;
	Mat result;
	int n = 1;
	int m = 1;

	int fps = (int)(inputVideo.get(CAP_PROP_FPS));
	int delay = Delay / fps;

	for (;;)
	{
		
		inputVideo >> srcImage; // frame�� �о�´�.
		srcImage_COLOR = srcImage.clone(); // �����̹��� ����(�÷�)
		cvtColor(srcImage, srcImage_GRAY, COLOR_BGR2GRAY); //������� ����
		Canny(srcImage_GRAY, canny_image, canny_threshold_1, canny_threshold_2, apertureSize); // edge ����
		ROI_image = region_of_interest(canny_image); // ROI
		imshow("d", ROI_image);
		//thread L1;
		//thread R1;
		hough_image = hough_lines(ROI_image, srcImage_COLOR); //������ȯ
		result = weight_image(hough_image, srcImage_COLOR);
		imshow("result", result);

		int ckey = waitKey(delay);
		if (ckey == ESC) break;
	}
	return 0;
}

Mat region_of_interest(Mat canny_image) // �����̹����� �ٰ��� �κ� ����
{
	int height = canny_image.size().height;
	int width = canny_image.size().width;
	Mat dst_image;

	Mat mask(canny_image.rows, canny_image.cols, CV_8UC1, Scalar(0));
	Point pts[6] = { Point(0,height),Point(0, height / 3 * 2), Point(width / 3, height / 2), Point(width / 3 * 2 , height / 2), Point(width, height / 3 * 2), Point(width,height) };
	fillConvexPoly(mask, pts, 6, Scalar(255));
	bitwise_and(canny_image, mask, dst_image);
	return dst_image;
}

Mat hough_lines(Mat ROI_image, Mat srcImage_COLOR)
{
	Mat line_image(ROI_image.size(), CV_8UC3, Scalar(0, 0, 0));

	vector<Vec4i> lines;
	HoughLinesP(ROI_image, lines, rho, delta, hough_threshold, MinLineLength, MaxLineGap);
	draw_lines(line_image, lines, srcImage_COLOR);

	return line_image;
}

Mat weight_image(Mat hough_image, Mat srcImage)
{
	Mat outMat;
	addWeighted(srcImage, alpha, hough_image, beta, gamma, outMat);
	return outMat;
}

void print_Line_and_point(Mat line_image, vector<Point2f> Pointxy, int L)
{
	if (Pointxy.size() != 0)
	{
		Vec4f output;
		four_points.clear(); // ���� ���� �ʱ�ȭ.
		four_points.resize(4);
		fitLine(Pointxy, output, DIST_L2, fit_param, fit_reps, fit_aeps); // vx, vy, x, y �� output�� ����.
		if (L == Left) Draw_fitLine_L(line_image, output); //���� ��ǥ�� �׸���
		if (L == Right) Draw_fitLine_R(line_image, output); //������ ��ǥ�� �׸���

		Point_<float> pt(output[2], output[3]); // ��ǥ��
		circle(line_image, pt, radius, Scalar(0, 255, 0), line_thickness);
		if (L == Right) // ������
		{
			mtx_lock.lock();
			cout << "Right ( " << output[2] << " , " << output[3] << " ) " << endl;
			mtx_lock.unlock();
		}
		else if (L == Left) // ����
		{
			mtx_lock.lock();
			cout << "Left ( " << output[2] << " , " << output[3] << " ) " << endl;
			mtx_lock.unlock();
		}	
	}
}

void Draw_fitLine_L(Mat line_image, Vec4f output)
{
	float vx = output[0];
	float vy = output[1];
	float x = output[2];
	float y = output[3];

	int x1 = (int)x;
	int y1 = (int)y;
	int x2 = (int)(x1 - 200 * vx);
	int y2 = (int)(y1 - 200 * vy);

	Point_<int> pt1(x1, y1), pt2(x2, y2);
	line(line_image, pt1, pt2, Scalar(255, 255, 255), line_thickness); // ��ǥ ����
	calc_x_y_point(line_image, vx, vy, x, y, Left);
}

void Draw_fitLine_R(Mat line_image, Vec4f output)
{
	float vx = output[0];
	float vy = output[1];
	float x = output[2];
	float y = output[3];

	// �̷��� ������ ����ϴ� ����??
	int x1 = (int)x;
	int y1 = (int)y;
	int x2 = (int)(x1 + 200 * vx);
	int y2 = (int)(y1 + 200 * vy);

	Point_<int> pt1(x1, y1), pt2(x2, y2);
	line(line_image, pt1, pt2, Scalar(255, 255, 255), line_thickness); // ��ǥ ����
	calc_x_y_point(line_image, vx, vy, x, y, Right);
}

void draw_lines(Mat line_image, vector<Vec4i> lines, Mat srcImage_COLOR)
{
	Vec4i params;

	vector<Point2f> L_Pointxy;
	vector<Point2f> R_Pointxy;

	int x1, y1, x2, y2;
	double slope_degree;
	int height = line_image.size().height;

	for (int k = 0; k < lines.size(); k++)
	{
		params = lines[k];
		x1 = params[0];
		y1 = params[1];
		x2 = params[2];
		y2 = params[3];
		Point_<int> pt1(x1, y1), pt2(x2, y2);
		slope_degree = atan2((height - pt1.y) - (height - pt2.y), pt1.x - pt2.x) * 180 / CV_PI;

		if (abs(slope_degree) < Max_slope_degree && abs(slope_degree) > Min_slope_degree) //������� , ��������
		{
			// ���� �ٻ� ���� ����
			if (slope_degree > inclination_standard)//0���� ������ ������
			{
				line(line_image, pt1, pt2, Scalar(255, 0, 0), line_thickness);
				R_Pointxy.push_back(pt1);
				R_Pointxy.push_back(pt2);

			}
			else if (slope_degree < inclination_standard)//0���� ũ�� ����
			{
				line(line_image, pt1, pt2, Scalar(255, 0, 0), line_thickness);
				L_Pointxy.push_back(pt1);
				L_Pointxy.push_back(pt2);
			}
		}
	}
	thread R1(print_Line_and_point, line_image, R_Pointxy, Right);
	thread L1(print_Line_and_point, line_image, L_Pointxy, Left);
//	R1.join();
//	L1.join();
	R1.detach();
	L1.detach();
}

void calc_x_y_point(Mat line_image, float vx, float vy, float x, float y, int L)
{
	float inclination = vy / vx; // ���͸� �̿��� ����
	int width = line_image.size().width;
	int height = line_image.size().height;
	int y_point = (int)(y - inclination * x); // y ����

	int x_down;
	int y_down;
	int x_up;
	int y_up;

	if (L == Left) // ���� ���� ���
	{
		//�Ʒ� �� ã��
		if (y_point > height) // ������ ���� �Ʒ��鿡�� ���涧
		{
			x_down = (int)((height - y_point) / inclination);// �Ʒ���� ������ ������ ��ǥ
			y_down = height;
		}
		else// ������ ���� �鿡�� ���� �� 
		{
			x_down = 0;
			y_down = y_point;// ���� ��� ������ ������ ��ǥ
		}
		// ���� ã��
		if ((height / 2 - y_point) / inclination > width) // ������ ��� ������ ������ ��ǥ
		{
			x_up = width;
			y_up = (int)(inclination * width + y_point);
		}
		else // ���� ��� ������ ������ ��ǥ
		{
			x_up = (int)((height / 2 - y_point) / inclination);
			y_up = height / 2;
		}
		Point2f pt1(x_down, y_down), pt2(x_up, y_up);
		four_points[0] = pt1;
		four_points[1] = pt2;
		circle(line_image, pt1, 3, Scalar(0, 0, 255), 3);
		circle(line_image, pt2, 3, Scalar(0, 0, 255), 3);
	}
	else if (L == Right) // ������ ���� �׸���
	{
		// �Ʒ��� ã��
		if (inclination * width + y_point > height) // ������ ���� �Ʒ��鿡�� ���涧
		{
			x_down = (int)((height - y_point) / inclination); // �Ʒ���� ������ ������ ��ǥ
			y_down = height;
		}
		else // ������ ������ �鿡�� ���� �� 
		{
			x_down = width;
			y_down = (int)(inclination * width + y_point); // ������ ��� ������ ������ ��ǥ
		}

		// ���� ã��
		if ((height / 2 - y_point) / inclination < 0) // ���� ��� ������ ������ ��ǥ
		{
			x_up = 0;
			y_up = y_point;
		}
		else
		{
			x_up = (int)((height / 2 - y_point) / inclination);
			y_up = height / 2;
		}

		Point pt3(x_down, y_down), pt4(x_up, y_up);
		four_points[2] = pt3;
		four_points[3] = pt4;
		circle(line_image, pt3, 3, Scalar(0, 0, 255), 3);
		circle(line_image, pt4, 3, Scalar(0, 0, 255), 3);
	}
	imshow("line_image", line_image);
}

void transform_point(Mat line_image)
{
	Mat mask(4, 2, CV_8UC1);

	Mat out;

	Point2f inputPoint[4];
	Point2f outputPoint[4];

	int height = line_image.size().height;
	int width = line_image.size().width;

	inputPoint[0] = four_points[0]; // pt1
	inputPoint[1] = four_points[1]; // pt2
	inputPoint[2] = four_points[2]; // pt3
	inputPoint[3] = four_points[3]; // pt4

	outputPoint[0] = Point(width / 4, height);
	outputPoint[1] = Point(width / 4, 0);
	outputPoint[2] = Point(width / 4 * 3, height);
	outputPoint[3] = Point(width / 4 * 3, 0);

	// ��ǥ���� �»�-> ����-> ���-> ����

	mask = getPerspectiveTransform(inputPoint, outputPoint); // ���ڰ� Point2f�̿�����.
	warpPerspective(line_image, out, mask, out.size());
	imshow("line_image", line_image);
	imshow("out", out);
}

// 2���� ������ ��� ��ǥ�� 1�������� ���δ�.

// void fitLine(InputArray points, OutputArray line, int distType, double param, double reps, double aeps)
// points�� �־��� �̹����� ���� ���� �ٻ���� line�� �����Ѵ�. 2D�ΰ�� line�� (vx, vy, x0, y0)�� ���� ��Ҹ� ����,
// (vx, vy)�� ������ ����ȭ�� ���� �����̰�, (x0, y0)�� ���� ���� �� ���̴�. param�� 0�̸� ������ ���� �Ի��Ѵ�.
//reps �� aeps�� ���� ������ ������ ����� ��Ȯ���� 0.01�� �ִ´�.

// �ؾ��� ��.

// ��� , HSV �� �޺��� ���� �ٸ�.
// ����� ��ü ���, �ִ��� �Ⱦ���
// ������ �ּ�ȭ.
// �׸��� ����

// birdeyes view
//1. x���� y���� ���ϱ�