

#ifndef COMMANDPREVIEW_H
#define COMMANDPREVIEW_H
#include <QDialog>
#include "Image2Machine/commandcontainer.h"
#include <opencv2/opencv.hpp>

namespace Ui{
class commandpreview;
}

class commandpreview: public QDialog{
    Q_OBJECT

public:
    explicit commandpreview(QWidget *parent =nullptr,CommandContainer* com=nullptr,int index=0);

    ~commandpreview();

private slots:

private:
    Ui::commandpreview *ui;
    container *pixel;

protected:
    CommandContainer *commands;
    cv::Mat image;

private:
    container* find(int index);


protected:
    void displayInfo();
    void displayImage();

    void setCurrent();
    void next();
    void pervious();


};


#endif // COMMANDPREVIEW_H
