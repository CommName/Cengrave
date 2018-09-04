#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene0=new QGraphicsScene(this);
    scene1=new QGraphicsScene(this);
    scene2=new QGraphicsScene(this);
    ui->imageViewMode0->setScene(scene0);
    ui->imageViewMode1->setScene(scene1);
    ui->imageViewMode2->setScene(scene2);

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


//Slots

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




