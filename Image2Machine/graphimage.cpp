#include "graphimage.h"
#include <cstdint>
#include <fstream>

GraphImage::GraphImage()
{
    root= nullptr;
    end= nullptr;
}
GraphImage::~GraphImage(){
    deleteAll();
}

void GraphImage::deleteAll(){
    while(root!=nullptr){
        pixel* tmp=root;
        edge *ptr=root->link;
        while(ptr!=nullptr){
            edge* tptr=ptr;
            ptr=ptr->link;
            delete tptr;
        }
        root=root->next;
        delete tmp;
    }
    root=nullptr;
    end=nullptr;
}
bool GraphImage::insert(int x,int y,pixel* (GraphImage::*findfunction)(int,int)){
    if(root==nullptr){
      //case if there is not a single pixel in a graph
     root=end= new pixel;
     root->x=x;
     root->y=y;
     root->next=nullptr;
     root->last=nullptr;
     root->link=nullptr;
     root->status=0;
     return true;
    }
    else {
    end->next=new pixel;
    end->next->last=end;
    end=end->next;
    end->x=x;
    end->y=y;
    end->link=nullptr;
    end->next=nullptr;
    end->status=0;
    //connect with adjacent pixels
    for(int i=x-1;i<=x+1;i++){
        for(int j=y-1;j<=y+1;j++){
            if(i>=0&&j>=0&&!(i==x&&j==y)){
            pixel* tmp= (this->*findfunction)(i,j);
            if(tmp!=nullptr){
                edge* temp=new edge;
                temp->dest=tmp;
                temp->link=end->link;
                end->link=temp;
                temp=new edge;
                temp->dest=end;
                temp->link=tmp->link;
                tmp->link =temp;
            }
            }
        }
    }
    }
    return true;
}
//find functions
pixel* GraphImage::findCR(int x,int y){
    pixel* ret= end;
    while(ret!=nullptr && (ret->x>=x)){
        if(ret->x==x&&ret->y==y){
            return ret;
        }
    ret=ret->last;
    }
    return nullptr;
}
pixel* GraphImage::findRC(int x,int y){
    pixel* ret= end;
    while(ret!=nullptr && (ret->y>=y)){
        if(ret->x==x&&ret->y==y){
            return ret;
        }
    ret=ret->last;
    }
    return nullptr;
}
pixel* GraphImage::findNotConnected(int,int){
    return nullptr;
}


bool GraphImage::insertColsRows(cv::Mat const &image){
    if(image.empty())
        return false;
    if(root!=nullptr)
        deleteAll();
    for(int c=0;c<image.cols;c++){
        for(int r=0;r<image.rows;r++){
            if (image.at<uint8_t>(r,c)==0){
               if(!insert(c,r,&GraphImage::findCR))
                   return false;
            }
        }
    }
    return true;
}
bool GraphImage::insertRowsCols(cv::Mat const &image){
    if(image.empty())
        return false;
    if(root!=nullptr)
        deleteAll();
    for(int r=0;r<image.rows;r++){
        for(int c=0;c<image.cols;c++){
            if(image.at<uint8_t>(r,c)==0){
                if(!insert(c,r,&GraphImage::findRC))
                    return false;
            }
        }
    }
    return true;
}

bool GraphImage::test(QString const &filePath){
    std::ofstream f(filePath.toLocal8Bit().constData(),std::ios::out);
    if(!f){
        return false;
    }
    pixel *tmp=root;
    while(tmp!=nullptr){
        f<<tmp->x<<' '<<tmp->y<<"|";
        edge *temp=tmp->link;
        while(temp!=nullptr){
        f<<temp->dest->x<<' '<<temp->dest->y<<'|';
        temp=temp->link;
        }
        f<<std::endl;
        tmp=tmp->next;
    }
    f.close();
    return true;
}

void GraphImage::printCommand(pixel* atm, pixel*next,std::ofstream &f){
if(atm==nullptr||next==nullptr||!f)
    return;
//UP
if((next->y)==(atm->y+1)){
    //UPLEFT
    if((next->x)==(atm->x-1)){
        f<<(uint8_t)(commands::UPLEFT);
    }
    //UPRIGHT
    else if((next->x)==(atm->x+1)){
        f<<(uint8_t)(commands::UPRIGHT);
    }
    //UP
    else if((next->x)==(atm->x)){
        f<<(uint8_t)(commands::UP);
    }
    else{
        f<<(uint8_t)(commands::SET)<<next->x<<next->y;
    }
}
//DOWN
else if((next->y)==(atm->y-1)){
    //DOWNLEFT
    if((next->x)==(atm->x-1)){
        f<<(uint8_t)(commands::DOWNLEFT);
    }
    //DOWNRIGHT
    else if((next->x)==(atm->x+1)){
        f<<(uint8_t)(commands::DOWNRIGHT);
    }
    //DOWN
    else if((next->x)==(atm->x)){
        f<<(uint8_t)(commands::DOWN);
    }
    else{
        f<<(uint8_t)(commands::SET)<<next->x<<next->y;
    }
}
//SAME LEVEL
else if((next->y)==(atm->y)){
    //LEFT
    if((next->x)==(atm->x-1))
        f<<(uint8_t)(commands::LEFT);
    //RIGHT
    else if((next->x)==(atm->x+1)){
        f<<(uint8_t)(commands::RIGHT);
    }
    //SAME PIXEL
    else if((next->x)==(atm->x)){
        return;
    }
    else{
        f<<(uint8_t)(commands::SET)<<next->x<<next->y;
    }
}
else{
    f<<(uint8_t)(commands::SET)<<next->x<<next->y;
}
}


bool GraphImage::tooFileHeightWidth(QString const &path){
    std::ofstream f(path.toLocal8Bit().constData(),std::ios::out|std::ios::binary);
    if(!f)
        return false;
    pixel *tmp=root;
    while(tmp!=nullptr){
        printCommand(tmp,tmp->next,f);
        tmp=tmp->next;
    }
    f.close();
    return true;

}
