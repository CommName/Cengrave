#include "commandcontainer.h"
#include <fstream>
#include <cstdint>
#include <iostream>
CommandContainer::CommandContainer()
{
    root=nullptr;
    last=nullptr;
    current=nullptr;

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
        case commands::SET: listView->addItem("SET"+QString::number(temp->x)+' '+QString::number(temp->y)); break;
        deault: break;
        }

        temp=temp->next;
    }
}

















