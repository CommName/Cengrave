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

    void on_buttonLoad_clicked();

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
