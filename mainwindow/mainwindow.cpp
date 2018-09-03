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
}

MainWindow::~MainWindow()
{
    delete ui;
}

//private functions
void MainWindow::loadImage(){

   QString path = QFileDialog::getOpenFileName(this,"Chose image","","*.png *.jpg *.bmp");
   if(path=="")
       return;
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
    cv::Mat temp;


    cv::cvtColor(imageMode0,temp,CV_BGR2RGB);
    QImage output((const uchar *)temp.data,temp.cols,temp.rows,QImage::Format_RGB888);
    output.bits();
    scene0->clear();
    scene0->addPixmap(QPixmap::fromImage(output));
    ui->imageViewMode0->fitInView(scene0->itemsBoundingRect(),Qt::KeepAspectRatio);
}

//Slots
void MainWindow::on_buttonLoad_clicked()
{
    loadImage();
}

