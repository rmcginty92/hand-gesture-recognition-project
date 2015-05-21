#include "opencv2\highgui\highgui.hpp"
#include "opencv2\opencv.hpp"
#include <iostream>
#include <stdlib.h>

using namespace cv;

double *g_RGBs[225][225];
double *g_RGBn[225][225];
double g_skin = 0; double g_nskin = 0;
Mat g_output;

Mat colorSpace(Mat image, int val){
	Mat output;
	image.copyTo(output);
	cvtColor(image, output, val);
	/*for (int i = 0; i < output.rows; i++){
	for (int j = 0; j < output.cols; j++){
	output.data[output.step[0] * i + output.step[1] * j + 0] = 255;
	output.data[output.step[0] * i + output.step[1] * j + 1] = 0;
	output.data[output.step[0] * i + output.step[1] * j + 2] = 0;
	}
	}*/
	return output;
}

Mat rgb2hsv(Mat &input){

	Mat output = Mat::zeros(input.rows, input.cols, CV_64FC3);

	int i, j;
	uchar *ptrI; double *ptrO;
	for (i = 0; i < output.rows; i++){
		ptrI = input.ptr<uchar>(i);
		ptrO = output.ptr<double>(i);
		for (j = 0; j < output.cols; j++){

			/*Vec3d val = input.at<Vec3b>(i, j);
			double r = val[2] / 255;
			double g = val[1] / 255;
			double b = val[0] / 255;*/
			double b = (double)*ptrI++ / 255;
			double g = (double)*ptrI++ / 255;
			double r = (double)*ptrI++ / 255;

			double cmax = std::max({ r, g, b });
			double cmin = std::min({ r, g, b });
			double del = cmax - cmin;

			//Vec3d outV = output.at<Vec3d>(i, j);
			if (del == 0.0){
				//outV[0] = 0.0;
				*ptrO++ = 0.0;
			}
			else if (std::abs(cmax - r) < std::abs(cmax - g) && std::abs(cmax - r) < std::abs(cmax - b)){
				double x = (g - b) / del;
				double y = fmod(x, 6.0);
				if (y < 0){
					y = 6.0 + y;
				}
				//outV[0] = 60.0 * y;
				*ptrO++ = 60.0 * y;
			}
			else if (std::abs(cmax - g) < std::abs(cmax - r) && std::abs(cmax - g) < std::abs(cmax - b)){
				//outV[0] = 60.0 * ((b - r) / del + 2);
				*ptrO++ = 60.0 * ((b - r) / del + 2);
			}
			else if (std::abs(cmax - b) < std::abs(cmax - r) && std::abs(cmax - b) < std::abs(cmax - g)){
				//outV[0] = 60.0 * ((g - r) / del + 4);
				*ptrO++ = 60.0 * ((g - r)) / del + 4;
			}

			if (cmax == 0.0){
				//outV[1] = 0.0;
				*ptrO++ = 0.0;
			}
			else if (cmax != 0.0){
				//outV[1] = del / cmax;
				*ptrO++ = del / cmax;
			}
			/*outV[2] = cmax;
			output.at<Vec3d>(i, j) = outV;*/
			*ptrO++ = cmax;
		}
	}
	return output;
}

Mat rgb2ycbcr(Mat &input){
	Mat output; input.copyTo(output);

	uchar *ptr, *ptO;
	char b, g, r;
	int i, j;

	for (i = 0; i < input.rows; i++){
		ptr = input.ptr<uchar>(i);
		ptO = output.ptr<uchar>(i);
		for (j = 0; j < input.cols; j++){
			b = *ptr++;
			g = *ptr++;
			r = *ptr++;

			*ptO++ = (uchar)round(16 + 0.257*r + 0.504*g + 0.098*b);
			*ptO++ = (uchar)round(128 - 0.148*r - 0.291*g + 0.439*b);
			*ptO++ = (uchar)round(128 + 0.439*r - 0.368*g - 0.071*b);
		}
	}
	return output;
}

