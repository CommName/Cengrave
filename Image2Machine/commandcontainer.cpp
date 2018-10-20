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
    height=0;
    width=0;
    engraveTime=1;
    speed=20;
    step=20;
    mode=1;


}
CommandContainer::CommandContainer(cv::Mat *image,QPlainTextEdit *log,int *x,int *y,HWF *h,Tmcl *tmcl):CommandContainer(){

    display=image;
    logs=log;
    displayX=x;
    displayY=y;
    tmclg=tmcl;
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
   width=0;
   height=9;

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
    if(last->x>width){
        width=last->x;
        display->create(height+10>100?height+10:100,width+10>100?width+10:100,CV_8U);
        *display = cv::Scalar(255);
    }
    if(last->y>height){
        height=last->y;
        display->create(height+10>100?height+10:100,width+10>100?width+10:100,CV_8U);
        *display = cv::Scalar(255);
    }
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
    if(last->x>width){
        width=x;
        display->create(height+10>100?height+10:100,width+10>100?width+10:100,CV_8U);
        *display = cv::Scalar(255);
    }
    if(last->y>height){
        height=last->y;
        display->create(height+10>100?height+10:100,width+10>100?width+10:100,CV_8U);
        *display = cv::Scalar(255);
    }
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
bool CommandContainer::loadGCode(QString const &path){
    std::ifstream f(path.toLocal8Bit().constData(),std::ios::in);
    if(!f)
        return false;
    if(root!=nullptr)
        deleteAll();
    //laser 0-off (m5) 1-on (m3,m4)
    //modes supported G01 and G91
    int mode=1,s=0,laser=0;
    float x=0,y=0;
    float x_next=0,y_next=0;

    while(!f.eof()){
        char command;
        command=f.get();
        switch(command){
        case 'M':
        case 'm':
            int laserMode;
            f>>laserMode;
            if(laserMode==5)
                laser=0;
            else if(laserMode==3||laserMode==4)
                laser=1;
            break;

        case 'G':
        case 'g':
            f>>mode;
            break;

        case 'S':
        case 's':
            f>>s;
            break;
        case 'X':
        case 'x':
            switch(mode){
            case 1: break;
            case 91: x_next+=x; break;
            default: break;
            }
            f>>x_next;
            break;
        case 'Y':
        case 'y':
            switch(mode){
            case 1: break;
            case 91: y_next+=y; break;
            default: break;
            }
            f>>y_next;
            break;
        case 'f':
        case 'F':
            f>>this->speed;
            break;
        case '\n':
            if(laser==0||s==0){
                if(x!=x_next||y!=y_next){
                switch(mode){
                case 1: this->insertSet(x_next*10/step,y_next*10/step); break;
                case 91: this->insertSet((x_next+x)*10/step,(y_next+y)*10/step); break;
                default: break;
                }
                x=x_next;
                y=y_next;
                }

            }
            else{

                while(x!=x_next || y!=y_next){
                    if(x<x_next-(float)(step)/10){
                        if(y<y_next-(float)(step)/10){
                            this->insert(commands::DOWNRIGHT);
                            y+=(float)(step)/10;
                        }
                        else if(y>y_next+(float)(step)/10){
                            this->insert(commands::UPRIGHT);
                            y-=(float)(step)/10;
                        }
                        else{
                            this->insert(commands::RIGHT);
                            y=y_next;
                        }
                        x+=(float)(step)/10;

                    }
                    else if(x>x_next+(float)(step)/10){
                        if(y<y_next-(float)(step)/10){
                            this->insert(commands::DOWNLEFT);
                            y+=(float)(step)/10;
                        }
                        else if(y>y_next+(float)(step)/10){
                            this->insert(commands::UPLEFT);
                            y-=(float)(step)/10;
                        }
                        else{
                            this->insert(commands::LEFT);
                            y=y_next;
                        }
                        x-=(float)(step)/10;
                    }
                    else{
                        x=x_next;
                        if(y<y_next-(float)(step)/10){
                            this->insert(commands::DOWN);
                            y+=(float)(step)/10;
                        }
                        else if(y>y_next+(float)(step)/10){
                            this->insert(commands::UP);
                            y-=(float)(step)/10;
                        }
                        else{
                            y=y_next;
                        }
                    }

                }

            }

        break;
        default:
            break;
        }

    }

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
    listView->clear();
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
                laser(false,simulation);
                this->executeSet(current->x,current->y,current->previous==nullptr?0:current->previous->x,current->previous==nullptr?0:current->previous->y,simulation);
                laser(true,simulation);
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
        workhorse(x-x_previous,y-y_previous);
    }
    displayAll(x,y,commands::SET);
}
void CommandContainer::executeUP(int x_previous,int y_previous,bool simulation){
    //execute
    if(!simulation){
        workhorse(0,-1);
    }
    displayAll(x_previous,y_previous-1,commands::UP);

}
void CommandContainer::executeDOWN(int x_previous,int y_previous, bool simulation){
    //execute
    if(!simulation){
        workhorse(0,1);

    }
    displayAll(x_previous,y_previous+1,commands::DOWN);
}
void CommandContainer::executeLEFT(int x_previous,int y_previous,bool simulation){

    //execute
    if(!simulation){
        workhorse(-1,0);
    }
    displayAll(x_previous-1,y_previous,commands::LEFT);
}
void CommandContainer::executeRIGHT(int x_previous,int y_previous,bool simulation){
    //execute
    if(!simulation){
        workhorse(1,0);
    }
    displayAll(x_previous+1,y_previous,commands::RIGHT);
}
void CommandContainer::executeUPLEFT(int x_previous,int y_previous,bool simulation){

    //execute
    if(!simulation){
        workhorse(-1,-1);

    }
    displayAll(x_previous-1,y_previous-1,commands::UPLEFT);
}
void CommandContainer::executeDOWNLEFT(int x_previous,int y_previous,bool simulation){

    //execute
    if(!simulation){
        workhorse(-1,1);
    }
    displayAll(x_previous-1,y_previous+1,commands::DOWNLEFT);
}
void CommandContainer::executeUPRIGHT(int x_previous,int y_previous,bool simulation){

    //execute
    if(!simulation){
        workhorse(1,-1);
    }
    displayAll(x_previous+1,y_previous-1,commands::UPRIGHT);
}
void CommandContainer::executeDOWNRIGHT(int x_previous,int y_previous,bool simulation){


    //execute
    if(!simulation){
        workhorse(1,1);
    }
    displayAll(x_previous+1,y_previous+1,commands::DOWNRIGHT);
}
void CommandContainer::laser(bool on,bool simulation){

    if(on){
        if(logs!=nullptr){
            logs->appendPlainText("Laser on");
        if(!simulation){
        switch(mode){
        case 0: tmclg->laserOn(true,maxStrength); break;
        default:
        case 1:hwf->motor_off(); break; //reversed logic
        }
        }
        }
    }
    else{
        if(logs!=nullptr){
            logs->appendPlainText("Laser off");
        }
        if(!simulation){
        switch(mode){
        case 0:tmclg->laserOn(false,0); break;
        default:
        case 1:hwf->motor_on(); break;//reversed logic
        }
        }
    }

}
void CommandContainer::connect(bool on){
    switch (mode) {
    case 0:
        if(on)
            tmclg->openSerialPortx();
        else
            tmclg->closeSerialPortx();
        break;
    case 1:
    default:
        if(on)
            hwf->enable_on();
        else
            hwf->enable_off();
        break;
    }
}

