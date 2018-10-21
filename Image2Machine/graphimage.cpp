#include "graphimage.h"
#include <cstdint>
#include <fstream>
#include <QStack>
#include <QApplication>

GraphImage::GraphImage()
{
    root= nullptr;
    end= nullptr;
    importing=nullptr;
    exporting=nullptr;
    numOfElements=0;
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
    numOfElements=0;
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
    numOfElements++;
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
    stop=false;
    setUpImporting(image.cols*image.rows);
    for(int c=0;c<image.cols;c++){
        for(int r=0;r<image.rows;r++){
            if (image.at<uint8_t>(r,c)==0){
               if(!insert(c,r,&GraphImage::findCR))
                   return false;
            }
            updateImporting();
            if(stop){
                return false;
            }
        }
    }
    return true;
}
bool GraphImage::insertColsRowsNotConnected(cv::Mat const &image){
    if(image.empty())
        return false;
    if(root!=nullptr)
        deleteAll();
    setUpImporting(image.cols*image.rows);
    stop=false;
    for(int c=0;c<image.cols;c++){
        for(int r=0;r<image.rows;r++){
            if(image.at<uint8_t>(r,c)==0){
                if(!insert(c,r,&GraphImage::findNotConnected)){
                    return false;
                }
            }
            updateImporting();
            if(stop){
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
    setUpImporting(image.cols*image.rows);
    stop=false;
    for(int r=0;r<image.rows;r++){
        for(int c=0;c<image.cols;c++){
            if(image.at<uint8_t>(r,c)==0){
                if(!insert(c,r,&GraphImage::findRC))
                    return false;
            }
            updateImporting();
            if(stop){
                return false;
            }
        }
    }
    return true;
}
bool GraphImage::insertRowsColsNotConnected(cv::Mat const &image){
    if(image.empty()){
        return false;
    }

    if(root!=nullptr)
        deleteAll();
    setUpImporting(image.cols*image.rows);
    stop=false;
    for(int r=0;r<image.rows;r++){
        for(int c=0;c<image.cols;c++){
            if(image.at<uint8_t>(r,c)==0){
                if(!insert(c,r,&GraphImage::findNotConnected))
                    return false;
            }
            updateImporting();
            if(stop){
                return false;
            }
        }
    }
    return true;
}
bool GraphImage::insertColsRowsZigZag(cv::Mat const &image){
    if(image.empty())
        return false;
    if(root!=nullptr)
        deleteAll();
    setUpImporting(image.cols*image.rows);
    stop=false;
    int c=0,r=0,step=1;
    while (c<image.cols) {
        //insert pixel
        if(image.at<uint8_t>(r,c)==0)
            if(!insert(c,r,&GraphImage::findCR))
                return false;
        updateImporting();
        //moving to next pixel
        r+=step;
        if(r<0||r==image.rows){
            step*=-1;
            r+=step;
            c++;
        }
        if(stop){
            return false;
        }
    }
    return true;
}
bool GraphImage::insertColsRowsZigZagNotConnected(cv::Mat const &image){
if(image.empty())
    return false;
setUpImporting(image.cols*image.rows);
if(root!=nullptr)
    deleteAll();
int c=0,r=0,step=1;
stop=false;
while(c<image.cols){
    //insert pixel
    if(image.at<uint8_t>(r,c)==0)
        if(!insert(c,r,&GraphImage::findNotConnected))
            return false;
    updateImporting();
    //moving to next pixel
    r+=step;
    if(r<0||r==image.rows){
        step*=-1;
        r+=step;
        c++;
    }
    if(stop){
        return false;
    }
}
return true;
}
bool GraphImage::insertRowsColsZigZag(cv::Mat const &image){
    if(image.empty())
        return false;
    setUpImporting(image.cols*image.rows);
    if(root!=nullptr)
        deleteAll();
    stop=false;
    int c=0,r=0,step=1;
    while(r<image.rows){
        //insert pixel
        if(image.at<uint8_t>(r,c)==0)
            if(!insert(c,r,&GraphImage::findRC))
                return false;
        updateImporting();
        c+=step;
        if(c<0||c==image.cols){
            step*=-1;
            c+=step;
            r++;
        }
        if(stop){
            return false;
        }
    }
    return true;
}
bool GraphImage::insertRowsColsZigZagNotConnected(cv::Mat const &image){
    if(image.empty())
        return false;
    setUpImporting(image.cols*image.rows);
    if(root!=nullptr){
        deleteAll();
    }
    stop=false;
    int c=0,r=0,step=-1;
    while(r<image.rows){
        if(image.at<uint8_t>(r,c)==0)
            if(!insert(c,r,&GraphImage::findNotConnected))
                return false;
        updateImporting();
        c+=step;
        if(c<0||c==image.cols){
            step*=-1;
            c+=step;
            r++;
        }
        if(stop){
            return false;
        }
    }
    return true;
}
bool GraphImage::insertDiagonal(cv::Mat const &image){
    if(image.empty())
        return false;
    setUpImporting(image.cols*image.rows);
    if(root!=nullptr)
        deleteAll();
    int c=0,r=image.rows;
    stop=false;
    //bottom half
    for(int i=0;i<image.cols;i++){
    c=i; r=image.rows-1;
    while(c>=0){
        if(r>=0&&r<image.rows)
        if(image.at<uint8_t>(r,c)==0)
               if(!insert(c,r,&GraphImage::findNotConnected))
                   return false;
        updateImporting();
        c--;
        r--;
        if(stop){
            return false;
        }
    }
    }
    //upper half
    for(int i=2;i<=image.rows;i++){
    c=image.cols-1; r=image.rows-i;
    while(c>=0){
        if(r>=0&&r<image.rows)
        if(image.at<uint8_t>(r,c)==0)
               if(!insert(c,r,&GraphImage::findNotConnected))
                   return false;
        updateImporting();
        c--;
        r--;
        if(stop){
            return false;
        }
    }
    }

    return true;
}

bool GraphImage::insertDiagonalZigZag(cv::Mat const &image){
    if(image.empty())
        return false;
    setUpImporting(image.cols*image.rows);
    if(root!=nullptr)
        deleteAll();
    int c=0,r=image.rows;
    stop=false;
    //bottom half
    for(int i=0;i<image.cols;i++){
        if(i%2==0){
            c=i; r=image.rows-1;
            while(c>=0){
                if(r>=0&&r<image.rows)
                if(image.at<uint8_t>(r,c)==0)
                       if(!insert(c,r,&GraphImage::findNotConnected))
                           return false;
                updateImporting();
                c--;
                r--;
            }
        }
        else{
            c=0;r=image.rows-i-1;
            while(c<image.cols){
                if(r>=0&&r<image.rows)
                if(image.at<uint8_t>(r,c)==0)
                       if(!insert(c,r,&GraphImage::findNotConnected))
                           return false;
                updateImporting();
                c++;
                r++;
            }

        }
        if(stop){
            return false;
        }
    }
    //upper half
    for(int i=1;i<=image.rows;i++){
    if(i%2==image.cols%2){
        c=image.cols-1; r=image.rows-i-1;
        while(c>=0){
            if(r>=0&&r<image.rows)
            if(image.at<uint8_t>(r,c)==0)
                   if(!insert(c,r,&GraphImage::findNotConnected))
                       return false;
            updateImporting();
            c--;
            r--;
        }
    }
    else{
        c=0; r=image.rows-i-image.cols;
        while(c<image.cols){
            if(r>=0&&r<image.rows)
            if(image.at<uint8_t>(r,c)==0)
                   if(!insert(c,r,&GraphImage::findNotConnected))
                       return false;
            updateImporting();
            c++;
            r++;
        }
    }
    if(stop){
        return false;
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
        if(stop)
            return false;
    }
    f.close();
    return true;
}

void GraphImage::printCommand(pixel* atm, pixel*next,std::ofstream &f,commands *lastCommand){
    if(atm==nullptr||next==nullptr||!f)
        return;
//UP
if((next->y)==(atm->y-1)){
    //UPLEFT
    if((next->x)==(atm->x-1)){
        f<<(uint8_t)(commands::UPLEFT);
        if(lastCommand!=nullptr)
            *lastCommand=commands::UPLEFT;
    }
    //UPRIGHT
    else if((next->x)==(atm->x+1)){
        f<<(uint8_t)(commands::UPRIGHT);
        if(lastCommand!=nullptr)
            *lastCommand=commands::UPRIGHT;
    }
    //UP
    else if((next->x)==(atm->x)){
        f<<(uint8_t)(commands::UP);
        if(lastCommand!=nullptr)
            *lastCommand=commands::UP;
    }
    else{
        f<<(uint8_t)(commands::SET)<<next->x<<'y'<<next->y;
        if(lastCommand!=nullptr)
            *lastCommand=commands::SET;
    }
}
//DOWN
else if((next->y)==(atm->y+1)){
    //DOWNLEFT
    if((next->x)==(atm->x-1)){
        f<<(uint8_t)(commands::DOWNLEFT);
        if(lastCommand!=nullptr)
            *lastCommand=commands::DOWNLEFT;
    }
    //DOWNRIGHT
    else if((next->x)==(atm->x+1)){
        f<<(uint8_t)(commands::DOWNRIGHT);
        if(lastCommand!=nullptr)
            *lastCommand=commands::DOWNRIGHT;
    }
    //DOWN
    else if((next->x)==(atm->x)){
        f<<(uint8_t)(commands::DOWN);
        if(lastCommand!=nullptr)
            *lastCommand=commands::DOWN;
    }
    else{
        f<<(uint8_t)(commands::SET)<<next->x<<'y'<<next->y;
        if(lastCommand!=nullptr)
            *lastCommand=commands::SET;
    }
}
//SAME LEVEL
else if((next->y)==(atm->y)){
    //LEFT
    if((next->x)==(atm->x-1)){
        f<<(uint8_t)(commands::LEFT);
    if(lastCommand!=nullptr)
        *lastCommand=commands::LEFT;
    }
    //RIGHT
    else if((next->x)==(atm->x+1)){
        f<<(uint8_t)(commands::RIGHT);
        if(lastCommand!=nullptr)
            *lastCommand=commands::RIGHT;
    }
    //SAME PIXEL
    else if((next->x)==(atm->x)){
        return;
    }
    else{
        f<<(uint8_t)(commands::SET)<<next->x<<'y'<<next->y;
        if(lastCommand!=nullptr)
            *lastCommand=commands::UPLEFT;
    }
}
else{
    f<<(uint8_t)(commands::SET)<<next->x<<'y'<<next->y;
    if(lastCommand!=nullptr)
        *lastCommand=commands::SET;
}
}


bool GraphImage::tooFileHeightWidth(QString const &path){
    std::ofstream f(path.toLocal8Bit().constData(),std::ios::out|std::ios::binary);
    if(!f)
        return false;
    setUpExporting();
    stop=false;
    pixel *tmp=root;
    f<<(uint8_t)commands::SET<<tmp->x<<'y'<<tmp->y;
    while(tmp!=nullptr){
        printCommand(tmp,tmp->next,f);
        tmp=tmp->next;
        updateExporting();
        if(stop){
            return false;
        }
    }
    f.close();
    return true;

}
bool GraphImage::tooFileDepth2(QString const &path){
    std::ofstream f(path.toLocal8Bit().constData(),std::ios::out|std::ios::binary);
    if(!f)
        return false;
    pixel *temp=root;
    while(temp!=nullptr){
        temp->status=0;
        temp=temp->next;
    }
    stop=false;
    temp=root;
    setUpExporting();
    while(temp!=nullptr){
        if(temp->status==0){
            commands com=commands::SET;
            pixel*prev=nullptr;
            f<<(uint8_t)(commands::SET)<<temp->x<<'y'<<temp->y;
            updateExporting();
            if(!pixelDepthFile(prev,temp,&com,f)){
                return false;
            }
        }
        if(stop){
            return false;
        }
        temp=temp->next;
    }
    f.close();
    return true;
}
bool GraphImage::tooFileDepth(QString const &path){
    std::ofstream f(path.toLocal8Bit().constData(),std::ios::out|std::ios::binary);
    if(!f)
        return false;
    pixel* tmp=root;
    //seting status to 0
    while(tmp!=nullptr){
        tmp->status=0;
        tmp=tmp->next;
    }
    stop=false;
    commands lastCommand=commands::SET;
    setUpExporting();
    tmp=root;
    while(tmp!=nullptr){
        if(tmp->status==0){ //unprocessed pixel
            f<<(uint8_t)(commands::SET)<<tmp->x<<'y'<<tmp->y;
            lastCommand=commands::SET;
            if(stop){
                return false;
            }
            updateExporting();
            pixel* branch=tmp;
            QStack<pixel*> stack;
            stack.push(nullptr);
            branch->status=1; //mark it is in queue for processing
            while(branch!=nullptr){
                pixel* next=nullptr;
                if(branch->status!=2){//proccessing pixel
                    branch->status=2;
                    //adding adjusted to queue for processing
                    edge *temp=branch->link;
                    while(temp!=nullptr){
                        if(temp->dest->status==0){
                            switch(lastCommand){
                            case commands::UP:
                                if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y))
                                    next=temp->dest;
                                else
                                    stack.push(temp->dest);
                                break;
                            case commands::DOWN:
                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y))
                                    next=temp->dest;
                                else
                                    stack.push(temp->dest);
                                break;
                            case commands::LEFT:
                                if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y))
                                    next=temp->dest;
                                else
                                    stack.push(temp->dest);
                                break;
                            case commands::RIGHT:
                                if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y))
                                    next=temp->dest;
                                else
                                    stack.push(temp->dest);
                                break;
                            case commands::UPLEFT:
                                if((temp->dest->x+1)==(branch->x)&&(temp->dest->y+1)==(branch->y))
                                    next=temp->dest;
                                else
                                    stack.push(temp->dest);
                                break;
                            case commands::UPRIGHT:
                                if((temp->dest->x-1)==(branch->x)&&(temp->dest->y+1)==(branch->y))
                                    next=temp->dest;
                                else
                                    stack.push(temp->dest);
                                break;
                            case commands::DOWNLEFT:
                                if((temp->dest->x+1)==(branch->x)&&(temp->dest->y-1)==(branch->y))
                                    next=temp->dest;
                                else
                                    stack.push(temp->dest);
                                break;
                            case commands::DOWNRIGHT:
                                if((temp->dest->x-1)==(branch->x)&&(temp->dest->y-1)==(branch->y))
                                    next=temp->dest;
                                else
                                    stack.push(temp->dest);
                                break;
                            case commands::SET:
                                if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)) //LEFT has priority
                                    next=temp->dest;
                                else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)) //RIGHT has priority
                                        next=temp->dest;
                                    else
                                    stack.push(temp->dest);
                                break;
                            }
                            temp->dest->status=1;
                        }
                        temp=temp->link;
                    }
                }
                //chosing next pixel
                if(next==nullptr){
                    next=stack.pop();
                }
                printCommand(branch,next,f,&lastCommand);
                updateExporting();
                branch=next;
                if(stop){
                    return false;
                }
            }
        }
        tmp=tmp->next;
    }
    f.close();
    return true;
}

