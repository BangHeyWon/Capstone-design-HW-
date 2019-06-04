#include"opencv2/opencv.hpp"
#include<iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat srcImage = imread("solidWhiteCurve.jpg");
	Mat dstImage = srcImage.clone();
	//BGR 제한 값 설정
	int blue_threshold = 200;
	int green_threshold = 200;
	int red_threshold = 200;
	int bgr_threshold[3] = { blue_threshold, green_threshold, red_threshold };
	int nBlue, nGreen, nRed;

	for (int j = 0; j < dstImage.rows; j++)
	{
		for (int i = 0; i < dstImage.cols; i++)
		{
			nBlue = dstImage.at<Vec3b>(j, i)[0];
			nGreen = dstImage.at<Vec3b>(j, i)[1];
			nRed = dstImage.at<Vec3b>(j, i)[2];

			if (nBlue < blue_threshold || nGreen < green_threshold || nRed < red_threshold)
			{
				dstImage.at<Vec3b>(j, i)[0] = 0;//몽땅 깜장색으로 만들어주기
				dstImage.at<Vec3b>(j, i)[1] = 0;
				dstImage.at<Vec3b>(j, i)[2] = 0;
			}
		}
	}
	imshow("dstImage", dstImage);
	waitKey();
	return 0;
}