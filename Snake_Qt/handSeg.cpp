#include "handSeg.h"

using namespace cv;

handSeg::handSeg(){
    g_xCentroid = -1;
    g_yCentroid = -1;
    g_topR = -1; g_leftC = -1;

    g_erodingE = getStructuringElement(MORPH_RECT,Size(5,5),Point(-1,-1));
    g_dilatingE = getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
}

Mat handSeg::rgb2ycbcr(Mat &input){
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

Mat handSeg::rgb2hsv(Mat &input){
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

Mat handSeg::handDetect(Mat &input){
    Mat output; output = input;
    //output = Mat::zeros(input.rows, input.cols, CV_8UC3);

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
                *ptrOutput++ = 0; *ptrOutput++ = 0; *ptrOutput++ = 0;
            }

        }
    }
    return output;
}

double handSeg::findCentroid(Mat input){
    uchar *ptr;

    //int sumPix = input.rows*input.cols;

    erode(input,input,g_erodingE);
    dilate(input,input,g_dilatingE);

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

    if (count != 0){
        g_xCentroid = xC / count;
        g_yCentroid = yC / count;
    }
    else{
        g_xCentroid = 0;
        g_yCentroid = 0;
    }

    double rad = .55*sqrt((double)((g_xCentroid - i)*(g_xCentroid - i) + (g_yCentroid - j)*(g_yCentroid - j)));

    int a = g_yCentroid - int(rad);
    int b = g_yCentroid + int(rad);
    int c = g_xCentroid - int(rad);
    int d = g_xCentroid + int(rad);

    g_topR = countHistDif(c, input.cols, 1, input);
    g_leftC = countHistDif(b,input.rows,2,input);

    //int reg = countReg(d, input.cols, 2, input);
    //cout << reg << endl;

    return rad;
}

/*int handSeg::countReg(int p, int q, int type, Mat input){
    int reg = 0;
    if (p > 0 && p < q){
        Vec3b val;
        int prev = -1; int i;
        for (i = 0; i < q; i++){
            if (type == 1){
                val = input.at<Vec3b>(p, i);
            }
            else{
                val = input.at<Vec3b>(i, p);
            }
            if (prev == 0 && val[0] == 255){
                reg++;
            }
            prev = val[0];
        }

    }
    if (reg == 2) {
        return -1;
    } else if(reg == 1) {
        return 1;
    }else {
        return 0;
    }
}
*/

int handSeg::countHistDif(int p,int q, int type, Mat input){
    int numFingers = 0;
    int length;

    if(type == 1){
        length = input.rows;
    }else{
        length = input.cols;
    }

    if (p > 0 && p < length){
        Vec3b val;
        int i;
        int pixelFingerLength = 2;
        int numWhitePixels = 0;

        for (i = 0; i < q; i++){
            if(type == 1){
                val = input.at<Vec3b>(p,i);
            }else{
                val = input.at<Vec3b>(i,p);
            }

            if (val[0] == 255) {
                numWhitePixels++;
            }
            else {
                if (numWhitePixels > pixelFingerLength) {
                    numFingers++;
                }
                numWhitePixels = 0;
            }
        }

    }

    return numFingers;
}

int handSeg::getCommand(){
    int com;
    if(g_topR == 1 && g_leftC == 0){
        com = 0;
        // "W"
    }else if(g_topR == 2 && g_leftC == 0){
        com = 1; // "A"
    }else if(g_topR == 3 && g_leftC == 0){
        com = 2; // "D"
    }else if(g_topR == 0 && g_leftC == 1){
        com = 3; // "S"
    }else if(g_topR > 3 && g_leftC == 1){
        com = 4; // "NOT A COMMAND"
    }else{
        com = 5;
    }
    return com;
}