bool GraphImage::tooCommandContainerHeightWidth(CommandContainer &con){
    con.deleteAll();
    pixel *tmp=root;
    con.insertSet(root->x,root->y);
    stop=false;
    setUpExporting();
    while(tmp!=nullptr){
        commandContainerInsert(tmp,tmp->next,con);
        updateExporting();
        if(stop){
            return false;
        }
        tmp=tmp->next;
    }
    return true;
}
bool GraphImage::tooCommandContainerDepth(CommandContainer &con){
    con.deleteAll();
    pixel* tmp=root;
    //seting status to 0
    while(tmp!=nullptr){
        tmp->status=0;
        tmp=tmp->next;
    }
    tmp=root;
    stop=false;
    setUpExporting();
    commands lastCommand=commands::SET;
    while(tmp!=nullptr){
        if(tmp->status==0){ //unprocessed pixel
            con.insertSet(tmp->x,tmp->y);
            lastCommand=commands::SET;
            if(stop){
                return false;
            }
            updateExporting();
            pixel* branch=tmp;
            QStack<pixel*> stack;
            stack.push(nullptr);
            branch->status=1; //mark it is in queue for processing
            while(branch!=nullptr){
                pixel* next=nullptr;
                if(branch->status!=2){//proccessing pixel
                    branch->status=2;
                    //adding adjusted to queue for processing
                    edge *temp=branch->link;
                    while(temp!=nullptr){
                        if(temp->dest->status==0){
                            switch(lastCommand){
                            case commands::UP:
                                if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)){
                                    if(next!=nullptr){
                                        stack.push(next);
                                    }
                                    next=temp->dest;
                                }
                                else
                                    if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //LEFT has priority
                                        next=temp->dest;
                                    }
                                    else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                            next=temp->dest;
                                    }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                                next=temp->dest;
                                            }
                                            else
                                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                    next=temp->dest;
                                                }
                                                else{
                                                    stack.push(temp->dest);
                                                }
                                break;
                            case commands::DOWN:
                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)){
                                    if(next!=nullptr){
                                        stack.push(next);
                                    }
                                    next=temp->dest;
                               }
                                else
                                    if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //LEFT has priority
                                        next=temp->dest;
                                    }
                                    else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                            next=temp->dest;
                                    }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                                next=temp->dest;
                                            }
                                            else
                                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                    next=temp->dest;
                                                }
                                                else{
                                                    stack.push(temp->dest);
                                                }
                                break;
                            case commands::LEFT:
                                if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)){
                                    if(next!=nullptr){
                                        stack.push(next);
                                    }
                                    next=temp->dest;
                                }
                                else
                                    if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //LEFT has priority
                                        next=temp->dest;
                                    }
                                    else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                            next=temp->dest;
                                    }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                                next=temp->dest;
                                            }
                                            else
                                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                    next=temp->dest;
                                                }
                                                else{
                                                    stack.push(temp->dest);
                                                }
                                break;
                            case commands::RIGHT:
                                if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)){
                                    if(next!=nullptr){
                                        stack.push(next);
                                    }
                                    next=temp->dest;
                                }
                                else
                                    if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //LEFT has priority
                                        next=temp->dest;
                                    }
                                    else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                            next=temp->dest;
                                    }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                                next=temp->dest;
                                            }
                                            else
                                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                    next=temp->dest;
                                                }
                                                else{
                                                    stack.push(temp->dest);
                                                }
                                break;
                            case commands::UPLEFT:
                                if((temp->dest->x+1)==(branch->x)&&(temp->dest->y+1)==(branch->y)){
                                    if(next!=nullptr){
                                        stack.push(next);
                                    }
                                    next=temp->dest;
                                }
                                else
                                    if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //LEFT has priority
                                        next=temp->dest;
                                    }
                                    else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                            next=temp->dest;
                                    }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                                next=temp->dest;
                                            }
                                            else
                                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                    next=temp->dest;
                                                }
                                                else{
                                                    stack.push(temp->dest);
                                                }
                                break;
                            case commands::UPRIGHT:
                                if((temp->dest->x-1)==(branch->x)&&(temp->dest->y+1)==(branch->y)){
                                    if(next!=nullptr){
                                        stack.push(next);
                                    }
                                    next=temp->dest;
                                }
                                else
                                    if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //LEFT has priority
                                        next=temp->dest;
                                    }
                                    else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                            next=temp->dest;
                                    }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                                next=temp->dest;
                                            }
                                            else
                                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                    next=temp->dest;
                                                }
                                                else{
                                                    stack.push(temp->dest);
                                                }
                                break;
                            case commands::DOWNLEFT:
                                if((temp->dest->x+1)==(branch->x)&&(temp->dest->y-1)==(branch->y)){
                                    if(next!=nullptr){
                                        stack.push(next);
                                    }
                                    next=temp->dest;
                                }
                                else
                                    if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //LEFT has priority
                                        next=temp->dest;
                                    }
                                    else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                            next=temp->dest;
                                    }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                                next=temp->dest;
                                            }
                                            else
                                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                    next=temp->dest;
                                                }
                                                else{
                                                    stack.push(temp->dest);
                                                }
                                break;
                            case commands::DOWNRIGHT:
                                if((temp->dest->x-1)==(branch->x)&&(temp->dest->y-1)==(branch->y)){
                                    if(next!=nullptr){
                                        stack.push(next);
                                    }
                                    next=temp->dest;
                                }
                                else
                                    if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //LEFT has priority
                                        next=temp->dest;
                                    }
                                    else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                            next=temp->dest;
                                    }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                                next=temp->dest;
                                            }
                                            else
                                                if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                    next=temp->dest;
                                                }
                                                else{
                                                    stack.push(temp->dest);
                                                }
                                break;
                            case commands::SET:
                                if((temp->dest->x+1)==(branch->x)&&(temp->dest->y)==(branch->y)){ //LEFT has priority
                                    if(next!=nullptr){
                                        stack.push(next);
                                }
                                    next=temp->dest;
                                }
                                else if((temp->dest->x-1)==(branch->x)&&(temp->dest->y)==(branch->y)&&(next==nullptr)){ //RIGHT has priority
                                        next=temp->dest;
                                }
                                    else
                                        if((temp->dest->x)==(branch->x)&&(temp->dest->y+1)==(branch->y)&&(next==nullptr)){ //UP has priority
                                            next=temp->dest;
                                        }
                                        else
                                            if((temp->dest->x)==(branch->x)&&(temp->dest->y-1)==(branch->y)&&(next==nullptr)){ //DOWN has priority
                                                next=temp->dest;
                                            }
                                            else{
                                                stack.push(temp->dest);
                                            }
                                break;
                            }
                            temp->dest->status=1;
                        }
                        temp=temp->link;
                    }
                }
                //chosing next pixel
                if(next==nullptr){
                    next=stack.pop();
                }
                updateExporting();
                commandContainerInsert(branch,next,con,&lastCommand);
                branch=next;
                if(stop){
                    return false;
                }
            }
        }
        tmp=tmp->next;
    }
    return true;
}
bool GraphImage::tooCommandContainerDepth2(CommandContainer &con){
    pixel *temp=root;
    while(temp!=nullptr){
        temp->status=0;
        temp=temp->next;
    }
    stop=false;
    temp=root;
    setUpExporting();
    while(temp!=nullptr){
        if(temp->status==0){
            commands com=commands::SET;
            pixel*prev=nullptr;
            con.insertSet(temp->x,temp->y);
            updateExporting();
            if(!pixelDepthCommandContainer(prev,temp,&com,con)){
                return false;
            }
        }
        if(stop){
            return false;
        }
        temp=temp->next;
    }
    return true;
}
bool GraphImage::pixelDepthCommandContainer(pixel *prev,pixel *next,commands *command,CommandContainer &con){
    if(next!=nullptr&&next->status==0){
        next->status=1;

        edge* temp=next->link;
        pixel* n=nullptr;
        prev=next;
        QStack<pixel*> stack;
        while(temp!=nullptr){
            if(temp->dest->status==0){
            if(n==nullptr){
                n=temp->dest;
            }
            else
                stack.push(temp->dest);
           }
            if(stop){
                return false;
            }
            temp=temp->link;
        }

        if(n!=nullptr){
        commandContainerInsert(next,n,con,command);
        updateExporting();
        if(!pixelDepthCommandContainer(next,n,command,con))
                return false;
        }

        while(!stack.empty()){
            pixel* t = stack.pop();
            if(t->status==0){
            //commandContainerInsert(prev,t,con,command);
            con.insertSet(t->x,t->y);
            updateExporting();
            if(pixelDepthCommandContainer(nullptr,t,command,con))
                return false;
            }
            if(stop){
                return false;
            }
        }

    }
    return true;
}
bool GraphImage::pixelDepthFile(pixel *prev,pixel *next,commands *LastCommand,std::ofstream &f){
    if(next!=nullptr&&next->status==0){
        next->status=1;

        edge* temp=next->link;
        pixel* n=nullptr;
        prev=next;
        QStack<pixel*> stack;
        while(temp!=nullptr){
            if(temp->dest->status==0){
            if(n==nullptr){
                n=temp->dest;
            }
            else
                stack.push(temp->dest);
           }
            if(stop){
                return false;
            }
            temp=temp->link;
        }

        if(n!=nullptr){
        printCommand(next,n,f,LastCommand);
        updateExporting();
        if(!pixelDepthFile(nullptr,n,LastCommand,f))
                return false;
        }

        while(!stack.empty()){
            pixel* t = stack.pop();
            if(t->status==0){
            f<<(uint8_t)(commands::SET)<<t->x<<'y'<<t->y;
            updateExporting();
            if(!pixelDepthFile(nullptr,t,LastCommand,f))
                return false;
            }
            if(stop){
                return false;
            }
        }

    }
    return true;
}

