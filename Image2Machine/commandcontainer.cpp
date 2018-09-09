#include "commandcontainer.h"
#include <fstream>
#include <cstdint>
#include <iostream>
#include <QApplication>
CommandContainer::CommandContainer()
{
    root=nullptr;
    last=nullptr;
    current=nullptr;
    display=nullptr;
    logs=nullptr;
}

CommandContainer::~CommandContainer(){
  deleteAll();
}

void CommandContainer::deleteAll(){
   container* tmp;
   while(tmp!=nullptr){
       container* del=tmp;
       tmp=tmp->next;
       delete del;
   }
   root=nullptr;
   last=nullptr;
   current=nullptr;
}

bool CommandContainer::insert(commands command){
    if(root==nullptr){
        //only set commands are allowed to be first
       return false;
    }
    last->next=new container;
    last->next->next=nullptr;
    last->next->command=command;
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
    return true;
}
bool CommandContainer::insertSet(int x,int y){
    if(root==nullptr){
        root=last=current=new container;
    }
    else{
        last->next=new container;
        last=last->next;
    }
    last->x=x;
    last->y=y;
    last->command=commands::SET;
    last->next=nullptr;
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

    //Edditing a pixel on a image
    if(display!=nullptr){
        display->at<uint8_t>(current->y,current->x)=0;
    }


    //Log output
    if(logs!=nullptr){
        switch (current->command) {
        case commands::UP: logs->appendPlainText("UP"); break;
        case commands::DOWN: logs->appendPlainText("DOWN"); break;
        case commands::LEFT: logs->appendPlainText("LEFT"); break;
        case commands::RIGHT: logs->appendPlainText("RIGHT"); break;
        case commands::UPLEFT: logs->appendPlainText("UPLEFT"); break;
        case commands::UPRIGHT: logs->appendPlainText("UPRIGHT"); break;
        case commands::DOWNLEFT: logs->appendPlainText("DOWNLEFT"); break;
        case commands::DOWNRIGHT: logs->appendPlainText("DOWNRIGHT"); break;
        case commands::SET: logs->appendPlainText("SET "+QString::number(current->x)+' '+QString::number(current->y)); break;
        }
    }

    //Machine execute
    if(!simulation){
        switch (current->command) {
        case commands::UP : break;
        case commands::DOWN: break;
        case commands::LEFT: break;
        case commands::RIGHT: break;
        case commands::UPLEFT: break;
        case commands::UPRIGHT: break;
        case commands::DOWNLEFT: break;
        case commands::DOWNRIGHT: break;
        case commands::SET: break;

        }
    }
    current=current->next;
    return true;
}

void CommandContainer::setImageOutput(cv::Mat *image){
    display=image;
}
void CommandContainer::setLogOutput(QPlainTextEdit *console){
    logs=console;
}


void CommandContainer::setCurrent(int index){
    if(index>=numOfElemets)
        return;

    current=root;
    for(int i=0;i<index;i++){
        current=current->next;
    }
    return;
}


void CommandContainer::executeSet(int x,int y,bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y,x)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("SET "+QString::number(x)+' '+QString::number(y));
    }
    //execute
    if(!simulation){

    }
}

void CommandContainer::executeUP(int x_previous,int y_previous,bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y_previous-1,x_previous)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("UP");
    }
    //execute
    if(!simulation){

    }

}
void CommandContainer::executeDOWN(int x_previous,int y_previous, bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y_previous+1,x_previous)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("DOWN");
    }
    //execute
    if(!simulation){

    }
}
void CommandContainer::executeLEFT(int x_previous,int y_previous,bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y_previous,x_previous-1)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("LEFT");
    }
    //execute
    if(!simulation){

    }
}

void CommandContainer::executeRIGHT(int x_previous,int y_previous,bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y_previous,x_previous+1)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("RIGHT");
    }
    //execute
    if(!simulation){

    }
}

void CommandContainer::executeUPLEFT(int x_previous,int y_previous,bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y_previous-1,x_previous-1)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("UPLEFT");
    }
    //execute
    if(!simulation){

    }
}

void CommandContainer::executeDOWNLEFT(int x_previous,int y_previous,bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y_previous+1,x_previous-1)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("DOWNLEFT");
    }
    //execute
    if(!simulation){

    }
}
void CommandContainer::executeUPRIGHT(int x_previous,int y_previous,bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y_previous-1,x_previous+1)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("UPRIGHT");
    }
    //execute
    if(!simulation){

    }
}
void CommandContainer::executeDOWNRIGHT(int x_previous,int y_previous,bool simulation){
    //image privew
    if(display!=nullptr){
        display->at<uint8_t>(y_previous+1,x_previous+1)=0;
    }
    //logs output
    if(logs!=nullptr){
        logs->appendPlainText("DOWNRIGHT");
    }
    //execute
    if(!simulation){

    }
}
void CommandContainer::laser(bool on){

}





