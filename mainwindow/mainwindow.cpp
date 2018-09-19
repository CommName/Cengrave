#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Image2Machine/graphimage.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include "settings.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->modeWidget->setCurrentIndex(0);


    //Setting up QGraphicView
    scene0=new QGraphicsScene(this);
    scene1=new QGraphicsScene(this);
    scene2=new QGraphicsScene(this);
    ui->imageViewMode0->setScene(scene0);
    ui->imageViewMode1->setScene(scene1);
    ui->imageViewMode2->setScene(scene2);
    imageMode2.create(500,500,CV_8U);
    imageMode2= cv::Scalar(255);

    commands.setImageOutput(&imageMode2);
    commands.setLogOutput(ui->consoleOutput_mode2);
    commands.setHWF(&hwf);
    x_current_position=0;
    y_current_position=0;
    laserON=false;
    commands.loadini();
    commands.laser(laserON);
    hwf.set_vxy(20);

    //mode2 image transformation combobox
    ui->comboBox_engraveMode->addItem("Threshold");
    ui->comboBox_engraveMode->addItem("Adaptive Threshold");
    ui->comboBox_engraveMode->setCurrentIndex(0);
    //mode2 extract image
    ui->comboBox_extractmode->addItem("Col-Row");
    ui->comboBox_extractmode->addItem("Row-Col");
    ui->comboBox_extractmode->addItem("Col-Row ZZ");
    ui->comboBox_extractmode->addItem("Row-Col ZZ");
    ui->comboBox_extractmode->addItem("Depth Col-Row");
    ui->comboBox_extractmode->addItem("Depth Row-Co");
    ui->comboBox_extractmode->addItem("Depth Col-Row ZZ");
    ui->comboBox_extractmode->addItem("Depth Row-Col ZZ");
    ui->comboBox_extractmode->setCurrentIndex(0);
    //mode2 test
    ui->comboBox_testmode->addItem("Test Width Height Not Connected");
    ui->comboBox_testmode->addItem("Test Height Width Not Connected");
    ui->comboBox_testmode->addItem("Test Width Height ZZ Not Connected");
    ui->comboBox_testmode->addItem("Test Height Width ZZ Not Connected");
    ui->comboBox_testmode->addItem("Test Width Height");
    ui->comboBox_testmode->addItem("Test Height Width");
    ui->comboBox_testmode->addItem("Test Width Height ZZ");
    ui->comboBox_testmode->addItem("Test Height Width ZZ");
    ui->comboBox_testmode->setCurrentIndex(0);

    //enabled disabled stuff
    ui->groupManuel->setEnabled(false);

    //hide unused stuff
    ui->groupBox_test_insert->setVisible(false);
    ui->groupBox_resize_cm_mode0->setVisible(false);
    ui->groupBox_imageEffects_mode0->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//private functions
void MainWindow::loadImage(QString const &path){

   if(path=="")
       return;

   if(!imageMode0.empty())
        imageMode0.release();

   //resets effects
   ui->checkBox_FlipHorizontal_mode0->setCheckState(Qt::Unchecked);
   ui->checkBox_FlipVertical_mode0->setCheckState(Qt::Unchecked);

   imagePath=path;
   imageMode0= cv::imread(path.toLocal8Bit().constData(),CV_LOAD_IMAGE_COLOR);

   displayImageInfo();
   displayImageMode0();

   return;
}
void MainWindow::displayImageInfo(){

    ui->label_Height->setText( QString::number(imageMode0.rows));
    ui->label_Width->setText(QString::number(imageMode0.cols));

}
void MainWindow::displayImageMode0(){
    //transform CV:MAT to QImage
    cv::Mat temp;
    cv::cvtColor(imageMode0,temp,CV_BGR2RGB);
    QImage output((const uchar *)temp.data,temp.cols,temp.rows,temp.step,QImage::Format_RGB888);
    output.bits();
    //Displays QImage
    scene0->clear();
    scene0->addPixmap(QPixmap::fromImage(output));
    ui->imageViewMode0->fitInView(scene0->itemsBoundingRect(),Qt::KeepAspectRatio);
}
void MainWindow::displayImageMode1(){


   QImage output((const uchar *)imageMode1.data,imageMode1.cols,imageMode1.rows,imageMode1.step,QImage::Format_Grayscale8);
   output.bits();
   scene1->clear();
   scene1->addPixmap(QPixmap::fromImage(output));
   ui->imageViewMode1->fitInView(scene1->itemsBoundingRect(),Qt::KeepAspectRatio);
}
void MainWindow::displayImageMode2(){
    QImage output((const uchar *)imageMode2.data,imageMode2.cols,imageMode2.rows,imageMode2.step,QImage::Format_Grayscale8);
    output.bits();
    scene2->clear();
    scene2->addPixmap(QPixmap::fromImage(output));
}

