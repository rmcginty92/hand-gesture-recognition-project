#include <iostream>
#include <math.h>
#include <ctime>
#include <fstream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

//std::ofstream g_logFile;

Mat rgb2ycbcr(Mat &input){
	Mat output; input.copyTo(output);	

	for (int i = 0; i < input.rows; i++){		
		uchar *ptr = input.ptr<uchar>(i);
		uchar *ptO = output.ptr<uchar>(i);
		for (int j = 0; j < input.cols; j++){			
			uchar b = *ptr++;
			uchar g = *ptr++;
			uchar r = *ptr++;

			*ptO++ = (uchar)round(16 + 0.257*r + 0.504*g + 0.098*b);
			*ptO++ = (uchar)round(128 - 0.148*r - 0.291*g + 0.439*b);
			*ptO++ = (uchar)round(128 + 0.439*r - 0.368*g - 0.071*b);					
		}
	}
	return output;
}
/*
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
	/*		double b = (double)*ptrI++ / 255;
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
				//*ptrO++ = 60.0 * y;
				*ptrO++ = y / 6.0;
			}
			else if (std::abs(cmax - g) < std::abs(cmax - r) && std::abs(cmax - g) < std::abs(cmax - b)){
				//outV[0] = 60.0 * ((b - r) / del + 2);
				*ptrO++ = ((b - r) / del + 2)/6.0;
			}
			else if (std::abs(cmax - b) < std::abs(cmax - r) && std::abs(cmax - b) < std::abs(cmax - g)){
				//outV[0] = 60.0 * ((g - r) / del + 4);
				*ptrO++ = ((g - r) / del + 4)/6.0;
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
	/*		*ptrO++ = cmax;
		}
	}
	return output;
}
*/
Mat rgb2hsv(Mat &input){
	
	Mat output = Mat::zeros(input.rows, input.cols, CV_64FC3);
	
	for (int i = 0; i < output.rows; i++){
		for (int j = 0; j < output.cols; j++){
			
			Vec3d val = input.at<Vec3b>(i, j);			
			double r = val[2] / 255;
			double g = val[1] / 255;
			double b = val[0] / 255;
			
			double cmax = std::max(r, std::max(g,b));
			double cmin = std::min(r, std::min(g,b));
			double del = cmax - cmin;
			
			Vec3d outV = output.at<Vec3d>(i, j);
			if (del == 0.0){
				outV[0] = 0.0;
			}
			else if (std::abs(cmax - r) < std::abs(cmax - g) && std::abs(cmax - r) < std::abs(cmax - b)){
				double x = (g - b) / del;
				double y = fmod(x, 6.0);
				if (y < 0){
					y = 6.0 + y;
				}
				outV[0] = y/6.0;
			}
			else if (std::abs(cmax - g) < std::abs(cmax - r) && std::abs(cmax - g) < std::abs(cmax - b)){				
				outV[0] = ((b - r) / del + 2) / 6.0;
			}
			else if (std::abs(cmax - b) < std::abs(cmax - r) && std::abs(cmax - b) < std::abs(cmax - g)){				
				outV[0] = ((g - r) / del + 4) / 6.0;
			}
			
			if (cmax == 0.0){				
				outV[1] = 0.0;
			}
			else if (cmax != 0.0){			
				outV[1] = del / cmax;
			}
			outV[2] = cmax;
			output.at<Vec3d>(i, j) = outV;			
		}
	}
	return output;
}

Mat handDetect(Mat &input){
	Mat output;
	output = Mat::zeros(input.rows, input.cols, CV_8UC3);

	Mat ycc = rgb2ycbcr(input);
	Mat hsv = Mat::zeros(input.rows, input.cols, CV_64FC3);
	hsv = rgb2hsv(input);

	int i, j;
	uchar *ptrRGB, *ptrYCC, *ptrOutput; double *ptrHSV;

	for (i = 0; i < input.rows; i++){
		ptrRGB = input.ptr<uchar>(i);
		ptrYCC = ycc.ptr<uchar>(i);
		ptrHSV = hsv.ptr<double>(i);
		ptrOutput = output.ptr<uchar>(i);
		for (j = 0; j < input.cols; j++){

			int B = *ptrRGB++; int G = *ptrRGB++; int R = *ptrRGB++;
			int Y = *ptrYCC++; int Cb = *ptrYCC++; int Cr = *ptrYCC++;
			double H = *ptrHSV++; double S = *ptrHSV++; double V = *ptrHSV++;		

			bool cond1 = R > 50 && G > 40 && B > 20 && (std::max(R, std::max(G,B)) - std::min(R, std::min(G,B))) > 10 &&
				abs(R - G) >= 10 && R > G && R > B;
			bool cond2 = R > 220 && G > 210 && B > 170 && abs(R - G) < 15 && R > B && G > B;

			if (cond1 || cond2){				
				if (Cb >= 60 && Cb <= 130 && Cr >= 130 && Cr <= 165){
					if (H >= 0 && H <= 50 && S >= 0.1 && S <= 0.9){						
						*ptrOutput++ = 255;
						*ptrOutput++ = 255;
						*ptrOutput++ = 255;
					}
				}
			}
			else{
				*ptrOutput++; *ptrOutput++; *ptrOutput++;
			}

		}
	}

	return output;
}

