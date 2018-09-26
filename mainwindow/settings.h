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
    explicit Settings(QWidget *parent = nullptr);
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

private:
    Ui::Settings *ui;


private:
    void loadSettings();
    void saveSettings();

private:
    void detectPorts();
};

#endif // SETTINGS_H
