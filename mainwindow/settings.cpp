#include "settings.h"
#include "ui_settings.h"
#include <QSettings>
#include <qextserialenumerator.h>
#include <qextserialport.h>
#include <QList>
Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    //General
    ui->comboBox_mode->addItem("Serial port");
    ui->comboBox_mode->addItem("Parallel port");

    //Serial port
    //Baurd
    ui->comboBox_Baud->addItem("110");
    ui->comboBox_Baud->addItem("300");
    ui->comboBox_Baud->addItem("600");
    ui->comboBox_Baud->addItem("1200");
    ui->comboBox_Baud->addItem("2400");
    ui->comboBox_Baud->addItem("4800");
    ui->comboBox_Baud->addItem("9600");
    ui->comboBox_Baud->addItem("19200");
    ui->comboBox_Baud->addItem("38400");
    ui->comboBox_Baud->addItem("115200");
    //ParityType
    ui->comboBox_Parity_type->addItem("None");
    ui->comboBox_Parity_type->addItem("Odd");
    ui->comboBox_Parity_type->addItem("Even");
    ui->comboBox_Parity_type->addItem("Mark");
    ui->comboBox_Parity_type->addItem("Space");
    //Flow type
    ui->comboBox_Flow_Type->addItem("OFF");
    ui->comboBox_Flow_Type->addItem("Hardware");
    ui->comboBox_Flow_Type->addItem("XONXOFF");
    //Stop bits
    ui->comboBox_Stop_bits->addItem("1");
    ui->comboBox_Stop_bits->addItem("1.5");
    ui->comboBox_Stop_bits->addItem("2");
    //Ports
    detectPorts();

    loadSettings();

}

Settings::~Settings()
{
    delete ui;
}

