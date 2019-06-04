#include"opencv2/opencv.hpp"
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;
//BGR = 0 ~ 255 

/*Mat region_of_interest(Mat img_edges, Point *points)
{
	Mat img_mask = Mat::zeros(img_edges.rows, img_edges.cols, CV_8UC1);

	Scalar ignore_mask_color = Scalar(255, 255, 255);
	const Point* ppt[1] = { points };
	int npt[] = { 4 };

	fillPoly(img_mask, ppt, npt, 1, Scalar(255, 255, 255), LINE_8);

	Mat img_masked;
	bitwise_and(img_edges, img_mask, img_masked);

	return img_masked;
}*/

int main()
{
	Mat srcImage = imread("solidWhiteCurve.jpg");
	Mat dstImage = srcImage.clone();
	Mat dstImage2 = srcImage.clone();
	Mat result = srcImage.clone();
	Mat mask(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(0, 0, 0));//8bit Unsigned Char 3ä��
	int height = srcImage.size().height;
	int width = srcImage.size().width;

	Point pts[4] = { Point(0,height),Point(width / 8 - 45, height / 2 + 60), Point(width / 8 * 7 + 45, height / 2 + 60), Point(width,height) };
	//��ٸ��� ���� �����ֱ�
	//���� ������ (0,0)

	fillConvexPoly(mask, pts, 4, Scalar(255, 255, 255));
	//������ ������ִ� �Լ� ����.
	//�̹���,����,����,������� ä����.
	imshow("mask", mask);
	imshow("srcImage", srcImage);
	waitKey();
	bitwise_and(dstImage, mask, dstImage2);//��ٸ��� ������ ������ ����.
	imshow("dstImage2", dstImage2);
	waitKey();
	//BGR ���� �� ����
	int blue_threshold = 200;
	int green_threshold = 200;
	int red_threshold = 200;
	int bgr_threshold[3] = { blue_threshold, green_threshold, red_threshold };
	int nBlue, nGreen, nRed;

	//�Ͼ�� ���� �κ�
	//ūfor���� �Ӱ谪�� �������ִ°�.
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
	
	Mat mask2 = mask.clone();
	mask2 = Scalar(0, 0, 255);
	imshow("mask2",mask2);
	waitKey();
	mask2.copyTo(result,dstImage2);//result�� ����� �Ӱ� �ٲٱ�!
	imshow("dstImage2", dstImage2);
	waitKey();
	imshow("result", result);
	waitKey();
	return 0;
}