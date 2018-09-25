#include "commandcontainer.h"
#include <fstream>
#include <cstdint>
#include <iostream>
#include <QApplication>
#include <QSettings>
CommandContainer::CommandContainer()
{
    root=nullptr;
    last=nullptr;
    current=nullptr;
    display=nullptr;
    logs=nullptr;
    hwf=nullptr;
    secure=false;
    progressbar=nullptr;
    displayX=nullptr;
    displayY=nullptr;
    numOfElemets=0;
    engraveTime=1;
    speed=20;
    step=20;

}
CommandContainer::CommandContainer(cv::Mat *image,QPlainTextEdit *log,int *x,int *y,HWF *h):CommandContainer(){

    display=image;
    logs=log;
    displayX=x;
    displayY=y;

    hwf=h;

    loadini();
}

CommandContainer::~CommandContainer(){
  deleteAll();
}

void CommandContainer::deleteAll(){
   container* tmp=root;
   while(tmp!=nullptr){
       container* del=tmp;
       tmp=tmp->next;
       delete del;
   }
   root=nullptr;
   last=nullptr;
   current=nullptr;
   numOfElemets=0;
}

bool CommandContainer::insert(commands command){
    if(root==nullptr){
        //only set commands are allowed to be first
       return false;
    }
    last->next=new container;
    last->next->next=nullptr;
    last->next->previous=last;
    last->next->command=command;
    last->next->index=last->index+1;
    switch (last->next->command) {
    case commands::UP: last->next->x=last->x; last->next->y=last->y-1; break;
    case commands::DOWN: last->next->x=last->x; last->next->y=last->y+1; break;
    case commands::LEFT: last->next->x=last->x-1; last->next->y=last->y; break;
    case commands::RIGHT: last->next->x=last->x+1; last->next->y=last->y; break;
    case commands::UPLEFT: last->next->x=last->x-1; last->next->y=last->y-1; break;
    case commands::UPRIGHT: last->next->x=last->x+1; last->next->y=last->y-1; break;
    case commands::DOWNLEFT: last->next->x=last->x-1; last->next->y=last->y+1; break;
    case commands::DOWNRIGHT: last->next->x=last->x+1; last->next->y=last->y+1; break;
    default: return false;
    }
    last=last->next;
    numOfElemets++;
    progressbar->setMaximum(numOfElemets);
    return true;
}
bool CommandContainer::insertSet(int x,int y){
    if(root==nullptr){
        root=last=current=new container;
        root->index=0;
        last->previous=nullptr;

    }
    else{
        last->next=new container;
        last->next->previous=last;
        last->next->index=last->index+1;
        last=last->next;
    }
    last->x=x;
    last->y=y;
    last->command=commands::SET;
    last->next=nullptr;
    numOfElemets++;
    progressbar->setMaximum(numOfElemets);
    return true;
}

bool CommandContainer::loadFile(QString const &path){
    std::ifstream f(path.toLocal8Bit().constData(),std::ios::in|std::ios::binary);
    if(!f)
        return false;
    if(root!=nullptr){
        deleteAll();
    }
    while(!f.eof()){
        uint8_t temp;
        f>>temp;
        if(temp==commands::SET){
            char tr; //temporary solution for seperating int until I have time to fix this
            int x=0,y=0;
            f>>x;
            f>>tr;
            f>>y;
            insertSet(x,y);
        }
        else{
            insert((commands)temp);
        }
    }
    f.close();
    current=root;
    return true;
}
bool CommandContainer::saveFile(QString const &path){
    std::ofstream f(path.toLocal8Bit().constData(),std::ios::out|std::ios::binary);
    if(!f)
        return false;
    container *ptr=root;
    while(ptr!=nullptr){
        f<<(uint8_t)(ptr->command);
        if(ptr->command==commands::SET)
            f<<ptr->x<<'y'<<ptr->y;
        ptr=ptr->next;
    }
    return true;
}
void CommandContainer::printToQListView(QListWidget *listView){
    container *temp=root;
    while(temp!=nullptr){
        switch (temp->command) {
        case commands::UP: listView->addItem("UP"); break;
        case commands::DOWN: listView->addItem("DOWN"); break;
        case commands::LEFT: listView->addItem("LEFT"); break;
        case commands::RIGHT: listView->addItem("RIGHT"); break;
        case commands::UPLEFT: listView->addItem("UPLEFT"); break;
        case commands::UPRIGHT: listView->addItem("UPRIGHT"); break;
        case commands::DOWNLEFT: listView->addItem("DOWNLEFT"); break;
        case commands::DOWNRIGHT: listView->addItem("DOWNRIGHT"); break;
        case commands::SET: listView->addItem("SET "+QString::number(temp->x)+' '+QString::number(temp->y)); break;
        }

        temp=temp->next;
    }
}