void loadData(){
	Mat hand1 = imread("hand1.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand2 = imread("hand2.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand3 = imread("hand3.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand4 = imread("hand4.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand5 = imread("hand5.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand6 = imread("hand6.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand7 = imread("hand7.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand8 = imread("hand8.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand9 = imread("hand9.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand10 = imread("hand10.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand11 = imread("hand11.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand12 = imread("hand12.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand13 = imread("hand13.jpg", CV_LOAD_IMAGE_COLOR);
	Mat hand14 = imread("hand14.jpg", CV_LOAD_IMAGE_COLOR);

	std::vector<Mat> dataset = { hand1, hand2, hand3, hand4, hand5, hand6, hand7, hand8, hand9,
		hand10, hand11, hand12, hand13, hand14 };
	hand1.copyTo(g_output);


	for (int k = 0; k < 14; k++){
		Mat img = dataset.at(k);
		Mat ycc = rgb2ycbcr(img);
		Mat hsv = Mat::zeros(img.rows, img.cols, CV_64FC3);
		hsv = rgb2hsv(img);

		for (int i = 0; i < img.rows; i++){
			for (int j = 0; j < img.cols; j++){

				Vec3b currRGB = img.at<Vec3b>(i, j);
				Vec3b currYCC = ycc.at<Vec3b>(i, j);
				Vec3d currHSV = hsv.at<Vec3d>(i, j);

				bool cond1 = currRGB[2] > 50 && currRGB[1] > 40 && currRGB[0] > 20 &&
					(std::max({ currRGB[0], currRGB[1], currRGB[2] }) - std::min({ currRGB[0], currRGB[1], currRGB[2] })) > 10 &&
					abs(currRGB[2] - currRGB[1]) >= 10 && currRGB[2] > currRGB[1] && currRGB[2] > currRGB[0];

				bool cond2 = currRGB[2] > 220 && currRGB[1] > 210 && currRGB[0] > 170 &&
					abs(currRGB[2] - currRGB[1]) < 15 && currRGB[2] > currRGB[0] && currRGB[1] > currRGB[0];

				if (cond1 || cond2){
					if (currYCC[1] >= 60 && currYCC[1] <= 130 && currYCC[2] >= 130 && currYCC[2] <= 165){
						if (currHSV[1] >= 0 && currHSV[2] <= 50 && currHSV[2] >= 0.1 && currHSV[2] <= 0.9){
							if (k == 1){
								g_output.data[g_output.step[0] * i + g_output.step[1] * j + 2] = 255;
								g_output.data[g_output.step[0] * i + g_output.step[1] * j] = 0;
								g_output.data[g_output.step[0] * i + g_output.step[1] * j + 1] = 0;
							}
							int Cb = currYCC[1];
							int Cr = currYCC[2];
							if (g_RGBs[Cb][Cr] == NULL){
								g_RGBs[Cb][Cr] = new double(0);
							}
							else{
								*g_RGBs[Cb][Cr] += 1;
							}
							g_skin += 1;
						}
					}
				}
				else{
					int Cb = currYCC[1];
					int Cr = currYCC[2];
					if (g_RGBn[Cb][Cr] == NULL){
						g_RGBn[Cb][Cr] = new double(0);
					}
					else{
						*g_RGBn[Cb][Cr] += 1;
					}
					g_nskin += 1;
				}
			}
		}
	}


	for (int i = 0; i < 225; i++){
		for (int j = 0; j < 225; j++){
			if (g_RGBs[i][j] == NULL){
				g_RGBs[i][j] = new double(0);
			}
			else{
				*g_RGBs[i][j] = (double)(*g_RGBs[i][j]) / (double)g_skin;
			}
			if (g_RGBn[i][j] == NULL){
				g_RGBn[i][j] = new double(0);
			}
			else{
				*g_RGBn[i][j] = (double)(*g_RGBn[i][j]) / (double)g_skin;
			}
		}
	}
}

Mat skinDetection(Mat &input){
	Mat output; input.copyTo(output);

	double total = g_skin + g_nskin;
	double pSkin = g_skin / total;
	double pNskin = g_nskin / total;

	Mat ycc = rgb2ycbcr(input);

	int i, j;
	uchar *ptr, *ptrycc;
	for (i = 0; i < input.rows; i++){
		ptr = output.ptr<uchar>(i);
		ptrycc = ycc.ptr<uchar>(i);
		for (j = 0; j < input.cols; j++){
			*ptrycc++;
			int Cb = *ptrycc++;
			int Cr = *ptrycc++;

			if (Cb >= 225){
				Cb = 224;
			}
			if (Cr >= 225){
				Cr = 224;
			}

			double pscRGB = *g_RGBs[Cb][Cr] * pSkin / (*g_RGBs[Cb][Cr] * pSkin + *g_RGBn[Cb][Cr] * pNskin);

			if (pscRGB >= 0.4){
				*ptr++ = 0;
				*ptr++ = 0;
				*ptr++ = 255;
			}
			else{
				*ptr++; *ptr++; *ptr++;
			}
		}
	}
	return output;
}

int main(){

	namedWindow("Live_Stream", WINDOW_AUTOSIZE);
	namedWindow("Output_Stream", WINDOW_AUTOSIZE);

	VideoCapture cap(0);
	Mat frame;
	Mat hand1 = imread("hand1.jpg", CV_LOAD_IMAGE_COLOR);



	loadData();
	Mat output;
	while (1){
		cap >> frame;
		output = skinDetection(frame);
		imshow("Live_Stream", frame);
		imshow("Output_Stream", output);
		if ((int)waitKey(10) == 27){
			break;
		}
	}
	destroyAllWindows();
}