bool MainWindow::engrave(){
    if(imageMode0.empty())
        return false;
    setEngraveModesInvisible();
    if(!imageMode1.empty())
        imageMode1.release();

    switch(ui->comboBox_engraveMode->currentIndex()){
    case 0: thresholdMode(); break;
    case 1: adaptiveThreshold(); break;

    default: break;
    }
    displayImageMode1();
    return true;
}
void MainWindow::setEngraveModesInvisible(){
    ui->thresholdmode->setVisible(false);
    ui->AdaptiveThresholdMode->setVisible(false);
}
void MainWindow::thresholdMode(){
 ui->thresholdmode->setVisible(true);
 if(!imageMode1.empty())
     imageMode1.release();
 imageMode1.create(imageMode0.rows,imageMode1.cols,CV_8UC1);

 cv::cvtColor(imageMode0,imageMode1,CV_BGR2GRAY);
 cv::threshold(imageMode1,imageMode1,ui->threshold_slider->value(),255,ui->threshold_invert_checkBox->isChecked()?cv::THRESH_BINARY_INV:cv::THRESH_BINARY);
}
void MainWindow::adaptiveThreshold(){

 ui->AdaptiveThresholdMode->setVisible(true);
 if(!imageMode1.empty())
     imageMode1.release();
 imageMode1.create(imageMode0.rows,imageMode1.cols,CV_8UC1);
 cv::cvtColor(imageMode0,imageMode1,CV_BGR2GRAY);
 cv::adaptiveThreshold(imageMode1,imageMode1,255,ui->adapriveThreshold_MeanC_radiobutton->isChecked()?cv::ADAPTIVE_THRESH_MEAN_C:cv::ADAPTIVE_THRESH_GAUSSIAN_C,ui->adaptiveThreshold_invert_checkBox->isChecked()?cv::THRESH_BINARY_INV:cv::THRESH_BINARY,ui->adaptiveThreshold_block_size_slider->value()*2+1,ui->adaptiveThreshold_C_slider->value());

}

void MainWindow::loadSettings(){
    hwf.read_port_ini();
    commands.loadini();
    QSettings setting("cengrave.conf");



}

void MainWindow::execute(){
    hwf.set_vxy(100);
    while(commands.execute(ui->check_simulation->isChecked())&&!stop){
            displayImageMode2();
            QApplication::processEvents();

    }
}


//Extrac


//Slots and Signals


void MainWindow::on_modeWidget_currentChanged(int index)
{
    if(index==1){
        //mode1
        if(!engrave()){
            //Error loading image
            QMessageBox::warning(this,"No image found","Please select an image before you continue with the engraving process!");
            ui->modeWidget->setCurrentIndex(0);
            on_buttonLoad_mode0_clicked();
            return;
        }
    }
}

//Mode0
//input image
void MainWindow::on_buttonLoad_mode0_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,"Chose image","","*.png *.jpg *.bmp");
    loadImage(path);
}
void MainWindow::on_buttonReload_mode0_clicked()
{
    loadImage(imagePath);
}
void MainWindow::on_buttonEngrave_clicked()
{
    ui->modeWidget->setCurrentIndex(1);
}

//image transformation
void MainWindow::on_button_resize_mode0_clicked()
{
    if(imageMode0.empty())
        return;
    cv::resize(imageMode0,imageMode0,CvSize(ui->spinBox_resize_x_mode0->value(),ui->spinBox_resize_y_mode0->value()),0,0,CV_INTER_LINEAR);
    displayImageInfo();
    displayImageMode0();
}
void MainWindow::on_checkBox_FlipVertical_mode0_clicked()
{
    if(imageMode0.empty())
        return;
    cv::flip(imageMode0,imageMode0,0);
    displayImageMode0();

}
void MainWindow::on_checkBox_FlipHorizontal_mode0_clicked()
{
    if(imageMode0.empty())
        return;
    cv::flip(imageMode0,imageMode0,1);
    displayImageMode0();
}

