

#ifndef COMMANDPREVIEW_H
#define COMMANDPREVIEW_H
#include <QDialog>
#include "Image2Machine/commandcontainer.h"
#include <opencv2/opencv.hpp>
#include <QGraphicsItem>

namespace Ui{
class commandpreview;
}

class commandpreview: public QDialog{
    Q_OBJECT

public:
    explicit commandpreview(QWidget *parent =nullptr,CommandContainer* com=nullptr,int index=0);

    ~commandpreview();

private slots:

    void on_button_next_clicked();

    void on_button_previous_clicked();

    void on_button_set_current_clicked();

    void on_commandList_currentRowChanged(int currentRow);

    void on_button_zoom_in_clicked();

    void on_button_zoom_out_clicked();

    void on_button_zoom_fit_clicked();

    void on_button_zoom_norma_clicked();

private:
    Ui::commandpreview *ui;
    container *pixel;

protected:
    CommandContainer *commands;
    cv::Mat image;
    QGraphicsScene *scene;

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
