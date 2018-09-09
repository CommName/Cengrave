#include "settings.h"
#include "ui_settings.h"
#include <QSettings>
Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    loadSettings();

}

Settings::~Settings()
{
    delete ui;
}

void Settings::on_button_set_machine_controls_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void Settings::loadSettings(){
    QSettings setting("cengrave.ini",QSettings::IniFormat);

    //Reading movement settings
    setting.beginGroup("Movement");

    //Step
    switch(setting.value("Step",1).toInt()){
    case 1: ui->step01mm->setChecked(true); break;
    case 10:ui->step1mm->setChecked(true); break;
    case 100:ui->step10mm->setChecked(true); break;
    case 1000:ui->step100mm->setChecked(true); break;
    default:ui->step01mm->setChecked(true); break;
    }
    //Movement speed
    ui->slider_movement_speed->setValue(setting.value("Movement speed",1).toInt());
    ui->spinBox_movement_speed->setValue(ui->slider_movement_speed->value());
    //Engraving speed
    ui->slider_engrave_speed->setValue(setting.value("Engraving speed",1).toInt());
    ui->spinBox_engrave_speed->setValue(ui->slider_engrave_speed->value());

    setting.endGroup();
}
void Settings::saveSettings(){
    QSettings setting("cengrave.ini",QSettings::IniFormat);

    //Saving movement settings
    setting.beginGroup("Movement");

    //step
    if(ui->step01mm->isChecked()){
        setting.setValue("Step",1);
    }
    else if(ui->step1mm->isChecked()){
        setting.setValue("Step",10);
    }
    else if(ui->step10mm->isChecked()){
        setting.setValue("Step",100);
    }
    else{
        setting.setValue("Step",1000);
    }
    //Movement speed
    setting.setValue("Movement speed",ui->slider_movement_speed->value());
    //Engraving speed
    setting.setValue("Engraving speed",ui->slider_engrave_speed->value());

    setting.endGroup();
}







void Settings::on_button_apply_clicked()
{
    saveSettings();
}

void Settings::on_button_ok_clicked()
{
    saveSettings();
    accept();
}
