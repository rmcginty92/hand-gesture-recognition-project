#include "window.h"
#include <iostream>

#define WIN_MAX_WIDTH 1400 //ori: 640
#define WIN_MAX_HEIGHT 560 //ori: 480
#define WIN_MIN_WIDTH 480 //ori: 480
#define WIN_MIN_HEIGHT 250 //ori: 250
#define SIDEBAR_WIDTH 160 //ori: 100
#define SIDEBAR_HEIGHT 50
#define BOARD_LIMIT 401//ori:201


using namespace cv;

window::window(QWidget *parent)
{

    /*
     * Game UI setup
     */
    fr = new board(this);
    g_handSeg = new handSeg();
    qpbstart = new QPushButton("START",this);
    qpbhs = new QPushButton("High Scores",this);
    qvb = new QVBoxLayout;
    qhb = new QHBoxLayout;
//    l1 = new QLCDNumber(this);
    l2 = new QLCDNumber(this);
//    l3 = new QLabel("Level:",this);
    l4 = new QLabel("Score:",this);
    this->setFixedSize(WIN_MAX_WIDTH, WIN_MAX_HEIGHT);

    fr->setFixedSize(BOARD_LIMIT,BOARD_LIMIT);
    qpbstart->setFixedHeight(SIDEBAR_HEIGHT);
    qpbstart->setFixedWidth(SIDEBAR_WIDTH);
    qpbstart->setStyleSheet("background-color: yellow;" \
                            "font: bold 16px;" \
                            "border-style: outset;" \
                            "border-width: 2px;" \
                            "border-radius: 10px;" \
                            "border-color: grey");
    qpbhs->setFixedWidth(SIDEBAR_WIDTH);
//    l1->setFixedWidth(SIDEBAR_WIDTH);
    l2->setFixedWidth(SIDEBAR_WIDTH);
//    l3->setFixedWidth(SIDEBAR_WIDTH);
//    l3->setFixedHeight(SIDEBAR_HEIGHT/2);
//    l3->setStyleSheet("font: 16px");
    l4->setFixedWidth(SIDEBAR_WIDTH);
    l4->setFixedHeight(SIDEBAR_HEIGHT/2);
    l4->setStyleSheet("font: 16px");
    qvb->addWidget(qpbstart);
    qvb->addWidget(qpbhs);
//    qvb->addWidget(l3);
//    qvb->addWidget(l1);
    qvb->addWidget(l4);
    qvb->addWidget(l2);
    qhb->addWidget(fr);

    // add gesture cmmand icons
    QPixmap *icon = new QPixmap(":/icons/dir_icon.png");
    QLabel *iLabel = new QLabel("Up", this);
    iLabel->setPixmap(*icon);
    qvb->addWidget(iLabel);
    qhb->addLayout(qvb);

    // add live cam display box
    lT = new QLabel("Test",this);
    lT->setFixedSize(640,480);
    QVBoxLayout *vbT = new QVBoxLayout;
    vbT->addWidget(lT);
    qhb->addLayout(vbT);

    this->setLayout(qhb);

    /*QLabel *lT = new QLabel("Test",this);
    cv::Mat input = cv::imread("/home/ee443/Downloads/qtsnake/hand1.jpg",CV_LOAD_IMAGE_COLOR);
    QImage qI((uchar*)input.data,input.cols,input.rows,input.step,QImage::Format_RGB888);
    if(input.empty()){
        std::cout << "nooo"<< std::endl;
    }

    lT->setFixedSize(640,480);
    lT->setPixmap(QPixmap::fromImage(qI));
    QVBoxLayout *vbT = new QVBoxLayout;
    vbT->addWidget(lT);
    qhb->addLayout(vbT);*/
    g_wnd = Rect((int)640/8,(int)480/3,220,200);
    g_vidCap.open(0);

    g_timer = new QTimer(this);



    connect(qpbstart,SIGNAL(clicked()),fr,SLOT(start()));
    connect(qpbhs,SIGNAL(clicked()),this,SLOT(viewhighscores()));
    connect(g_timer,SIGNAL(timeout()),this,SLOT(display()));
    g_timer->start(100);
}

void window::test(int i,int j){
    l1->display(QString::number(i));
    l2->display(QString::number(j));
}
void window::end(){
    this->close();
}
void window::viewhighscores(){
    hs = new highscores();
    hs->show();
}
void window::display(){
    g_vidCap.read(g_input);

    //Processing Finger Count
    rectangle(g_input, g_wnd, Scalar(0,255,0), 1, 1, 0);
    Mat subInput(g_input,g_wnd);

    g_maskOutput = g_handSeg->handDetect(subInput);

    double radius = g_handSeg->findCentroid(g_maskOutput);
    rectangle(g_maskOutput, Rect(g_handSeg->getYCen() - radius, g_handSeg->getXCen() - radius, (int)2 * radius, (int)2 * radius), Scalar(255,0,0), 2, 1, 0);
    g_maskOutput.copyTo(g_input(g_wnd));

    //Flipping video frame (easier to respond to)
    flip(g_input,g_input, 1);
    cvtColor(g_input,g_input,CV_BGR2RGB);

    //Converting Mat image into qInput and displaying in qt gui window
    QImage qInput((uchar*)g_input.data,g_input.cols,g_input.rows,g_input.step,QImage::Format_RGB888);
    lT->setPixmap(QPixmap::fromImage(qInput));

    //Make 1 movement based on valid gesture command
    if(g_handSeg->getCommand() > -1 && g_handSeg->getCommand() < 5){
        fr->setCommand(g_handSeg->getCommand());
    }
}

