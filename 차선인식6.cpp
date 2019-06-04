//�����ν�
/*
Canny �˰����� ���� ��� �� ���� ���� ����ؼ� ����(gradient) ���⿡�� 
���� ��� �� ���� �ʿ� �ִ� ���� ���� ���� ������ �����ϸ� ������ �����ϰ�, 
���� ������ �ִ밡 �ƴ� �������� �����Ѵ�. �̷μ� ������ ��� ���� �����ϴ� �� 
���� ǰ���� ������ ������ �� �ִ�.
*/
#include"opencv2/opencv.hpp"
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;

int main() {

	Mat srcImage = imread("solidWhiteCurve.jpg");
	Mat dstImage = srcImage.clone();
	Mat dstImage2 = srcImage.clone();
	Mat result = srcImage.clone();
	Mat mask(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(255, 255, 255));//8bit Unsigned Char 3ä��
	int height = srcImage.size().height;
	int width = srcImage.size().width;

	

	Mat edge;
	Canny(dstImage, edge, 130, 210, 3);
	//�Է¿���, �������, ���� ��谪, ���� ��谪
	threshold(edge, edge, 255, 0, CV_THRESH_MASK);//sobel����� ���Ϸ��� ����
	imshow("Canny Image", edge);
	waitKey();

	Mat sobelX, sobelY, sobel;
	Sobel(dstImage, sobelX, CV_16S, 1, 0);
	Sobel(dstImage, sobelY, CV_16S, 0, 1);
	sobel = abs(sobelX) + abs(sobelY);//L1 ��(norm)���

	double sobmin, sobmax;
	minMaxLoc(sobel, &sobmin, &sobmax);//sobel �ִ� ã��

	Mat sobelIamge;
	sobel.convertTo(sobelIamge, CV_8U, -255. / sobmax, 255);//8bit�������� ��ȯ
	imshow("Sobel Image", sobelIamge);

	//sobel ���� ��谪
	Mat sobelThresholded;
	threshold(sobelIamge, sobelThresholded, 210, 255, THRESH_BINARY_INV);
	imshow("sobel ���� ��� ��", sobelThresholded);
	//sobel ���� ��� ��
	threshold(sobelIamge, sobelThresholded, 130, 255, THRESH_BINARY_INV);
	imshow("sobel ���� ��� ��", sobelThresholded);
	waitKey();

	return 0;
}
//Point pts[4] = { Point(0,height),Point(width / 8 - 45, height / 2 + 60), Point(width / 8 * 7 + 45, height / 2 + 60), Point(width,height) };
	//��ٸ��� ���� �����ֱ�
	//���� ������ (0,0)

	//fillConvexPoly(mask, pts, 4, Scalar(255, 255, 255));
	//������ ������ִ� �Լ� ����.
	//�̹���,����,����,������� ä����.
	/*imshow("mask", mask);
	imshow("srcImage", srcImage);
	waitKey();
	bitwise_and(dstImage, mask, dstImage2);//��ٸ��� ������ ������ ����.
	imshow("dstImage2", dstImage2);
	waitKey();*/
/*
int main()
{
	Mat srcImage = imread("solidWhiteCurve.jpg",IMREAD_GRAYSCALE);
	resize(srcImage, srcImage, Size(), 0.6, 0.6);
	imshow("Original Image", srcImage);

	Mat edge;
	Canny(srcImage, edge, 130, 210, 3);
	threshold(edge, edge, 0, 255, CV_THRESH_BINARY_INV);//sobel����� ���Ϸ��� ����
	imshow("Canny Image", edge);

	Mat sobelX, sobelY, sobel;
	Sobel(srcImage, sobelX, CV_16S, 1, 0);
	Sobel(srcImage, sobelY, CV_16S, 0, 1);
	sobel = abs(sobelX) + abs(sobelY);//L1 ��(norm)���

	double sobmin, sobmax;
	minMaxLoc(sobel, &sobmin, &sobmax);//sobel �ִ� ã��

	Mat sobelIamge;
	sobel.convertTo(sobelIamge, CV_8U, -255. / sobmax, 255);//8bit�������� ��ȯ
	imshow("Sobel Image", sobelIamge);

	//sobel ���� ��谪
	Mat sobelThresholded;
	threshold(sobelIamge, sobelThresholded, 210, 255, THRESH_BINARY);
	imshow("sobel ���� ��� ��", sobelThresholded);
	//sobel ���� ��� ��
	threshold(sobelIamge, sobelThresholded, 130, 255, THRESH_BINARY);
	imshow("sobel ���� ��� ��", sobelThresholded);
	waitKey();
	return 0;

}*/


//���, HSV�� �޺��� ���� �ٸ�.
//������ �����ϰ�� �ν����� ���� //������ �ּ�ȭ
//��� �ִ��� �Ⱦ���
//������ ��� ���ص� ��.