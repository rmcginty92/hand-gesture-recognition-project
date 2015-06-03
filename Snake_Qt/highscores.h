#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <QtGui>
#include <QVector>
#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

class highscores : public QWidget
{
    Q_OBJECT
public:
    highscores();
    QLabel *qltext,*qlscore;
    QHBoxLayout *qhb;

private:
    QVector<QVector<QString> > qvscores;
};

#endif // HIGHSCORES_H
