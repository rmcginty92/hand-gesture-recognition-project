#include <opencv2/objdetect.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay(Mat frame);

/** Global variables */
String fist_cascade_name = "fist.xml";
String palm_cascade_name = "palm.xml";
CascadeClassifier fist_cascade;
CascadeClassifier palm_cascade;
String window_name = "Capture - Face detection";


int main(void)
{
	VideoCapture capture;
	Mat frame;

	//-- 1. Load the cascade
	if (!fist_cascade.load(fist_cascade_name)){ printf("--(!)Error loading fist cascade\n"); return -1; };
	if (!palm_cascade.load(palm_cascade_name)){ printf("--(!)Error loading palm cascade\n"); return -1; };

	//-- 2. Read the video stream
	capture.open(1);
	if (!capture.isOpened()) { printf("--(!)Error opening video capture\n"); return -1; }

	while (capture.read(frame))
	{
		if (frame.empty())
		{
			printf(" --(!) No captured frame -- Break!");
			break;
		}

		//-- 3. Apply the classifier to the frame
		detectAndDisplay(frame);

		//-- bail out if escape was pressed
		int c = waitKey(10);
		if ((char)c == 27) { break; }
	}
	return 0;
}

/**
* @function detectAndDisplay
*/
void detectAndDisplay(Mat frame)
{
	vector<Rect> fists;
	vector<Rect> palms;

	Mat frame_gray;

	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	//-- Detect fists
	fist_cascade.detectMultiScale(frame_gray, fists, 1.1, 2, 0, Size(48, 48));
	//-- In each face, detect palms
	palm_cascade.detectMultiScale(frame_gray, palms, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(48, 64));

	for (size_t i = 0; i < fists.size(); i++)
	{		
		//-- Draw the face
		Point center(fists[i].x + fists[i].width / 2, fists[i].y + fists[i].height / 2);
		
		rectangle(frame, Rect(fists[i].x, fists[i].y, 150, 150), Scalar(0, 0, 255), 2, 1, 0);
	}

	for (size_t j = 0; j < palms.size(); j++)
	{ //-- Draw the palms
		Point palm_center(palms[j].x + palms[j].width / 2, palms[j].y + palms[j].height / 2);
		int radius = cvRound((palms[j].width + palms[j].height)*0.25);
		rectangle(frame, Rect(palms[j].x, palms[j].y, 150, 200), Scalar(0, 255, 0), 2, 1, 0);

	}

	//-- Show what you got
	imshow(window_name, frame);
}

/*
void detectAndDisplay(Mat frame)
{
vector<Rect> faces;
Mat frame_gray;

cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
equalizeHist(frame_gray, frame_gray);

//-- Detect faces
fist_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0, Size(80, 80));

for (size_t i = 0; i < faces.size(); i++)
{
Mat faceROI = frame_gray(faces[i]);
vector<Rect> eyes;

//-- In each face, detect eyes
palm_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
if (eyes.size() == 2)
{
//-- Draw the face
Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
ellipse(frame, center, Size(faces[i].width / 2, faces[i].height / 2), 0, 0, 360, Scalar(255, 0, 0), 2, 8, 0);

for (size_t j = 0; j < eyes.size(); j++)
{ //-- Draw the eyes
Point eye_center(faces[i].x + eyes[j].x + eyes[j].width / 2, faces[i].y + eyes[j].y + eyes[j].height / 2);
int radius = cvRound((eyes[j].width + eyes[j].height)*0.25);
circle(frame, eye_center, radius, Scalar(255, 0, 255), 3, 8, 0);
}
}

}
//-- Show what you got
imshow(window_name, frame);
}

*/