void GraphImage::commandContainerInsert(pixel *atm,pixel *next,CommandContainer &com,commands *lastCommand){
    if(atm==nullptr||next==nullptr)
        return;
    //UP
    if((next->y)==(atm->y-1)){
        //UPLEFT
        if((next->x)==(atm->x-1)){
            com.insert(commands::UPLEFT);
            if(lastCommand!=nullptr)
                *lastCommand=commands::UPLEFT;
        }
        //UPRIGHT
        else if((next->x)==(atm->x+1)){
            com.insert(commands::UPRIGHT);
            if(lastCommand!=nullptr)
                *lastCommand=commands::UPRIGHT;
        }
        //UP
        else if((next->x)==(atm->x)){
            com.insert(commands::UP);
            if(lastCommand!=nullptr)
                *lastCommand=commands::UP;
        }
        else{
            com.insertSet(next->x,next->y);
            if(lastCommand!=nullptr)
                *lastCommand=commands::SET;
        }
    }
    //DOWN
    else if((next->y)==(atm->y+1)){
        //DOWNLEFT
        if((next->x)==(atm->x-1)){
            com.insert(commands::DOWNLEFT);
            if(lastCommand!=nullptr)
                *lastCommand=commands::DOWNLEFT;
        }
        //DOWNRIGHT
        else if((next->x)==(atm->x+1)){
            com.insert(commands::DOWNRIGHT);
            if(lastCommand!=nullptr)
                *lastCommand=commands::DOWNRIGHT;
        }
        //DOWN
        else if((next->x)==(atm->x)){
            com.insert(commands::DOWN);
            if(lastCommand!=nullptr)
                *lastCommand=commands::DOWN;
        }
        else{
            com.insertSet(next->x,next->y);
            if(lastCommand!=nullptr)
                *lastCommand=commands::SET;
        }
    }
    //SAME LEVEL
    else if((next->y)==(atm->y)){
        //LEFT
        if((next->x)==(atm->x-1)){
            com.insert(commands::LEFT);
        if(lastCommand!=nullptr)
            *lastCommand=commands::LEFT;
        }
        //RIGHT
        else if((next->x)==(atm->x+1)){
            com.insert(commands::RIGHT);
            if(lastCommand!=nullptr)
                *lastCommand=commands::RIGHT;
        }
        //SAME PIXEL
        else if((next->x)==(atm->x)){
            return;
        }
        else{
            com.insertSet(next->x,next->y);
            if(lastCommand!=nullptr)
                *lastCommand=commands::SET;
        }
    }
    else{
        com.insertSet(next->x,next->y);
        if(lastCommand!=nullptr)
            *lastCommand=commands::SET;
    }
}

void GraphImage::setUpImporting(int maxNumber){
    if(importing!=nullptr){
       importing->setMaximum(maxNumber);
       importing->setValue(0);
    }
}
void GraphImage::setUpExporting(){
    if(exporting!=nullptr){
        exporting->setMaximum(numOfElements);
        exporting->setValue(0);
    }
}
void GraphImage::updateImporting(){
    if(importing!=nullptr){
        importing->setValue(importing->value()+1);
        QApplication::processEvents();
    }
}
void GraphImage::updateExporting(){
    if(exporting!=nullptr){
        exporting->setValue(exporting->value()+1);
        QApplication::processEvents();
    }
}