//Returns false when it comes to the end
bool CommandContainer::execute(bool simulation){
    if(current==nullptr)
        return false;

    switch (current->command) {
    case commands::UP: this->executeUP(current->previous->x,current->previous->y,simulation); break;
    case commands::DOWN: this->executeDOWN(current->previous->x,current->previous->y,simulation); break;;
    case commands::LEFT: this->executeLEFT(current->previous->x,current->previous->y,simulation); break;
    case commands::RIGHT: this->executeRIGHT(current->previous->x,current->previous->y,simulation); break;
    case commands::UPLEFT: this->executeUPLEFT(current->previous->x,current->previous->y,simulation); break;
    case commands::UPRIGHT: this->executeUPRIGHT(current->previous->x,current->previous->y,simulation); break;
    case commands::DOWNLEFT: this->executeDOWNLEFT(current->previous->x,current->previous->y,simulation); break;
    case commands::DOWNRIGHT: this->executeDOWNRIGHT(current->previous->x,current->previous->y,simulation); break;
    case commands::SET:
                laser(false);
                this->executeSet(current->x,current->y,current->previous==nullptr?0:current->previous->x,current->previous==nullptr?0:current->previous->y,simulation);
                laser(true);
                break;

    }
    Sleeper::msleep(engraveTime);
    if(progressbar!=nullptr){
        progressbar->setValue(current->index+1);
    }
    current=current->next;
    return true;
}

void CommandContainer::setImageOutput(cv::Mat *image){
    display=image;
}

void CommandContainer::setCurrent(int index){

    current=root;
    for(int i=0;i<index&&current!=nullptr;i++){
        current=current->next;
    }
    return;
}

