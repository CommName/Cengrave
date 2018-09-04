#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
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

    //mode2 image transformation combobox
    ui->comboBox_engraveMode->addItem("Threshold");
    ui->comboBox_engraveMode->setCurrentIndex(0);

    //hide unused stuff
    ui->groupBox_resize_cm_mode0->setVisible(false);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//private functions
void MainWindow::loadImage(QString const &path){

   if(path=="")
       return;

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


bool MainWindow::engrave(){
    if(imageMode0.empty())
        return false;
    setEngraveModesInvisible();
    switch(ui->comboBox_engraveMode->currentIndex()){
    case 0: thresholdMode(); break;

    default: break;
    }
    displayImageMode1();
    return true;
}
void MainWindow::setEngraveModesInvisible(){
    ui->thresholdmode->setVisible(false);
    ui->RGBmode->setVisible(false);
}
void MainWindow::thresholdMode(){
 ui->thresholdmode->setVisible(true);
 cv::cvtColor(imageMode0,imageMode1,CV_BGR2GRAY);
 cv::threshold(imageMode1,imageMode1,ui->threshold_slider->value(),255,ui->threshold_invert_checkBox->isChecked()?cv::THRESH_BINARY_INV:cv::THRESH_BINARY);
}
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

void MainWindow::on_threshold_slider_valueChanged(int value)
{
    thresholdMode();
    displayImageMode1();
}
void MainWindow::on_threshold_invert_checkBox_stateChanged(int arg1)
{
    thresholdMode();
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





