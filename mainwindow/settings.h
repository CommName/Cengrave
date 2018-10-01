#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr,int index=0);

    ~Settings();

private slots:
    void on_button_set_machine_controls_clicked();

    void on_button_apply_clicked();

    void on_button_ok_clicked();

    void on_butto_parallel_port_clicked();

    void on_button_general_clicked();

    void on_horizontalSlider_Step_valueChanged(int value);

    void on_spinBox_step_valueChanged(double arg1);

    void on_button_serial_port_clicked();

    void on_pushButton_clicked();

    void on_button_Qt_licence_clicked();

    void on_button_openCv_licence_clicked();

    void on_button_inpout32_licence_clicked();

    void on_button_SerialPort_licence_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::Settings *ui;


private:
    void loadSettings(QString file);
    void saveSettings();

private:
    void detectPorts();
};

#endif // SETTINGS_H
