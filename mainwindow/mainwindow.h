#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <QString>
#include <QGraphicsItem>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    //mode0

    void on_buttonLoad_clicked();

    //zoom mode0
    void on_button_mode0_zoom_in_clicked();
    void on_button_mode0_zoom_out_clicked();
    void on_button_mode0_zoom_fit_clicked();
    void on_button_mode0_zoom_normal_clicked();
    //zoom mode1
    void on_button_mode1_zoom_in_clicked();
    void on_button_mode1_zoom_out_clicked();
    void on_button_mode1_zoom_fit_clicked();
    void on_button_mode1_zoom_normal_clicked();
    //zoom mode2
    void on_button_mode2_zoom_in_clicked();
    void on_button_mode2_zoom_out_clicked();
    void on_button_mode2_zoom_fit_clicked();
    void on_button_mode2_zoom_normal_clicked();

private:
    Ui::MainWindow *ui;


//variable
private:
    cv::Mat imageMode0;
    cv::Mat imageMode1;
    cv::Mat imageMode2;
    QString imagePath;
    QGraphicsScene *scene0;
    QGraphicsScene *scene1;
    QGraphicsScene *scene2;


//funcions
private:
    void loadImage();
    void displayImageInfo();
    void displayImageMode0();



};    


#endif // MAINWINDOW_H
