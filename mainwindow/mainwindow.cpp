#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Image2Machine/graphimage.h"
#include "Image2Machine/commandpreview.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include "settings.h"
#include <cstdint>
#include <QFile>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    commands(&imageMode2,ui->consoleOutput_mode2,&x_current_position,&y_current_position,&hwf,&tmcl)
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

    x_current_position=0;
    y_current_position=0;
    laserON=false;
    alfa=1;
    beta=0;


    commands.laser(laserON,ui->check_simulation->isChecked());

    //linking elements
    commands.setLogOutput(ui->consoleOutput_mode2);
    commands.setProgressBar(ui->progressBar_automode_mode2);

    graph.setImportProgressBar(ui->progressBar_extract_import);
    graph.setExportProgressBar(ui->progressBar_extrac_extract);



    //mode2 image transformation combobox
    ui->comboBox_engraveMode->addItem("Threshold");
    ui->comboBox_engraveMode->addItem("Adaptive Threshold");
    ui->comboBox_engraveMode->addItem("RGB");
    ui->comboBox_engraveMode->setCurrentIndex(0);
    //mode2 extract image
    ui->comboBox_extractmode->addItem("Horizontal");
    ui->comboBox_extractmode->addItem("Vertical");
    ui->comboBox_extractmode->addItem("Horizontal ZZ");
    ui->comboBox_extractmode->addItem("Vertical ZZ");
    ui->comboBox_extractmode->addItem("Depth 1");
    ui->comboBox_extractmode->addItem("Depth 2 (BETA)");
    ui->comboBox_extractmode->addItem("Diagonal");
    ui->comboBox_extractmode->addItem("Diagonal ZZ");
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
    ui->comboBox_testmode->addItem("Test Diagonal");
    ui->comboBox_testmode->addItem("Test Diagonal ZZ");
    ui->comboBox_testmode->setCurrentIndex(0);

    //enabled disabled stuff
    ui->groupManuel->setEnabled(false);

    //hide unused stuff
    ui->groupBox_test_insert->setVisible(false);
    ui->groupBox_resize_cm_mode0->setVisible(false);
    ui->groupBox_extract_status->setVisible(false);
    ui->groupBox_imageBrightness_mode0->setVisible(false);
    loadSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//private functions
