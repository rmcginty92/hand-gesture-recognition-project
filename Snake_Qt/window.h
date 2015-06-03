#ifndef WINDOW_H
#define WINDOW_H

#include<QtGui>
#include "board.h"
#include "highscores.h"
#include "handSeg.h"
#include <QLCDNumber>
#include <QTimer>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class window:public QWidget
{
    Q_OBJECT
public:
    window(QWidget *parent=0);
    board *fr;
    QHBoxLayout *qhb;
    QVBoxLayout *qvb;
    QPushButton *qpbstart,*qpbhs;
    QLabel *l3,*l4; QLabel *lT;
    QLCDNumber *l1,*l2;
    highscores *hs;

    QTimer *g_timer;
    handSeg *g_handSeg;
    cv::VideoCapture g_vidCap;
    cv::Mat g_input, g_maskOutput;
    cv::Rect g_wnd;
    int fr1;
public slots:
    void test(int,int);
    void end();
    void viewhighscores();
    void display();
};

#endif // WINDOW_H
