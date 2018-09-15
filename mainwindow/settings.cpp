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
    //end of movement settings



    //Reading parallel port settings
    setting.beginGroup("Parallel port");

    //Reading Laser (motor) settings
    ui->lineEdit_motor_port->setText(setting.value("motor_port","0x37a").toString());
    ui->lineEdit_motor_bit->setText(setting.value("motor_bit","0x02").toString());


    //reading x setting
    ui->lineEdit_x_dir_bit->setText(setting.value("x_dir_bit","0x378").toString());
    ui->lineEdit_x_dir_logic->setText(setting.value("x_dir_logic","1").toString());
    ui->lineEdit_x_dir_port->setText(setting.value("x_dir_port","0x378").toString());
    ui->lineEdit_x_enable_bit->setText(setting.value("x_enable_bit","0x02").toString());
    ui->lineEdit_x_enable_port->setText(setting.value("x_enable_port","0x378").toString());
    ui->lineEdit_x_limit_max_active->setText(setting.value("x_limit_max_active","1").toString());
    ui->lineEdit_x_limit_max_bit->setText(setting.value("x_limit_max_bit","0x40").toString());
    ui->lineEdit_x_limit_max_port->setText(setting.value("x_limit_max_port","0x379").toString());
    ui->lineEdit_x_limit_min_active->setText(setting.value("x_limit_min_active","1").toString());
    ui->lineEdit_x_limit_min_bit->setText(setting.value("x_limit_min_bit","0x02").toString());
    ui->lineEdit_x_limit_min_port->setText(setting.value("x_limit_min_port","0x379").toString());
    ui->lineEdit_x_step_bit->setText(setting.value("x_step_bit","0x80").toString());
    ui->lineEdit_x_step_port->setText(setting.value("x_step_port","0x378").toString());

    //reading y setting
    ui->lineEdit_y_dir_bit->setText(setting.value("y_dir_bit","0x10").toString());
    ui->lineEdit_y_dir_logic->setText(setting.value("y_dir_logic","1").toString());
    ui->lineEdit_y_dir_port->setText(setting.value("y_dir_port","0x378").toString());
    ui->lineEdit_y_enable_bit->setText(setting.value("y_enable_bit","0x01").toString());
    ui->lineEdit_y_enable_port->setText(setting.value("y_enable_port","0x378").toString());
    ui->lineEdit_y_limit_max_active->setText(setting.value("y_limit_max_active","1").toString());
    ui->lineEdit_y_limit_max_bit->setText(setting.value("y_limit_max_bit","0x40").toString());
    ui->lineEdit_y_limit_max_port->setText(setting.value("y_limit_max_port","0x379").toString());
    ui->lineEdit_y_limit_min_active->setText(setting.value("y_limit_min_active","1").toString());
    ui->lineEdit_y_limit_min_bit->setText(setting.value("y_limit_min_bit","0x06").toString());
    ui->lineEdit_y_limit_min_port->setText(setting.value("y_limit_min_port","0x379").toString());
    ui->lineEdit_y_step_bit->setText(setting.value("y_step_bit","0x20").toString());
    ui->lineEdit_y_step_port->setText(setting.value("y_step_port","0x378").toString());


    setting.endGroup();
    //end of parallel port settings

}
void Settings::saveSettings(){
    QSettings setting("cengrave.ini",QSettings::IniFormat);

    //Movement settings
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
    //end of movement settings


    //Parallel port settings
    setting.beginGroup("Parallel port");

    //Laser (motor)port
    setting.setValue("motor_port",ui->lineEdit_motor_port->text());
    setting.setValue("motor_bit",ui->lineEdit_motor_bit->text());


    //saving x settings
    setting.setValue("x_dir_bit",ui->lineEdit_x_dir_bit->text());
    setting.setValue("x_dir_logic",ui->lineEdit_x_dir_logic->text());
    setting.setValue("x_dir_port",ui->lineEdit_x_dir_port->text());
    setting.setValue("x_enable_bit",ui->lineEdit_x_enable_bit->text());
    setting.setValue("x_enable_port",ui->lineEdit_x_enable_port->text());
    setting.setValue("x_limit_max_active",ui->lineEdit_x_limit_max_active->text());
    setting.setValue("x_limit_max_bit",ui->lineEdit_x_limit_max_bit->text());
    setting.setValue("x_limit_max_port",ui->lineEdit_x_limit_max_port->text());
    setting.setValue("x_limit_min_active",ui->lineEdit_x_limit_min_active->text());
    setting.setValue("x_limit_min_bit",ui->lineEdit_x_limit_min_bit->text());
    setting.setValue("x_limit_min_port",ui->lineEdit_x_limit_min_port->text());
    setting.setValue("x_step_bit",ui->lineEdit_x_step_bit->text());
    setting.setValue("x_step_port",ui->lineEdit_x_step_port->text());

    //saving y settings
    setting.setValue("y_dir_bit",ui->lineEdit_y_dir_bit->text());
    setting.setValue("y_dir_logic",ui->lineEdit_y_dir_logic->text());
    setting.setValue("y_dir_port",ui->lineEdit_y_dir_port->text());
    setting.setValue("y_enable_bit",ui->lineEdit_y_enable_bit->text());
    setting.setValue("y_enable_port",ui->lineEdit_y_enable_port->text());
    setting.setValue("y_limit_max_active",ui->lineEdit_y_limit_max_active->text());
    setting.setValue("y_limit_max_bit",ui->lineEdit_y_limit_max_bit->text());
    setting.setValue("y_limit_max_port",ui->lineEdit_y_limit_max_port->text());
    setting.setValue("y_limit_min_active",ui->lineEdit_y_limit_min_active->text());
    setting.setValue("y_limit_min_bit",ui->lineEdit_y_limit_min_bit->text());
    setting.setValue("y_limit_min_port",ui->lineEdit_y_limit_min_port->text());
    setting.setValue("y_step_bit",ui->lineEdit_y_step_bit->text());
    setting.setValue("y_step_port",ui->lineEdit_y_step_port->text());


    setting.endGroup();
    //end of parallel port settings
}




//Switching tabs
void Settings::on_button_general_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void Settings::on_button_set_machine_controls_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
void Settings::on_butto_parallel_port_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}



//End buttons
void Settings::on_button_apply_clicked()
{
    saveSettings();
}
void Settings::on_button_ok_clicked()
{
    saveSettings();
    accept();
}


