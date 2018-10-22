#include "commandpreview.h"
#include "ui_commandpreview.h"
#include <cstdint>
commandpreview::commandpreview(QWidget *parent,CommandContainer *com,int index):
    QDialog(parent),
    ui(new Ui::commandpreview)
{
    ui->setupUi(this);

    if (com==nullptr)
        reject();

    commands=com;
    pixel = find(index);

    displayInfo();
    displayImage();
}
commandpreview::~commandpreview(){
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
    return;
}

void commandpreview::setCurrent(){
    commands->root=pixel;
    return;
}

void commandpreview::next(){
    pixel=pixel->next;
    if(pixel==nullptr)
        pixel=commands->last;
    displayInfo();
    displayImage();
    return;
}

void commandpreview::pervious(){
    pixel=pixel->previous;
    if(pixel==nullptr)
        pixel=commands->root;
    displayImage();
    displayInfo();
}
