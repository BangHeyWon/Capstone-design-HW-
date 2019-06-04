#include"opencv2/opencv.hpp"
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;
//BGR = 0 ~ 255
int main()
{
	Mat srcImage = imread("solidWhiteCurve.jpg");
	Mat dstImage = srcImage.clone();
	Mat dstImage2 = srcImage.clone();
	Mat mask(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(0, 0, 0));//8bit Unsigned Char 3채널

	int height = srcImage.size().height;
	int width = srcImage.size().width;

	Point pts[4] = { Point(0,height),Point(width / 8 - 45, height / 2 + 60), Point(width / 8 * 7 + 45, height / 2 + 60), Point(width,height) };
	//사다리꼴 범위 정해주기
	//왼쪽 위에가 (0,0)

	fillConvexPoly(mask, pts, 4, Scalar(255, 255, 255));
	//모형을 만들어주는 함수 같음.
	//이미지,범위,개수,흰색으로 채워라.
	imshow("mask", mask);
	imshow("srcImage", srcImage);
	waitKey();
	bitwise_and(dstImage, mask, dstImage2);//사다리꼴 범위만 나오게 반전.
	imshow("dstImage2", dstImage2);
	waitKey();
	//BGR 제한 값 설정
	int blue_threshold = 200;
	int green_threshold = 200;
	int red_threshold = 200;
	int bgr_threshold[3] = { blue_threshold, green_threshold, red_threshold };
	int nBlue, nGreen, nRed;

	//하얀색 추출 부분
	//큰for문은 임계값을 설정해주는것.
	for (int j = 0; j < dstImage2.rows; j++)
	{
		for (int i = 0; i < dstImage2.cols; i++)
		{
			nBlue = dstImage2.at<Vec3b>(j, i)[0];
			nGreen = dstImage2.at<Vec3b>(j, i)[1];
			nRed = dstImage2.at<Vec3b>(j, i)[2];

			if (nBlue < blue_threshold || nGreen < green_threshold || nRed < red_threshold)
			{
				dstImage2.at<Vec3b>(j, i)[0] = 0;
				dstImage2.at<Vec3b>(j, i)[1] = 0;
				dstImage2.at<Vec3b>(j, i)[2] = 0;
			}
		}
	}
	imshow("dstImage2", dstImage2);
	waitKey();
	return 0;
}