void MainWindow::loadImage(QString const &path){

   if(path=="")
       return;
   if(!QFile::exists(path)){
       QMessageBox::critical(this,"File not found","Could not find a file");
       return;
   }

   if(!imageMode0.empty())
        imageMode0.release();

   //resets effects
   ui->checkBox_FlipHorizontal_mode0->setCheckState(Qt::Unchecked);
   ui->checkBox_FlipVertical_mode0->setCheckState(Qt::Unchecked);
   ui->slider_mode0_alfa->setValue(1);
   ui->slider_mode0_beta->setValue(0);
   alfa=1;
   beta=0;

   imagePath=path;
   imageMode0= cv::imread(path.toLocal8Bit().constData(),CV_LOAD_IMAGE_COLOR);

   displayImageInfo();
   displayImageMode0();

   return;
}
void MainWindow::brightnessMode0(){
    if(imageMode0.empty())
        return;
    for(int r=0;r<imageMode0.rows;r++){
        for(int c=0;c<imageMode0.cols;c++){
            for(int ch=0;ch<imageMode0.channels();ch++){
                imageMode0.at<cv::Vec3b>(r,c)[ch]=cv::saturate_cast<uchar>((((float)imageMode0.at<cv::Vec3b>(r,c)[ch]-beta)/alfa)*ui->slider_mode0_alfa->value()+ui->slider_mode0_beta->value());
            }
        }
    }
    alfa=(float)(ui->slider_mode0_alfa->value());
    beta=ui->slider_mode0_beta->value();

}
void MainWindow::displayImageInfo(){
    ui->label_ImageName->setText(imagePath.right(imagePath.size()-1-imagePath.lastIndexOf("/",-1,Qt::CaseSensitivity::CaseSensitive)));
    ui->label_Height->setText( QString::number(imageMode0.rows));
    ui->label_Width->setText(QString::number(imageMode0.cols));
    ui->label_HeightCM->setText(QString::number((float)(imageMode0.rows)*commands.getStep()/100));
    ui->label_WidthCM->setText(QString::number((float)(imageMode0.cols)*commands.getStep()/100));
}
void MainWindow::displayImageMode2Info(){
    ui->label_imagemode2_Height->setText(QString::number(commands.getHeight()));
    ui->label_imagemode2_Width->setText(QString::number(commands.getWidth()));
    ui->label_imagemode2_Height_cm->setText(QString::number((float)(commands.getHeight())*commands.getStep()/100));
    ui->label_imagemode2_Width_cm->setText(QString::number((float)(commands.getWidth())*commands.getStep()/100));
    ui->label_imagemode2_EngravingTime->setText(QString::number((commands.getNumberOfElements()*commands.getEngravingTime()/60000)+(commands.getNumberOfElements()*commands.getStep()/(commands.getSpeed()*254))));
    if(ui->checkBox_imagemode2_preview->isChecked()){
        displayimagemode2Privew();
    }

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
    case 2: rgbAvrage(); break;

    default: break;
    }
    displayImageMode1();
    return true;
}
void MainWindow::setEngraveModesInvisible(){
    ui->thresholdmode->setVisible(false);
    ui->AdaptiveThresholdMode->setVisible(false);
    ui->RGBengrave->setVisible(false);
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
void MainWindow::rgbAvrage(){
    ui->RGBengrave->setVisible(true);
    if(!imageMode1.empty())
        imageMode1.release();
    imageMode1.create(imageMode0.rows,imageMode0.cols,CV_8UC1);
    for(int c=0;c<imageMode1.cols;c++){
        for(int r=0;r<imageMode1.rows;r++){
            if((imageMode0.at<cv::Vec3b>(r,c)[0]>=ui->spinBox_engraveRGB_B->value())&&(imageMode0.at<cv::Vec3b>(r,c)[1]>=ui->spinBox_engraveRGB_G->value())&&(imageMode0.at<cv::Vec3b>(r,c)[2]>=ui->spinBox_engraveRGB_R->value())){
                imageMode1.at<uint8_t>(r,c)=ui->checkBox_RGB_invert->isChecked()?0:255;
            }
            else{
                imageMode1.at<uint8_t>(r,c)=ui->checkBox_RGB_invert->isChecked()?255:0;
            }
        }
    }

}
void MainWindow::openSettings(int index){
    Settings settings(this,index);
    settings.setModal(true);
    settings.exec();
    loadSettings();

    displayImageInfo();

}
void MainWindow::loadSettings(){
    hwf.read_port_ini();
    tmcl.read_port_ini();
    commands.loadini();
    displayImageInfo();
    displayImageMode2Info();

}
void MainWindow::error(QString &error){
    commands.laser(false,false);
    laserON=false;
    QMessageBox::critical(this,"Error",error);
}

void MainWindow::execute(){
    ui->button_start_auto->setEnabled(false);
    ui->button_continue_auto->setEnabled(false);
    ui->button_stop_auto->setEnabled(true);
    ui->groupSelectMode->setEnabled(false);
    ui->button_load_auto->setEnabled(false);
    try{
    while(commands.execute(ui->check_simulation->isChecked())&&!stop){
            displayImageMode2();
            displayCordinates();
            ui->command_listWidget->setCurrentRow(commands.getIndex());
            QApplication::processEvents();

    }
    }
    catch(QString error){
        this->error(error);
    }
    commands.laser(false,false);
    ui->button_start_auto->setEnabled(true);
    ui->groupSelectMode->setEnabled(true);
    ui->button_continue_auto->setEnabled(true);
    ui->button_load_auto->setEnabled(true);
    ui->button_stop_auto->setEnabled(false);
}
void MainWindow::displayimagemode2Privew(){
    if(ui->checkBox_imagemode2_preview->isChecked()){
        commands.displayPreview(&imageMode2);

    }
    else{
        for(int c=0;c<imageMode2.cols;c++){
            for(int r=0;r<imageMode2.rows;r++){
                if(imageMode2.at<uint8_t>(r,c)==187)
                    imageMode2.at<uint8_t>(r,c)=255;
            }
        }
    }
    displayImageMode2();
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
    QFileDialog openfile(this,"Chose a file",QDir::homePath(),"Image (*.png *.jpg *.bmp);;CommandContainer (*.cmc);;G code (*.gcode *txt)");
    if(openfile.exec()== QDialog::Accepted){
    if(openfile.selectedNameFilter()=="Image (*.png *.jpg *.bmp)"){
        loadImage(openfile.selectedFiles()[0]);
    }
    else{
        CommandContainer temp;
        temp.setImageOutput(&imageMode0);
        if (openfile.selectedNameFilter()=="CommandContainer (*.cmc)"){
        temp.loadFile(openfile.selectedFiles()[0]);
        }
        else if (openfile.selectedNameFilter()=="G code (*.gcode *txt)"){
        temp.loadGCode(openfile.selectedFiles()[0]);
        }
        temp.displayPreviewBGR(&imageMode0,0,0,0);
        displayImageMode0();
    }
    }
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
void MainWindow::on_checkBox_keep_aspect_ratio_stateChanged(int arg1)
{
    if(arg1){
       on_spinBox_resize_x_mode0_valueChanged(ui->spinBox_resize_x_mode0->value());
    }
}

void MainWindow::on_spinBox_resize_x_mode0_valueChanged(int arg1)
{
    if(ui->checkBox_keep_aspect_ratio->isChecked()&&!imageMode0.empty()&&(arg1!=ui->spinBox_resize_y_mode0->value()*imageMode0.cols/imageMode0.rows))
    ui->spinBox_resize_y_mode0->setValue(arg1*imageMode0.rows/imageMode0.cols);

}
void MainWindow::on_spinBox_resize_y_mode0_valueChanged(int arg1)
{
    if(ui->checkBox_keep_aspect_ratio->isChecked()&&!imageMode0.empty()&&(arg1!=ui->spinBox_resize_x_mode0->value()*imageMode0.rows/imageMode0.cols))
    ui->spinBox_resize_x_mode0->setValue((arg1*imageMode0.cols/imageMode0.rows));
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

void MainWindow::on_button_mode2_rotateL_clicked()
{
    if(imageMode0.empty()){
        return;
    }
    cv::rotate(imageMode0,imageMode0,cv::RotateFlags::ROTATE_90_COUNTERCLOCKWISE);
    displayImageMode0();
}
void MainWindow::on_button_mode2_rotateR_clicked()
{
    if(imageMode0.empty()){
        return;
    }
    cv::rotate(imageMode0,imageMode0,cv::RotateFlags::ROTATE_90_CLOCKWISE);
    displayImageMode0();
}
void MainWindow::on_slider_mode0_beta_valueChanged(int value)
{
    brightnessMode0();
    displayImageMode0();
}
void MainWindow::on_slider_mode0_alfa_valueChanged(int value)
{
    brightnessMode0();
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
//RGB avrage
void MainWindow::on_spinBox_engraveRGB_R_valueChanged(int arg1)
{
    rgbAvrage();
    displayImageMode1();
}

void MainWindow::on_spinBox_engraveRGB_G_valueChanged(int arg1)
{
    rgbAvrage();
    displayImageMode1();
}

void MainWindow::on_spinBox_engraveRGB_B_valueChanged(int arg1)
{
    rgbAvrage();
    displayImageMode1();
}
void MainWindow::on_checkBox_RGB_invert_stateChanged(int arg1)
{
    rgbAvrage();
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
    displayimagemode2Privew();
    displayImageMode2();
}






//mode1 test insert image functions
void MainWindow::on_button_extract_test_clicked()
{
    GraphImage temp;
    QFileDialog testPath(this,"Chose a file",QDir::homePath(),"Text file (*txt);;All files (*)");
    if(testPath.exec()==QDialog::Accepted){
    switch(ui->comboBox_testmode->currentIndex()){
    case 0: temp.insertColsRowsNotConnected(imageMode1); break;
    case 1: temp.insertRowsColsNotConnected(imageMode1); break;
    case 2: temp.insertColsRowsZigZagNotConnected(imageMode1); break;
    case 3: temp.insertRowsColsZigZagNotConnected(imageMode1); break;
    case 4: temp.insertColsRows(imageMode1); break;
    case 5: temp.insertRowsCols(imageMode1); break;
    case 6: temp.insertColsRowsZigZag(imageMode1); break;
    case 7: temp.insertRowsColsZigZag(imageMode1); break;
    case 8: temp.insertDiagonal(imageMode1); break;
    case 9: temp.insertDiagonalZigZag(imageMode1); break;
    default: break;
    }
    temp.test(testPath.selectedFiles()[0]);
    }
}
//mode1 extract
void MainWindow::on_button_extract_clicked()
{
    QString filter = "cmc";
    QString extractPath = QFileDialog::getSaveFileName(this,"Save as",QDir::homePath(),"CommandContainer (*.cmc)",&filter);

    if(extractPath=="")
        return;

    ui->button_extract->setEnabled(false);
    ui->button_toMachine->setEnabled(false);
    ui->button_extract_stop->setEnabled(true);
    ui->label_extract_extract->setText("Writing a file");
    ui->groupBox_extract_status->setVisible(true);

    switch (ui->comboBox_extractmode->currentIndex()) {
    case 0:if(graph.insertColsRowsNotConnected(imageMode1)) graph.tooFileHeightWidth(extractPath); break;
    case 1:if(graph.insertRowsColsNotConnected(imageMode1)) graph.tooFileHeightWidth(extractPath); break;
    case 2:if(graph.insertColsRowsZigZagNotConnected(imageMode1)) graph.tooFileHeightWidth(extractPath); break;
    case 3:if(graph.insertRowsColsZigZagNotConnected(imageMode1)) graph.tooFileHeightWidth(extractPath); break;
    case 4:if(graph.insertColsRows(imageMode1)) graph.tooFileDepth(extractPath); break;
    case 5:if(graph.insertRowsCols(imageMode1)) graph.tooFileDepth2(extractPath); break;
    case 6:if(graph.insertDiagonal(imageMode1)) graph.tooFileHeightWidth(extractPath); break;
    case 7:if(graph.insertDiagonalZigZag(imageMode1)) graph.tooFileHeightWidth(extractPath); break;
    default:    break;
    }
    ui->groupBox_extract_status->setVisible(false);
    ui->progressBar_extrac_extract->setValue(0);
    ui->progressBar_extract_import->setValue(0);
    ui->button_extract->setEnabled(true);
    ui->button_extract_stop->setEnabled(false);
    ui->button_toMachine->setEnabled(true);



}
void MainWindow::on_button_toMachine_clicked()
{
    ui->button_extract->setEnabled(false);
    ui->button_toMachine->setEnabled(false);
    ui->button_extract_stop->setEnabled(true);
    ui->label_extract_extract->setText("Sending commands");
    ui->groupBox_extract_status->setVisible(true);

    switch (ui->comboBox_extractmode->currentIndex()) {
    case 0:if(graph.insertColsRowsNotConnected(imageMode1)) graph.tooCommandContainerHeightWidth(commands); break;
    case 1:if(graph.insertRowsColsNotConnected(imageMode1)) graph.tooCommandContainerHeightWidth(commands); break;
    case 2:if(graph.insertColsRowsZigZagNotConnected(imageMode1)) graph.tooCommandContainerHeightWidth(commands); break;
    case 3:if(graph.insertRowsColsZigZagNotConnected(imageMode1)) graph.tooCommandContainerHeightWidth(commands); break;
    case 4:if(graph.insertColsRows(imageMode1)) graph.tooCommandContainerDepth(commands); break;
    case 5:if(graph.insertColsRows(imageMode1)) graph.tooCommandContainerDepth2(commands); break;
    case 6:if(graph.insertDiagonal(imageMode1)) graph.tooCommandContainerHeightWidth(commands); break;
    case 7:if(graph.insertDiagonalZigZag(imageMode1)) graph.tooCommandContainerHeightWidth(commands); break;
    default:    break;
    }
    ui->label_imagemode2_Name->setText(ui->label_ImageName->text());
    displayImageMode2Info();
    ui->modeWidget->setCurrentIndex(2);
    commands.printToQListView(ui->command_listWidget);


    ui->groupBox_extract_status->setVisible(false);
    ui->progressBar_extrac_extract->setValue(0);
    ui->progressBar_extract_import->setValue(0);

    ui->button_extract->setEnabled(true);
    ui->button_extract_stop->setEnabled(false);
    ui->button_toMachine->setEnabled(true);

}
void MainWindow::on_button_extract_stop_clicked()
{
    graph.cease();
}
//mode2 auto mode
void MainWindow::on_button_load_auto_clicked()
{
    QFileDialog loadPath(this,"Load commands","Chose a file","CommandContainer (*.cmc);;G code(*.gcode *.txt");
    if(loadPath.exec()==QDialog::Accepted){
    ui->label_imagemode2_Name->setText(loadPath.selectedFiles()[0].right(loadPath.selectedFiles()[0].size()-1-loadPath.selectedFiles()[0].lastIndexOf('/',-1,Qt::CaseSensitive)));
    if(loadPath.selectedNameFilter()=="CommandContainer (*.cmc)"){
    commands.loadFile(loadPath.selectedFiles()[0]);
    }
    else if(loadPath.selectedNameFilter()=="G code(*.gcode *.txt"){
    commands.loadGCode(loadPath.selectedFiles()[0]);
    }
    commands.printToQListView(ui->command_listWidget);
    displayImageMode2Info();
    }
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
    commands.laser(true,ui->check_simulation->isChecked());
    execute();
}


//Settings
void MainWindow::on_settings_settings_triggered()
{
    openSettings(0);
}
void MainWindow::on_settings_Movement_triggered()
{
    openSettings(1);
}
void MainWindow::on_settings_Serial_port_triggered()
{
    openSettings(2);
}
void MainWindow::on_settings_Parallel_port_triggered()
{
    openSettings(3);
}
void MainWindow::on_settings_about_triggered()
{
    openSettings(4);
}
void MainWindow::on_command_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    commandpreview preview(this,&commands,ui->command_listWidget->SelectRows);
    preview.setModal(true);
    preview.exec();
}



//Manual movement
void MainWindow::on_button_set_coordinates_clicked()
{
    try{
    commands.executeSet(ui->spinBox_set_x->value(),ui->spinBox_set_y->value(),x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();
}
void MainWindow::on_movement_upleft_clicked()
{
    try{
    commands.executeUPLEFT(x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();

}
void MainWindow::on_movement_up_clicked()
{
    try{
    commands.executeUP(x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();

}
void MainWindow::on_movement_upright_clicked()
{
    try{
    commands.executeUPRIGHT(x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();
}
void MainWindow::on_movement_left_clicked()
{
    try{
    commands.executeLEFT(x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();
}
void MainWindow::on_movement_right_clicked()
{
    try{
    commands.executeRIGHT(x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();
}
void MainWindow::on_laser_on_off_clicked()
{
    laserON=!laserON;
    try{
    commands.laser(laserON,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
}
void MainWindow::on_movement_downleft_clicked()
{
    try{
    commands.executeDOWNLEFT(x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();
}
void MainWindow::on_movement_down_clicked()
{
    try{
    commands.executeDOWN(x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();
}
void MainWindow::on_movement_downright_clicked()
{
    try{
    commands.executeDOWNRIGHT(x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    displayImageMode2();
    displayCordinates();
}
void MainWindow::on_check_simulation_stateChanged(int arg1)
{

    if(arg1==0){
        commands.connect(true);
    }
    else{
        commands.connect(false);
    }


}


void MainWindow::on_radio_auto_toggled()
{
    laserON=false;
    commands.laser(false,ui->check_simulation->isChecked());
}

void MainWindow::on_button_set_home_clicked()
{
    x_current_position=0;
    y_current_position=0;
    displayCordinates();
}

void MainWindow::on_button_go_home_clicked()
{
    try{
    commands.executeSet(0,0,x_current_position,y_current_position,ui->check_simulation->isChecked());
    }
    catch(QString error){
        this->error(error);
    }
    x_current_position=0;
    y_current_position=0;
    displayCordinates();
}

void MainWindow::displayCordinates(){
    ui->number_rn_x->display(x_current_position);
    ui->number_rn_y->display(y_current_position);
}



void MainWindow::on_checkBox_imagemode2_preview_stateChanged(int arg1)
{
    displayimagemode2Privew();
}

void MainWindow::on_file_open_image_triggered()
{
    ui->modeWidget->setCurrentIndex(0);
    on_buttonLoad_mode0_clicked();
}

void MainWindow::on_file_open_command_container_triggered()
{
    ui->modeWidget->setCurrentIndex(2);
    on_button_load_auto_clicked();
}



