int countReg(int p,int q, int type, Mat input){
	int reg = 0; 

	if (p > 0 && p < q){		
		Vec3b val;
		int prev = -1; int i;
		//g_logFile << "[countReg]:: rows = " << input.rows << "; cols = " << input.cols << endl;
		for (i = 0; i < q; i++){
			if (type == 1){
				//g_logFile << "[countReg]:: p = " << p << "; i = " << i << "; q = " << q << endl;
				val = input.at<Vec3b>(p, i);
			}
			else{
				//g_logFile << "[countReg]:: p = " << p << "; i = " << i << "; q = " << q << endl;
				val = input.at<Vec3b>(i, p);
			}

			if (prev == 0 && val[0] == 255){
				reg++;
			}

			prev = val[0];
		}
		
	}
	return reg;
}

double findCentroid(Mat input, int *x, int *y, int *finger){
	
	uchar *ptr;
	int sumPix = input.rows*input.cols;
	
	int i, j, count = 0; int xC = 0, yC = 0;
	int ymax = -1, xmax = -1;
	
	for (i = 0; i < input.rows; i++){
		ptr = input.ptr<uchar>(i);
		for (j = 0; j < input.cols; j++){
			int B = *ptr++; int G = *ptr++; int R = *ptr++;
			if (B == 255){
				xC += i;
				yC += j;
				count++;
				if (j > ymax){
					ymax = j;
					xmax = i;
				}
			}
		}
	}
	
	//g_logFile << "[findCentroid]:: xC = " << xC << " ;yC = " << yC << endl;
	//g_logFile << "[findCentroid]:: xmax = " << xmax << " ;ymax = " << ymax << endl;
	
	if (count != 0){
		*x = xC / count;
		*y = yC / count;
	}
	else{
		*x = 0;
		*y = 0;
	}

	double rad = .55*sqrt((double)((*x - i)*(*x - i) + (*y - j)*(*y - j)));
	
	int a = *y - int(rad);
	int b = *y + int(rad);
	int c = *x - int(rad);
	int d = *x + int(rad);
	//g_logFile << "[findCentroid]:: a = " << a << " ;b = " << b << " ;c = " << c << " ;d = " << d << endl;
	int reg1 = countReg(a, input.rows, 1, input);
	//int reg = countReg(d, input.cols, 2, input);
	//cout << reg << endl;

	*finger = reg1;
	return rad;
}

int main(int argc, char** argv)
{
	/*Comment this if running on Atom processor*/
	/*time_t now = time(0);
	tm localTime;
	localtime_s(&localTime, &now);

	char temp[256], logFileName[256];

	strcpy_s(logFileName, "log_file.");
	_itoa_s(1900 + localTime.tm_year, &temp[0], 10, 10);	strcat_s(logFileName, &temp[0]); strcat_s(logFileName, ".");
	_itoa_s(1 + localTime.tm_mon, &temp[0], 10, 10);	strcat_s(logFileName, &temp[0]); strcat_s(logFileName, ".");
	_itoa_s(localTime.tm_mday, &temp[0], 10, 10);	strcat_s(logFileName, &temp[0]); strcat_s(logFileName, ".");
	_itoa_s(localTime.tm_hour, &temp[0], 10, 10);	strcat_s(logFileName, &temp[0]); strcat_s(logFileName, ".");
	_itoa_s(localTime.tm_min, &temp[0], 10, 10);	strcat_s(logFileName, &temp[0]); strcat_s(logFileName, ".");
	_itoa_s(localTime.tm_sec, &temp[0], 10, 10);	strcat_s(logFileName, &temp[0]); strcat_s(logFileName, ".txt");
	
	g_logFile.open(&logFileName[0], std::ios_base::app);*/
	/******************************************/


	Mat input, subInput;	

	namedWindow("Original", WINDOW_AUTOSIZE);
	namedWindow("Mask", WINDOW_AUTOSIZE);
	
	Scalar colorMask = Scalar(0, 255, 0);
	Scalar colorWin = Scalar(0, 0, 255);
	Mat handDetectionOutput;

	VideoCapture vidCap(0);
	if (!vidCap.isOpened()){
		cout << "Unable to initialize camera";
		return -1;
	}
	
	int x, y, finger = -1; double radius;
	String command = "";

	Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
	while (true)
	{
		vidCap >> input; 				
		
		cv::rectangle(input, Rect(220, 130, 220, 200), colorMask, 1, 1, 0);
		Mat subInput(input, Rect(240, 150, 190, 170));
		
		handDetectionOutput = handDetect(subInput);			
		radius = findCentroid(handDetectionOutput, &x, &y, &finger);
		
		rectangle(handDetectionOutput, Rect(y - radius, x - radius, (int)2 * radius, (int)2 * radius), colorWin, 2, 1, 0);
		
		if (finger == 0){
			command = "GO STRAIGHT";
		}
		else if(finger == 1){
			command = "TURN LEFT";
		}
		else if (finger == 2){
			command = "TURN RIGHT";
		}
		else{
			command = "NOT A COMMAND";
		}

		imshow("Original", input);
		imshow("Mask", handDetectionOutput);
		std::cout << command << std::endl;

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			//g_logFile.close();

			break;
		}
	}

	return 0;

}
