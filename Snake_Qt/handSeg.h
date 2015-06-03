#ifndef HANDSEG_H
#define HANDSEG_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class handSeg{
public:
    handSeg();
    cv::Mat rgb2ycbcr(cv::Mat &input);
    cv::Mat rgb2hsv(cv::Mat &input);
    cv::Mat handDetect(cv::Mat &input);
    double findCentroid(cv::Mat input);
    int countHistDif(int p,int q,int type,cv::Mat input);
    int getYCen(){return g_yCentroid;}
    int getXCen(){return g_xCentroid;}
    int getCommand();

private:
    cv::Mat g_input;
    cv::Mat g_erodingE;
    cv::Mat g_dilatingE;

    int g_xCentroid;
    int g_yCentroid;

    int g_topR;
    int g_leftC;
};

#endif // HANDSEG_H