void Settings::loadSettings(){
    QSettings setting("cengrave.ini",QSettings::IniFormat);

   //General
    setting.beginGroup("General");
    ui->comboBox_mode->setCurrentIndex(setting.value("mode",0).toInt());
    setting.endGroup();

    //Reading movement settings
    setting.beginGroup("Movement");

    //Step
    ui->spinBox_step->setValue(setting.value("step",1).toInt()*0.1);
    ui->SpinBox_stepX->setValue(setting.value("step_x",42.0).toDouble());
    ui->SpinBox_stepY->setValue(setting.value("step_y",42.0).toDouble());
    ui->spinBox_maxStrength->setValue(setting.value("max_strength",255).toInt());
    ui->spinBox_minStrebth->setValue(setting.value("min_strength",0).toInt());

    //Movement speed
    ui->slider_movement_speed->setValue(setting.value("movement speed",1).toInt());
    ui->spinBox_movement_speed->setValue(ui->slider_movement_speed->value());
    //Engrave time
    ui->slider_engrave_time->setValue(setting.value("engrave time",1).toInt());
    ui->spinBox_engrave_time->setValue(ui->slider_engrave_time->value());


    setting.endGroup();
    //end of movement settings

    //Serial port
    setting.beginGroup("Serial port");
    ui->comboBox_COM->setCurrentText(setting.value("com_port","COM4").toString());
    ui->comboBox_Baud->setCurrentText(QString::number(setting.value("baud",115200).toInt()));
    ui->spinBox_Data_bits->setValue(setting.value("data_bits",8).toInt());
    ui->comboBox_Parity_type->setCurrentIndex(setting.value("parity_type",0).toInt());
    ui->comboBox_Flow_Type->setCurrentIndex(setting.value("flow_type",0).toInt());
    ui->comboBox_Stop_bits->setCurrentIndex(setting.value("stop_bits",0).toInt());
    ui->spinBox_Timeout->setValue(setting.value("timeout",100).toInt());
    ui->spinBox_xDir->setValue(setting.value("x_dir",0).toInt());
    ui->spinBox_yDir->setValue(setting.value("y_dir",0).toInt());
    ui->lineEdit_laserOn->setText(setting.value("laser_on","M3").toString());
    ui->lineEdit_laserOff->setText(setting.value("laser_off","M5").toString());


    setting.endGroup();
    //end of serial port settings

    //Reading parallel port settings
    setting.beginGroup("Parallel port");

    //Reading Laser (motor) settings
    ui->lineEdit_motor_port->setText(QString::number(setting.value("motor_port",0x37a).toInt(),16));
    ui->lineEdit_motor_bit->setText(QString::number(setting.value("motor_bit",0x02).toInt(),16));
    ui->lineEdit_msec_delay_const->setText(QString::number(setting.value("msec_delay_const",356).toInt()));


    //reading x setting
    ui->lineEdit_x_dir_bit          ->setText(QString::number(setting.value("x_dir_bit",            0x378 ).toInt(),16));
    ui->lineEdit_x_dir_logic        ->setText(QString::number(setting.value("x_dir_logic",          1     ).toInt(),16));
    ui->lineEdit_x_dir_port         ->setText(QString::number(setting.value("x_dir_port",           0x378 ).toInt(),16));
    ui->lineEdit_x_enable_bit       ->setText(QString::number(setting.value("x_enable_bit",         0x02  ).toInt(),16));
    ui->lineEdit_x_enable_port      ->setText(QString::number(setting.value("x_enable_port",        0x378 ).toInt(),16));
    ui->lineEdit_x_limit_max_active ->setText(QString::number(setting.value("x_limit_max_active",   1     ).toInt(),16));
    ui->lineEdit_x_limit_max_bit    ->setText(QString::number(setting.value("x_limit_max_bit",      0x40  ).toInt(),16));
    ui->lineEdit_x_limit_max_port   ->setText(QString::number(setting.value("x_limit_max_port",     0x379 ).toInt(),16));
    ui->lineEdit_x_limit_min_active ->setText(QString::number(setting.value("x_limit_min_active",   1     ).toInt(),16));
    ui->lineEdit_x_limit_min_bit    ->setText(QString::number(setting.value("x_limit_min_bit",      0x02  ).toInt(),16));
    ui->lineEdit_x_limit_min_port   ->setText(QString::number(setting.value("x_limit_min_port",     0x379 ).toInt(),16));
    ui->lineEdit_x_step_bit         ->setText(QString::number(setting.value("x_step_bit",           0x80  ).toInt(),16));
    ui->lineEdit_x_step_port        ->setText(QString::number(setting.value("x_step_port",          0x378 ).toInt(),16));

    //reading y setting
    ui->lineEdit_y_dir_bit          ->setText(QString::number(setting.value("y_dir_bit",            0x10  ).toInt(),16));
    ui->lineEdit_y_dir_logic        ->setText(QString::number(setting.value("y_dir_logic",          1     ).toInt(),16));
    ui->lineEdit_y_dir_port         ->setText(QString::number(setting.value("y_dir_port",           0x378 ).toInt(),16));
    ui->lineEdit_y_enable_bit       ->setText(QString::number(setting.value("y_enable_bit",         0x01  ).toInt(),16));
    ui->lineEdit_y_enable_port      ->setText(QString::number(setting.value("y_enable_port",        0x378 ).toInt(),16));
    ui->lineEdit_y_limit_max_active ->setText(QString::number(setting.value("y_limit_max_active",   1     ).toInt(),16));
    ui->lineEdit_y_limit_max_bit    ->setText(QString::number(setting.value("y_limit_max_bit",      0x40  ).toInt(),16));
    ui->lineEdit_y_limit_max_port   ->setText(QString::number(setting.value("y_limit_max_port",     0x379 ).toInt(),16));
    ui->lineEdit_y_limit_min_active ->setText(QString::number(setting.value("y_limit_min_active",   1     ).toInt(),16));
    ui->lineEdit_y_limit_min_bit    ->setText(QString::number(setting.value("y_limit_min_bit",      0x06  ).toInt(),16));
    ui->lineEdit_y_limit_min_port   ->setText(QString::number(setting.value("y_limit_min_port",     0x379 ).toInt(),16));
    ui->lineEdit_y_step_bit         ->setText(QString::number(setting.value("y_step_bit",           0x20  ).toInt(),16));
    ui->lineEdit_y_step_port        ->setText(QString::number(setting.value("y_step_port",          0x378 ).toInt(),16));


    setting.endGroup();
    //end of parallel port settings

}
void Settings::saveSettings(){
    QSettings setting("cengrave.ini",QSettings::IniFormat);

    //General
     setting.beginGroup("General");
     setting.setValue("mode",ui->comboBox_mode->currentIndex());
     setting.endGroup();

    //Movement settings
    setting.beginGroup("Movement");

    //step

    setting.setValue("step",(int)(ui->spinBox_step->value()*100+1)/10); //because 0.7*100=69

    setting.setValue("step_x",ui->SpinBox_stepX->value());
    setting.setValue("step_y",ui->SpinBox_stepY->value());

    //Movement speed
    setting.setValue("movement speed",ui->slider_movement_speed->value());
    //Engrave time
    setting.setValue("engrave time",ui->slider_engrave_time->value());
    setting.setValue("max_strength",ui->spinBox_maxStrength->value());
    setting.setValue("min_strength",ui->spinBox_minStrebth->value());
    setting.endGroup();
    //end of movement settings

    //Serial port
    setting.beginGroup("Serial port");

    setting.setValue("com_port",ui->comboBox_COM->currentText());
    setting.setValue("baud",ui->comboBox_Baud->currentText().toInt());
    setting.setValue("data_bits",ui->spinBox_Data_bits->value());
    setting.setValue("parity_type",ui->comboBox_Parity_type->currentIndex());
    setting.setValue("flow_type",ui->comboBox_Flow_Type->currentIndex());
    setting.setValue("stop_bits",ui->comboBox_Stop_bits->currentIndex());
    setting.setValue("timeout",ui->spinBox_Timeout->value());
    setting.setValue("x_dir",ui->spinBox_xDir->value());
    setting.setValue("y_dir",ui->spinBox_yDir->value());
    setting.setValue("laser_on",ui->lineEdit_laserOn->text());
    setting.setValue("laser_off",ui->lineEdit_laserOff->text());

    setting.endGroup();
    //end of serial port settings


    //Parallel port settings
    setting.beginGroup("Parallel port");

    //Laser (motor)port
    setting.setValue("motor_port",ui->lineEdit_motor_port->text().toInt(nullptr,16));
    setting.setValue("motor_bit",ui->lineEdit_motor_bit->text().toInt(nullptr,16));
    setting.setValue("msec_delay_const",ui->lineEdit_msec_delay_const->text().toInt());

    //saving x settings
    setting.setValue("x_dir_bit",ui->lineEdit_x_dir_bit->text().toInt(nullptr,16));
    setting.setValue("x_dir_logic",ui->lineEdit_x_dir_logic->text().toInt(nullptr,16));
    setting.setValue("x_dir_port",ui->lineEdit_x_dir_port->text().toInt(nullptr,16));
    setting.setValue("x_enable_bit",ui->lineEdit_x_enable_bit->text().toInt(nullptr,16));
    setting.setValue("x_enable_port",ui->lineEdit_x_enable_port->text().toInt(nullptr,16));
    setting.setValue("x_limit_max_active",ui->lineEdit_x_limit_max_active->text().toInt(nullptr,16));
    setting.setValue("x_limit_max_bit",ui->lineEdit_x_limit_max_bit->text().toInt(nullptr,16));
    setting.setValue("x_limit_max_port",ui->lineEdit_x_limit_max_port->text().toInt(nullptr,16));
    setting.setValue("x_limit_min_active",ui->lineEdit_x_limit_min_active->text().toInt(nullptr,16));
    setting.setValue("x_limit_min_bit",ui->lineEdit_x_limit_min_bit->text().toInt(nullptr,16));
    setting.setValue("x_limit_min_port",ui->lineEdit_x_limit_min_port->text().toInt(nullptr,16));
    setting.setValue("x_step_bit",ui->lineEdit_x_step_bit->text().toInt(nullptr,16));
    setting.setValue("x_step_port",ui->lineEdit_x_step_port->text().toInt(nullptr,16));

    //saving y settings
    setting.setValue("y_dir_bit",ui->lineEdit_y_dir_bit->text().toInt(nullptr,16));
    setting.setValue("y_dir_logic",ui->lineEdit_y_dir_logic->text().toInt(nullptr,16));
    setting.setValue("y_dir_port",ui->lineEdit_y_dir_port->text().toInt(nullptr,16));
    setting.setValue("y_enable_bit",ui->lineEdit_y_enable_bit->text().toInt(nullptr,16));
    setting.setValue("y_enable_port",ui->lineEdit_y_enable_port->text().toInt(nullptr,16));
    setting.setValue("y_limit_max_active",ui->lineEdit_y_limit_max_active->text().toInt(nullptr,16));
    setting.setValue("y_limit_max_bit",ui->lineEdit_y_limit_max_bit->text().toInt(nullptr,16));
    setting.setValue("y_limit_max_port",ui->lineEdit_y_limit_max_port->text().toInt(nullptr,16));
    setting.setValue("y_limit_min_active",ui->lineEdit_y_limit_min_active->text().toInt(nullptr,16));
    setting.setValue("y_limit_min_bit",ui->lineEdit_y_limit_min_bit->text().toInt(nullptr,16));
    setting.setValue("y_limit_min_port",ui->lineEdit_y_limit_min_port->text().toInt(nullptr,16));
    setting.setValue("y_step_bit",ui->lineEdit_y_step_bit->text().toInt(nullptr,16));
    setting.setValue("y_step_port",ui->lineEdit_y_step_port->text().toInt(nullptr,16));


    setting.endGroup();
    //end of parallel port settings
}

void Settings::detectPorts(){
   QList<QextPortInfo> ports;
   ports= QextSerialEnumerator::getPorts();
   for(int i=0; i<ports.length();i++){
       ui->comboBox_COM->addItem(ports[i].portName);
   }
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
void Settings::on_button_serial_port_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);

}
void Settings::on_butto_parallel_port_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
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



void Settings::on_horizontalSlider_Step_valueChanged(int value)
{
    if(value!=ui->spinBox_step->value()*100)
    ui->spinBox_step->setValue(value*0.01);
}
void Settings::on_spinBox_step_valueChanged(double arg1)
{
    if(arg1!=ui->horizontalSlider_Step->value()*0.01)
    ui->horizontalSlider_Step->setValue((arg1*100));
}