//engraving image
void MainWindow::on_comboBox_engraveMode_currentIndexChanged(int index)
{
    engrave();
}
//threshold
void MainWindow::on_threshold_slider_valueChanged(int value)
{
    ui->threshold_spinBox->setValue(value);
    thresholdMode();
    displayImageMode1();
}
void MainWindow::on_threshold_invert_checkBox_stateChanged(int arg1)
{
    thresholdMode();
    displayImageMode1();
}
//adaptive threshold
void MainWindow::on_adaptiveThreshold_C_slider_valueChanged(int value)
{
    ui->adaptiveThreshold_C_spinBox->setValue(value);
    adaptiveThreshold();
    displayImageMode1();
}
void MainWindow::on_adaptiveThreshold_block_size_slider_valueChanged(int value)
{
    ui->adaptiveThreshold_block_size_spinBox->setValue(value);
    adaptiveThreshold();
    displayImageMode1();
}
void MainWindow::on_adaptiveThreshold_invert_checkBox_stateChanged(int arg1)
{
    adaptiveThreshold();
    displayImageMode1();
}
void MainWindow::on_adapriveThreshold_MeanC_radiobutton_clicked()
{
    adaptiveThreshold();
    displayImageMode1();
}
void MainWindow::on_adapriveThreshold_GaussianC_radiobutton_clicked()
{
    adaptiveThreshold();
    displayImageMode1();
}


//Zoom functions
//moode0
void MainWindow::on_button_mode0_zoom_in_clicked()
{
    ui->imageViewMode0->scale(1.1,1.1);
}
void MainWindow::on_button_mode0_zoom_out_clicked()
{
    ui->imageViewMode0->scale(1/1.1,1/1.1);
}
void MainWindow::on_button_mode0_zoom_fit_clicked()
{
    ui->imageViewMode0->fitInView(scene0->itemsBoundingRect(),Qt::KeepAspectRatio);
}
void MainWindow::on_button_mode0_zoom_normal_clicked()
{
    ui->imageViewMode0->resetTransform();
}
//mode1
void MainWindow::on_button_mode1_zoom_in_clicked()
{
    ui->imageViewMode1->scale(1.1,1.1);
}
void MainWindow::on_button_mode1_zoom_out_clicked()
{
    ui->imageViewMode1->scale(1/1.1,1/1.1);
}
void MainWindow::on_button_mode1_zoom_fit_clicked()
{
    ui->imageViewMode1->fitInView(scene1->itemsBoundingRect(),Qt::KeepAspectRatio);
}
void MainWindow::on_button_mode1_zoom_normal_clicked()
{
    ui->imageViewMode1->resetTransform();
}
//mode2
void MainWindow::on_button_mode2_zoom_in_clicked()
{
    ui->imageViewMode2->scale(1.1,1.1);
}
void MainWindow::on_button_mode2_zoom_out_clicked()
{
    ui->imageViewMode2->scale(1/1.1,1/1.1);
}
void MainWindow::on_button_mode2_zoom_fit_clicked()
{
  ui->imageViewMode2->fitInView(scene2->itemsBoundingRect(),Qt::KeepAspectRatio);
}
void MainWindow::on_button_mode2_zoom_normal_clicked()
{
   ui->imageViewMode2->resetTransform();
}
void MainWindow::on_button_clear_console_clicked()
{
    ui->consoleOutput_mode2->clear();

}
void MainWindow::on_button_clear_image_clicked()
{
    imageMode2=cv::Scalar(255);
    displayImageMode2();
}






