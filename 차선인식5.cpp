#include"opencv2/opencv.hpp"
#include<iostream>
#include<vector>

	using namespace std;
	using namespace cv;

	Mat region_of_interest(Mat srcImage, Mat dstImage, Mat dstImage2);
	Mat mark_img(Mat dstImage2);
	Mat transfer_srcImage(Mat srcImage, Mat image2);

	int main()
	{
		VideoCapture inputVideo	("ssuderive.mp4");//������ü�� �޾ƿ�.
		//Size size = Size((int)inputVideo.get(CAP_PROP_FRAME_WIDTH), (int)inputVideo.get(CAP_PROP_FRAME_HEIGHT));
		Mat srcImage;
		Mat dstImage1;
		Mat dstImage2;
		Mat image1, image2, real_image;

		int fps = (int)(inputVideo.get(CAP_PROP_FPS));//������ �����ִ� ���ۼӵ�. //���ۼӵ��� �������������� ������ �� �� �ִ�.
		int delay = 1000 / fps;

		for (;;)
		{
			inputVideo >> srcImage; // frame�� �о�´�. 
			dstImage1 = srcImage.clone();
			dstImage2 = srcImage.clone();
			image1 = region_of_interest(srcImage, dstImage1, dstImage2);
			image2 = mark_img(dstImage2);
			real_image = transfer_srcImage(srcImage, image2);

			imshow("road", real_image);
			int ckey = waitKey(delay);
			if (ckey == 27) break;
		}
		return 0;
	}

	Mat region_of_interest(Mat srcImage, Mat dstImage, Mat dstImage2) // �����̹����� �ٰ��� �κ� ����
	{
		int height = srcImage.size().height;
		int width = srcImage.size().width;

		Mat mask(srcImage.rows, srcImage.cols, CV_8UC3, Scalar(0, 0, 0));
		Point pts[4] = { Point(0,height),Point(0, height / 2 + 60), Point(width, height / 2 + 60), Point(width,height) };
		fillConvexPoly(mask, pts, 4, Scalar(255, 255, 255));
		bitwise_and(dstImage, mask, dstImage2);

		return dstImage.clone();
	}

	Mat mark_img(Mat dstImage2) // ���� �̹������� �Ͼ�� �κ��� ������ �������� �������� �ٲٰ�, ���������� ĥ�ϱ�.
	{
		//BGR ���� �� ����
		int blue_threshold = 200;
		int green_threshold = 200;
		int red_threshold = 200;
		int nBlue, nGreen, nRed;

		for (int j = 0; j < dstImage2.rows; j++)
		{
			for (int i = 0; i < dstImage2.cols; i++)
			{
				nBlue = dstImage2.at<Vec3b>(j, i)[0]; // Blue
				nGreen = dstImage2.at<Vec3b>(j, i)[1]; // Green
				nRed = dstImage2.at<Vec3b>(j, i)[2]; // Red

				if (nBlue < blue_threshold || nGreen < green_threshold || nRed < red_threshold)
				{
					dstImage2.at<Vec3b>(j, i)[0] = 0;
					dstImage2.at<Vec3b>(j, i)[1] = 0;
					dstImage2.at<Vec3b>(j, i)[2] = 0;
				}
				else
				{
					dstImage2.at<Vec3b>(j, i)[0] = 0;
					dstImage2.at<Vec3b>(j, i)[1] = 0;
					dstImage2.at<Vec3b>(j, i)[2] = 255;
				}
			}
		}
		return dstImage2.clone();
	}

	Mat transfer_srcImage(Mat srcImage, Mat image2) // �����̹����� ������ �� ���.
	{
		for (int j = 0; j < srcImage.rows; j++)
		{
			for (int i = 0; i < srcImage.cols; i++)
			{
				if (image2.at<Vec3b>(j, i)[0] == 0 && image2.at<Vec3b>(j, i)[1] == 0 && image2.at<Vec3b>(j, i)[2] == 255)
				{
					srcImage.at<Vec3b>(j, i)[0] = 0;
					srcImage.at<Vec3b>(j, i)[1] = 0;
					srcImage.at<Vec3b>(j, i)[2] = 255;
				}
			}
		}
		return srcImage.clone();
	}