void CommandContainer::loadini(){
    QSettings settings("cengrave.ini",QSettings::IniFormat);

    //General
    settings.beginGroup("General");
    mode=settings.value("mode",0).toInt();
    settings.endGroup();

    settings.beginGroup("Movement");
    speed=settings.value("movement speed",20).toInt();
    step=settings.value("step",1).toInt();
    engraveTime=settings.value("engrave time",1).toInt();
    minStrenght=settings.value("min_strenght",0).toInt();
    maxStrength=settings.value("max_strength",255).toInt();
    settings.endGroup();
}

void CommandContainer::displayAll(int x,int y,commands com){
    //image privew
    if(display!=nullptr){
        if(y<0||x<0||y>=display->rows||x>=display->cols){
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

void CommandContainer::workhorse(int x,int y){
    switch(mode){
    case 0:
        tmclg->move(x*step*0.1,y*step*0.1,speed);
        break;

    case 1:
    default:
        hwf->set_vxy(speed);
        hwf->stepx(x*step*1000/hwf->get_korak_x(),1);
        hwf->stepy(y*step*1000/hwf->get_korak_y(),1);
        break;
    }
}

long CommandContainer::getIndex(){
    if(current!=nullptr)
        return current->index;
    else
        return 0;
}

void CommandContainer::displayPreview(cv::Mat *image){
    if(image->cols!=(width+10)&&image->rows!=(height+10)){
    image->create(height+10>100?height+10:100,width+10>100?width+10:100,CV_8U);
    *image = cv::Scalar(255);
    }
    container *tmp =root;
    while(tmp!=nullptr){
        if(image->at<uint8_t>(tmp->y,tmp->x)!=0)
            image->at<uint8_t>(tmp->y,tmp->x)=187;
        tmp=tmp->next;
    }

}
void CommandContainer::displayPreviewBGR(cv::Mat *image, uint8_t blue, uint8_t green,uint8_t red){
    if(image==nullptr)
        return;
    image->create(height+10,width+10,CV_8UC3);
    *image= cv::Vec3b(255,255,255);
    container *tmp=root;
    while(tmp!=nullptr){
        image->at<cv::Vec3b>(tmp->y,tmp->x)[0]=blue;
        image->at<cv::Vec3b>(tmp->y,tmp->x)[1]=green;
        image->at<cv::Vec3b>(tmp->y,tmp->x)[2]=red;
        tmp=tmp->next;
    }
}
