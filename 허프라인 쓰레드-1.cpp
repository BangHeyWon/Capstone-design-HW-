#include"opencv2/opencv.hpp"
#include <cstdio>
#include <mutex>
#include <thread>
#include<iostream>
#include<vector>
#include<conio.h>
#include <math.h>
#include"constant.h" // 상수가 정의되어있는 헤더파일

// 기울기 직선이 영상의 끝과 만나는 점 -> 기울기 직선이 ROI영역과 만나는 점

using namespace std;
using namespace cv;
std::mutex mtx_lock;//lock()/unlock()함수를 통해 공유자원에 접근하고 관리할 수 있다.

Mat region_of_interest(Mat canny_image); // ROI 
Mat hough_lines(Mat ROI_image, Mat srcImage_COLOR); // 허프 변환
Mat weight_image(Mat hough_image, Mat srcImage); // 원본 이미지에 검출 된 선 합치기

void draw_lines(Mat ROI_image, vector<Vec4i> lines, Mat srcImage_COLOR); // 선 그리기 함수.
void Draw_fitLine_L(Mat line_image, Vec4f output); // fitLine함수에서 추출된 output을 가지고 대표선 그리기
void Draw_fitLine_R(Mat line_image, Vec4f output);
void print_Line_and_point(Mat line_image, vector<Point2f> Pointxy, int L); //Draw_fitLine함수를 호출하며, 대표점 출력
void calc_x_y_point(Mat line_image, float vx, float vy, float x, float y, int L); // 기울기로 x절편 y 절편 구하기
void transform_point(Mat line_image);//아직 구현안됨.

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
		
		inputVideo >> srcImage; // frame을 읽어온다.
		srcImage_COLOR = srcImage.clone(); // 원본이미지 복사(컬러)
		cvtColor(srcImage, srcImage_GRAY, COLOR_BGR2GRAY); //흑백으로 변경
		Canny(srcImage_GRAY, canny_image, canny_threshold_1, canny_threshold_2, apertureSize); // edge 검출
		ROI_image = region_of_interest(canny_image); // ROI
		imshow("d", ROI_image);
		//thread L1;
		//thread R1;
		hough_image = hough_lines(ROI_image, srcImage_COLOR); //허프변환
		result = weight_image(hough_image, srcImage_COLOR);
		imshow("result", result);

		int ckey = waitKey(delay);
		if (ckey == ESC) break;
	}
	return 0;
}

Mat region_of_interest(Mat canny_image) // 원본이미지의 다각형 부분 추출
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
		four_points.clear(); // 전역 벡터 초기화.
		four_points.resize(4);
		fitLine(Pointxy, output, DIST_L2, fit_param, fit_reps, fit_aeps); // vx, vy, x, y 값 output에 저장.
		if (L == Left) Draw_fitLine_L(line_image, output); //왼쪽 대표선 그리기
		if (L == Right) Draw_fitLine_R(line_image, output); //오른쪽 대표선 그리기

		Point_<float> pt(output[2], output[3]); // 대표점
		circle(line_image, pt, radius, Scalar(0, 255, 0), line_thickness);
		if (L == Right) // 오른쪽
		{
			mtx_lock.lock();
			cout << "Right ( " << output[2] << " , " << output[3] << " ) " << endl;
			mtx_lock.unlock();
		}
		else if (L == Left) // 왼쪽
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
	line(line_image, pt1, pt2, Scalar(255, 255, 255), line_thickness); // 대표 차선
	calc_x_y_point(line_image, vx, vy, x, y, Left);
}

void Draw_fitLine_R(Mat line_image, Vec4f output)
{
	float vx = output[0];
	float vy = output[1];
	float x = output[2];
	float y = output[3];

	// 이렇게 공식을 사용하는 이유??
	int x1 = (int)x;
	int y1 = (int)y;
	int x2 = (int)(x1 + 200 * vx);
	int y2 = (int)(y1 + 200 * vy);

	Point_<int> pt1(x1, y1), pt2(x2, y2);
	line(line_image, pt1, pt2, Scalar(255, 255, 255), line_thickness); // 대표 차선
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

		if (abs(slope_degree) < Max_slope_degree && abs(slope_degree) > Min_slope_degree) //수평기울기 , 수직기울기
		{
			// 직선 근사 따로 따로
			if (slope_degree > inclination_standard)//0보다 작으면 오른쪽
			{
				line(line_image, pt1, pt2, Scalar(255, 0, 0), line_thickness);
				R_Pointxy.push_back(pt1);
				R_Pointxy.push_back(pt2);

			}
			else if (slope_degree < inclination_standard)//0보다 크면 왼쪽
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
	float inclination = vy / vx; // 벡터를 이용한 기울기
	int width = line_image.size().width;
	int height = line_image.size().height;
	int y_point = (int)(y - inclination * x); // y 절편

	int x_down;
	int y_down;
	int x_up;
	int y_up;

	if (L == Left) // 왼쪽 절편 출력
	{
		//아래 점 찾기
		if (y_point > height) // 차선이 영상 아랫면에서 끊길때
		{
			x_down = (int)((height - y_point) / inclination);// 아랫면과 차선이 만나는 좌표
			y_down = height;
		}
		else// 차선이 왼쪽 면에서 끊길 때 
		{
			x_down = 0;
			y_down = y_point;// 왼쪽 면과 차선이 만나는 좌표
		}
		// 윗점 찾기
		if ((height / 2 - y_point) / inclination > width) // 오른쪽 면과 차선이 만나는 좌표
		{
			x_up = width;
			y_up = (int)(inclination * width + y_point);
		}
		else // 위쪽 면과 차선이 만나는 좌표
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
	else if (L == Right) // 오른쪽 절편 그리기
	{
		// 아래점 찾기
		if (inclination * width + y_point > height) // 차선이 영상 아랫면에서 끊길때
		{
			x_down = (int)((height - y_point) / inclination); // 아랫면과 차선이 만나는 좌표
			y_down = height;
		}
		else // 차선이 오른쪽 면에서 끊길 때 
		{
			x_down = width;
			y_down = (int)(inclination * width + y_point); // 오른쪽 면과 차선이 만나는 좌표
		}

		// 윗점 찾기
		if ((height / 2 - y_point) / inclination < 0) // 왼쪽 면과 차선이 만나는 좌표
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

	// 좌표점은 좌상-> 좌하-> 우상-> 우하

	mask = getPerspectiveTransform(inputPoint, outputPoint); // 인자가 Point2f이여야함.
	warpPerspective(line_image, out, mask, out.size());
	imshow("line_image", line_image);
	imshow("out", out);
}

// 2차원 영상의 행렬 좌표를 1차원으로 줄인다.

// void fitLine(InputArray points, OutputArray line, int distType, double param, double reps, double aeps)
// points로 주어진 이미지에 대해 직선 근사시켜 line에 저장한다. 2D인경우 line은 (vx, vy, x0, y0)의 직선 요소를 저장,
// (vx, vy)는 직선의 정규화된 방향 벡터이고, (x0, y0)는 직선 위의 한 점이다. param은 0이면 최적의 값을 게산한다.
//reps 와 aeps는 각각 반지름 각도의 충분한 정확도로 0.01을 넣는다.

// 해야할 것.

// 상수 , HSV 도 햇빛에 따라 다름.
// 상수의 대체 방안, 최대한 안쓰기
// 노이즈 최소화.
// 그림자 제거

// birdeyes view
//1. x절편 y절편 구하기