//mode1 test insert image functions
void MainWindow::on_button_extract_test_clicked()
{
    GraphImage temp;
    QString testPath = QFileDialog::getSaveFileName(this,"Save as","C://");
    switch(ui->comboBox_testmode->currentIndex()){
    case 0: temp.insertColsRowsNotConnected(imageMode1); break;
    case 1: temp.insertRowsColsNotConnected(imageMode1); break;
    case 2: temp.insertColsRowsZigZagNotConnected(imageMode1); break;
    case 3: temp.insertRowsColsZigZagNotConnected(imageMode1); break;
    case 4: temp.insertColsRows(imageMode1); break;
    case 5: temp.insertRowsCols(imageMode1); break;
    case 6: temp.insertColsRowsZigZag(imageMode1); break;
    case 7: temp.insertRowsColsZigZag(imageMode1); break;
    default: break;
    }
    temp.test(testPath);
}
//mode1 extract
void MainWindow::on_button_extract_clicked()
{
    GraphImage temp;
    QString extractPath = QFileDialog::getSaveFileName(this,"Save as","C://");
    switch (ui->comboBox_extractmode->currentIndex()) {
    case 0:temp.insertColsRowsNotConnected(imageMode1); temp.tooFileHeightWidth(extractPath); break;
    case 1:temp.insertRowsColsNotConnected(imageMode1); temp.tooFileHeightWidth(extractPath); break;
    case 2:temp.insertColsRowsZigZagNotConnected(imageMode1); temp.tooFileHeightWidth(extractPath); break;
    case 3:temp.insertRowsColsZigZagNotConnected(imageMode1); temp.tooFileHeightWidth(extractPath); break;
    case 4:temp.insertColsRows(imageMode1); temp.tooFileDepth(extractPath); break;
    case 5:temp.insertRowsCols(imageMode1); temp.tooFileDepth(extractPath); break;
    case 6:temp.insertColsRowsZigZag(imageMode1); temp.tooFileDepth(extractPath); break;
    case 7:temp.insertRowsColsZigZag(imageMode1); temp.tooFileDepth(extractPath); break;
    default:    break;
    }
}

//mode2 auto mode
void MainWindow::on_button_load_auto_clicked()
{
    QString loadPath = QFileDialog::getOpenFileName(this,"Load commands");
    commands.loadFile(loadPath);
    commands.printToQListView(ui->command_listWidget);

}
void MainWindow::on_button_start_auto_clicked()
{
    stop=false;
    commands.setCurrent(0);
    execute();

}
void MainWindow::on_button_stop_auto_clicked()
{
    stop=true;
}
void MainWindow::on_button_continue_auto_clicked()
{
    stop=false;

    execute();
}


//Settings
void MainWindow::on_actionSettings_triggered()
{
 Settings settings(this);
 settings.setModal(true);
 settings.exec();
 loadSettings();
}




//Manual movement
void MainWindow::on_button_set_coordinates_clicked()
{

    commands.executeSet(ui->spinBox_set_x->value(),ui->spinBox_set_y->value(),x_current_position,y_current_position,ui->check_simulation->isChecked());
    x_current_position=ui->spinBox_set_x->value();
    y_current_position=ui->spinBox_set_y->value();
    displayImageMode2();
}
void MainWindow::on_movement_upleft_clicked()
{
    commands.executeUPLEFT(x_current_position--,y_current_position--,ui->check_simulation->isChecked());
    displayImageMode2();

}
void MainWindow::on_movement_up_clicked()
{
    commands.executeUP(x_current_position,y_current_position--,ui->check_simulation->isChecked());
    displayImageMode2();

}
void MainWindow::on_movement_upright_clicked()
{
    commands.executeUPRIGHT(x_current_position++,y_current_position--,ui->check_simulation->isChecked());
    displayImageMode2();
}
void MainWindow::on_movement_left_clicked()
{
    commands.executeLEFT(x_current_position--,y_current_position,ui->check_simulation->isChecked());
    displayImageMode2();
}
void MainWindow::on_movement_right_clicked()
{
    commands.executeRIGHT(x_current_position++,y_current_position,ui->check_simulation->isChecked());
    displayImageMode2();
}
void MainWindow::on_laser_on_off_clicked()
{
    laserON=!laserON;
    commands.laser(laserON);

}
void MainWindow::on_movement_downleft_clicked()
{
    commands.executeDOWNLEFT(x_current_position--,y_current_position++,ui->check_simulation->isChecked());
    displayImageMode2();
}
void MainWindow::on_movement_down_clicked()
{
    commands.executeDOWN(x_current_position,y_current_position++,ui->check_simulation->isChecked());
    displayImageMode2();
}
void MainWindow::on_movement_downright_clicked()
{
    commands.executeDOWNRIGHT(x_current_position++,y_current_position++,ui->check_simulation->isChecked());
    displayImageMode2();
}
void MainWindow::on_check_simulation_stateChanged(int arg1)
{

    if(arg1==0){
        hwf.enable_on();
    }
    else{
        hwf.enable_off();
    }


}
