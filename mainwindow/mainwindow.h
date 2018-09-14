#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QString>
#include <QGraphicsItem>
#include "Image2Machine/commandcontainer.h"

#include "Image2Machine/hwf.h"

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
    void on_button_start_auto_clicked();

    void on_button_clear_console_clicked();

    void on_actionSettings_triggered();

    void on_button_clear_image_clicked();

    void on_button_set_coordinates_clicked();

    void on_movement_upleft_clicked();

    void on_movement_up_clicked();

    void on_movement_upright_clicked();

    void on_movement_left_clicked();

    void on_movement_right_clicked();

    void on_laser_on_off_clicked();

    void on_movement_downleft_clicked();

    void on_movement_down_clicked();

    void on_movement_downright_clicked();

    void on_button_stop_auto_clicked();

    void on_button_continue_auto_clicked();

    void on_check_simulation_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;


//variable
private:
    cv::Mat imageMode0;
    cv::Mat imageMode1;
    cv::Mat imageMode2;
    CommandContainer commands;
    HWF hwf;
    QString imagePath;
    QGraphicsScene *scene0;
    QGraphicsScene *scene1;
    QGraphicsScene *scene2;

    int x_current_position;
    int y_current_position;
    bool laserON;
    bool stop;
    //Settings


//funcions
private:
    void loadImage(QString const &path);
    void displayImageInfo();

    bool engrave();
    void setEngraveModesInvisible();
    void thresholdMode();
    void adaptiveThreshold();

    void loadSettings();
    void saveSettings();

    void execute();

public:
    void displayImageMode0();
    void displayImageMode1();
    void displayImageMode2();



};    


#endif // MAINWINDOW_H
