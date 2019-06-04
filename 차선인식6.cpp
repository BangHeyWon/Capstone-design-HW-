//엣지인식
/*
Canny 알고리즘은 높은 경계 값 에지 맵을 사용해서 기울기(gradient) 방향에서 
낮은 경계 값 에지 맵에 있는 에지 점이 나올 때까지 추적하며 에지를 연결하고, 
기울기 강도가 최대가 아닌 에지점은 제거한다. 이로서 적절한 경계 값을 지정하는 한 
좋은 품질의 에지를 추출할 수 있다.
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
	Mat mask(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(255, 255, 255));//8bit Unsigned Char 3채널
	int height = srcImage.size().height;
	int width = srcImage.size().width;

	

	Mat edge;
	Canny(dstImage, edge, 130, 210, 3);
	//입력영상, 결과영상, 낮은 경계값, 높은 경계값
	threshold(edge, edge, 255, 0, CV_THRESH_MASK);//sobel영상과 비교하려고 반전
	imshow("Canny Image", edge);
	waitKey();

	Mat sobelX, sobelY, sobel;
	Sobel(dstImage, sobelX, CV_16S, 1, 0);
	Sobel(dstImage, sobelY, CV_16S, 0, 1);
	sobel = abs(sobelX) + abs(sobelY);//L1 놈(norm)계산

	double sobmin, sobmax;
	minMaxLoc(sobel, &sobmin, &sobmax);//sobel 최댓값 찾기

	Mat sobelIamge;
	sobel.convertTo(sobelIamge, CV_8U, -255. / sobmax, 255);//8bit영상으로 전환
	imshow("Sobel Image", sobelIamge);

	//sobel 낮은 경계값
	Mat sobelThresholded;
	threshold(sobelIamge, sobelThresholded, 210, 255, THRESH_BINARY_INV);
	imshow("sobel 낮은 경계 값", sobelThresholded);
	//sobel 높은 경계 값
	threshold(sobelIamge, sobelThresholded, 130, 255, THRESH_BINARY_INV);
	imshow("sobel 높은 경계 값", sobelThresholded);
	waitKey();

	return 0;
}
//Point pts[4] = { Point(0,height),Point(width / 8 - 45, height / 2 + 60), Point(width / 8 * 7 + 45, height / 2 + 60), Point(width,height) };
	//사다리꼴 범위 정해주기
	//왼쪽 위에가 (0,0)

	//fillConvexPoly(mask, pts, 4, Scalar(255, 255, 255));
	//모형을 만들어주는 함수 같음.
	//이미지,범위,개수,흰색으로 채워라.
	/*imshow("mask", mask);
	imshow("srcImage", srcImage);
	waitKey();
	bitwise_and(dstImage, mask, dstImage2);//사다리꼴 범위만 나오게 반전.
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
	threshold(edge, edge, 0, 255, CV_THRESH_BINARY_INV);//sobel영상과 비교하려고 반전
	imshow("Canny Image", edge);

	Mat sobelX, sobelY, sobel;
	Sobel(srcImage, sobelX, CV_16S, 1, 0);
	Sobel(srcImage, sobelY, CV_16S, 0, 1);
	sobel = abs(sobelX) + abs(sobelY);//L1 놈(norm)계산

	double sobmin, sobmax;
	minMaxLoc(sobel, &sobmin, &sobmax);//sobel 최댓값 찾기

	Mat sobelIamge;
	sobel.convertTo(sobelIamge, CV_8U, -255. / sobmax, 255);//8bit영상으로 전환
	imshow("Sobel Image", sobelIamge);

	//sobel 낮은 경계값
	Mat sobelThresholded;
	threshold(sobelIamge, sobelThresholded, 210, 255, THRESH_BINARY);
	imshow("sobel 낮은 경계 값", sobelThresholded);
	//sobel 높은 경계 값
	threshold(sobelIamge, sobelThresholded, 130, 255, THRESH_BINARY);
	imshow("sobel 높은 경계 값", sobelThresholded);
	waitKey();
	return 0;

}*/


//상수, HSV도 햇빛에 따라 다름.
//차선을 제외하고는 인식하지 말기 //노이즈 최소화
//상수 최대한 안쓰기
//다차선 고려 안해도 됨.