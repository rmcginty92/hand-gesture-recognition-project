#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;
char key;

//Mat hist(Mat image);
void skinMapping();

int main(){	
	namedWindow("Live_Stream",cv::WINDOW_AUTOSIZE);
	namedWindow("Output_Stream",cv::WINDOW_AUTOSIZE);
	VideoCapture cap(0);
	
	while(1){
		//Mat frame = imread("hand1.jpg");		
		Mat frame;		
		cap >> frame;
		
		imshow("Live_Stream",frame);
		//imshow("Output_Stream",hist(frame));
		key = cvWaitKey(10);
		if(char(key) == 27){
			break;
		}
	}
	
	destroyAllWindows();
	
	return 0;
}

void skinMapping(){
	Mat hand1 = imread("hand1.jpg");
	Mat hand2 = imread("hand2.jpg");
	Mat hand3 = imread("hand3.jpg");
	Mat hand4 = imread("hand4.jpg");
	Mat hand5 = imread("hand5.jpg");
	Mat hand6 = imread("hand6.jpg");
	Mat hand7 = imread("hand7.jpg");
	Mat hand8 = imread("hand8.jpg");
	Mat hand9 = imread("hand9.jpg");
	Mat hand10 = imread("hand10.jpg");
	Mat hand11 = imread("hand11.jpg");
	Mat hand12 = imread("hand12.jpg");
	Mat hand13 = imread("hand13.jpg");
	Mat hand14 = imread("hand14.jpg");	
}
/*
Mat hist(Mat image){
	int i,j,r,g,b;
	Mat output(image);
	for(int i = 0;i < image.rows;i++){
		for(int j = 0;j < image.cols;j++){
			output[i*output.step[0]+j*output.step[1]] = 255;
			output[i*output.step[0]+j*output.step[1] + 1] = 0;
			output[i*output.step[0]+j*output.step[1] + 2] = 0;
		}
	}
	return output;
}
*/
