#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QString>
#include <QGraphicsItem>
#include "Image2Machine/commandcontainer.h"

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

    void on_modeWidget_currentChanged(int index);

    //mode0
    void on_buttonLoad_mode0_clicked();
    void on_buttonReload_mode0_clicked();
    void on_buttonEngrave_clicked();
    //image transformation
    void on_button_resize_mode0_clicked();
    void on_checkBox_FlipVertical_mode0_clicked();
    void on_checkBox_FlipHorizontal_mode0_clicked();

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

    //mode1
    //engraving modes
    void on_comboBox_engraveMode_currentIndexChanged(int index);
    //threshold
    void on_threshold_slider_valueChanged(int value);
    void on_threshold_invert_checkBox_stateChanged(int arg1);
    //adaptvie threshold
    void on_adaptiveThreshold_C_slider_valueChanged(int value);
    void on_adaptiveThreshold_block_size_slider_valueChanged(int value);
    void on_adaptiveThreshold_invert_checkBox_stateChanged(int arg1);
    void on_adapriveThreshold_MeanC_radiobutton_clicked();
    void on_adapriveThreshold_GaussianC_radiobutton_clicked();

    //extract
    void on_button_extract_test_clicked();
    void on_button_extract_clicked();
    void on_button_load_auto_clicked();

    //Menu
    //Window


private:
    Ui::MainWindow *ui;


//variable
private:
    cv::Mat imageMode0;
    cv::Mat imageMode1;
    cv::Mat imageMode2;
    CommandContainer commands;
    QString imagePath;
    QGraphicsScene *scene0;
    QGraphicsScene *scene1;
    QGraphicsScene *scene2;


//funcions
private:
    void loadImage(QString const &path);
    void displayImageInfo();
    void displayImageMode0();
    void displayImageMode1();
    bool engrave();
    void setEngraveModesInvisible();
    void thresholdMode();
    void adaptiveThreshold();




};    


#endif // MAINWINDOW_H
