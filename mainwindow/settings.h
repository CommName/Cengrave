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

private:
    Ui::Settings *ui;


private:
    void loadSettings();
    void saveSettings();
};

#endif // SETTINGS_H
