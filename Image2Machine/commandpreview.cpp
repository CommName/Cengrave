#include "commandpreview.h"
#include "ui_commandpreview.h"
#include <cstdint>
commandpreview::commandpreview(QWidget *parent,CommandContainer *com,int index):
    QDialog(parent),
    ui(new Ui::commandpreview)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);

    if (com==nullptr)
        reject();

    commands=com;
    pixel = find(index);

    commands->printToQListView(ui->commandList);
    displayInfo();
    displayImage();
    ui->commandList->setCurrentRow(index);
}
commandpreview::~commandpreview(){
    delete scene;
    delete ui;
}

container* commandpreview::find(int index){
    container *ret=commands->root;
    while(ret!=nullptr&&ret->index!=index){
        ret=ret->next;
    }
    if (ret==nullptr)
        ret=commands->root;
    return ret;
}

void commandpreview::displayInfo(){
    ui->label_command->setText(CommandContainer::commandToString(pixel->command));
    ui->label_index->setText(QString::number(pixel->index));
    ui->label_x->setText(QString::number(pixel->x));
    ui->label_y->setText(QString::number(pixel->y));
    return;
}

void commandpreview::displayImage(){
    commands->displayPreview(&image);
    image.at<uint8_t>(pixel->y,pixel->x)=0;

    cv::Mat temp;
    cv::cvtColor(image,temp,CV_BGR2RGB);
    QImage output((const uchar *)temp.data,temp.cols,temp.rows,temp.step,QImage::Format_RGB888);
    output.bits();
    scene->clear();
    scene->addPixmap(QPixmap::fromImage(output));
    ui->image->fitInView(scene->itemsBoundingRect(),Qt::KeepAspectRatio);

    return;
}

void commandpreview::setCurrent(){
    commands->root=pixel;
    ui->button_set_current->setEnabled(false);
    return;
}

void commandpreview::next(){
    pixel=pixel->next;
    if(pixel==nullptr)
        pixel=commands->last;
    if(pixel==commands->current){
        ui->button_set_current->setEnabled(false);
    }
    else{
        ui->button_set_current->setEnabled(true);
    }
    ui->commandList->setCurrentRow(pixel->index);
    displayInfo();
    displayImage();
    return;
}

void commandpreview::pervious(){
    pixel=pixel->previous;
    if(pixel==nullptr)
        pixel=commands->root;
    if(pixel==commands->current){
        ui->button_set_current->setEnabled(false);
    }
    else{
        ui->button_set_current->setEnabled(true);
    }
    ui->commandList->setCurrentRow(pixel->index);
    displayImage();
    displayInfo();
}


//SLOTS

//Buttons
void commandpreview::on_button_next_clicked()
{
    next();
}

void commandpreview::on_button_previous_clicked()
{
    pervious();
}

void commandpreview::on_button_set_current_clicked()
{
    setCurrent();
}

void commandpreview::on_commandList_currentRowChanged(int currentRow)
{
    pixel = find(currentRow);
    if(pixel==commands->current){
        ui->button_set_current->setEnabled(false);
    }
    else{
        ui->button_set_current->setEnabled(true);
    }
    displayInfo();
    displayImage();
    return;

}

//ZOOM
void commandpreview::on_button_zoom_in_clicked()
{
    ui->image->scale(1.1,1.1);
}
void commandpreview::on_button_zoom_out_clicked()
{
    ui->image->scale(1/1.1,1/1.1);
}
void commandpreview::on_button_zoom_fit_clicked()
{
    ui->image->fitInView(scene->itemsBoundingRect(),Qt::KeepAspectRatio);
}
void commandpreview::on_button_zoom_norma_clicked()
{
    ui->image->resetTransform();
}