//Manual controls
void CommandContainer::executeSet(int x,int y,int x_previous,int y_previous,bool simulation){
    //execute
    if(!simulation){
        hwf->set_vxy(speed);
        hwf->stepx((x-x_previous)*step*1000/hwf->get_korak_x(),1);
        hwf->stepy((y-y_previous)*step*1000/hwf->get_korak_y(),1);

    }
    displayAll(x,y,commands::SET);
}
void CommandContainer::executeUP(int x_previous,int y_previous,bool simulation){
    //execute
    if(!simulation){
        hwf->set_vxy(speed);
        hwf->stepy(-1*step*1000/hwf->get_korak_y(),1);
    }
    displayAll(x_previous,y_previous-1,commands::UP);

}
void CommandContainer::executeDOWN(int x_previous,int y_previous, bool simulation){
    //execute
    if(!simulation){
        hwf->set_vxy(speed);
        hwf->stepy(step*1000/hwf->get_korak_y(),1);

    }
    displayAll(x_previous,y_previous+1,commands::DOWN);
}
void CommandContainer::executeLEFT(int x_previous,int y_previous,bool simulation){

    //execute
    if(!simulation){
        hwf->set_vxy(speed);
        hwf->stepx(-1*step*1000/hwf->get_korak_x(),1);
    }
    displayAll(x_previous-1,y_previous,commands::LEFT);
}
void CommandContainer::executeRIGHT(int x_previous,int y_previous,bool simulation){
    //execute
    if(!simulation){
        hwf->set_vxy(speed);
        hwf->stepx(step*1000/hwf->get_korak_x(),1);

    }
    displayAll(x_previous+1,y_previous,commands::RIGHT);
}
void CommandContainer::executeUPLEFT(int x_previous,int y_previous,bool simulation){

    //execute
    if(!simulation){
        hwf->set_vxy(10);
        hwf->stepx(-1*speed*1000/hwf->get_korak_x(),1);
        hwf->stepy(-1*step*1000/hwf->get_korak_y(),1);

    }
    displayAll(x_previous-1,y_previous-1,commands::UPLEFT);
}
void CommandContainer::executeDOWNLEFT(int x_previous,int y_previous,bool simulation){

    //execute
    if(!simulation){
        hwf->set_vxy(speed);
        hwf->stepx(-1*step*1000/hwf->get_korak_x(),1);
        hwf->stepy(step*1000/hwf->get_korak_y(),1);
    }
    displayAll(x_previous-1,y_previous+1,commands::DOWNLEFT);
}
void CommandContainer::executeUPRIGHT(int x_previous,int y_previous,bool simulation){

    //execute
    if(!simulation){
        hwf->set_vxy(speed);
        hwf->stepx(step*1000/hwf->get_korak_x(),1);
        hwf->stepy(-1*step*1000/hwf->get_korak_y(),1);
    }
    displayAll(x_previous+1,y_previous-1,commands::UPRIGHT);
}
void CommandContainer::executeDOWNRIGHT(int x_previous,int y_previous,bool simulation){


    //execute
    if(!simulation){
        hwf->set_vxy(speed);
       hwf->stepx(step*1000/hwf->get_korak_x(),1);
       hwf->stepy(step*1000/hwf->get_korak_y(),1);
    }
    displayAll(x_previous+1,y_previous+1,commands::DOWNRIGHT);
}
void CommandContainer::laser(bool on){


    if(on){
        if(logs!=nullptr){
            logs->appendPlainText("Laser on");
        hwf->motor_off(); //reversed logic
        }
    }
    else{
        if(logs!=nullptr){
            logs->appendPlainText("Laser off");
        }
        hwf->motor_on(); //reversed logic
    }

}

void CommandContainer::loadini(){
    QSettings settings("cengrave.ini",QSettings::IniFormat);
    settings.beginGroup("Movement");
    speed=settings.value("movement speed",20).toInt();
    step=settings.value("step",1).toInt();
    engraveTime=settings.value("engrave time",1).toInt();
    settings.endGroup();
}

void CommandContainer::displayAll(int x,int y,commands com){
    //image privew
    if(display!=nullptr){
        if(y<0||x<0||y>=display->cols||x>=display->rows){
            //if(secure)
            //    throw QString("Image out of range");
        }
        else
            display->at<uint8_t>(y,x)=0;
    }
    if(displayX!=nullptr)
        *displayX=x;
    if(displayY!=nullptr)
        *displayY=y;
    //logs output
    if(logs!=nullptr){
        switch (com) {
        case commands::UP:          logs->appendPlainText("UP"); break;
        case commands::DOWN:        logs->appendPlainText("DOWN"); break;
        case commands::LEFT:        logs->appendPlainText("LEFT"); break;
        case commands::RIGHT:       logs->appendPlainText("RIGHT"); break;
        case commands::UPLEFT:      logs->appendPlainText("UPLEFT"); break;
        case commands::UPRIGHT:     logs->appendPlainText("UPRIGHT"); break;
        case commands::DOWNLEFT:    logs->appendPlainText("DOWNLEFT"); break;
        case commands::DOWNRIGHT:   logs->appendPlainText("DOWNRIGHT"); break;
        case commands::SET:         logs->appendPlainText("SET "+QString::number(x)+' '+QString::number(y)); break;
        }
    }
}

