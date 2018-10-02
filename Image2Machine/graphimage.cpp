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
    setUpImporting(image.cols*image.rows);
    int status=0;
    for(int c=0;c<image.cols;c++){
        for(int r=0;r<image.rows;r++){
            if (image.at<uint8_t>(r,c)==0){
               if(!insert(c,r,&GraphImage::findCR))
                   return false;
            }
            updateImporting(status);
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
    int status=0;
    for(int c=0;c<image.cols;c++){
        for(int r=0;r<image.rows;r++){
            if(image.at<uint8_t>(r,c)==0){
                if(!insert(c,r,&GraphImage::findNotConnected)){
                    return false;
                }
            }
            updateImporting(status);
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
    int status=0;
    for(int r=0;r<image.rows;r++){
        for(int c=0;c<image.cols;c++){
            if(image.at<uint8_t>(r,c)==0){
                if(!insert(c,r,&GraphImage::findRC))
                    return false;
            }
            updateImporting(status);
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
    int status=0;
    setUpImporting(image.cols*image.rows);
    for(int r=0;r<image.rows;r++){
        for(int c=0;c<image.cols;c++){
            if(image.at<uint8_t>(r,c)==0){
                if(!insert(c,r,&GraphImage::findNotConnected))
                    return false;
            }
            updateImporting(status);
        }
    }
}
bool GraphImage::insertColsRowsZigZag(cv::Mat const &image){
    if(image.empty())
        return false;
    if(root!=nullptr)
        deleteAll();
    setUpImporting(image.cols*image.rows);
    int c=0,r=0,step=1;
    int status=0;
    while (c<image.cols) {
        //insert pixel
        if(image.at<uint8_t>(r,c)==0)
            if(!insert(c,r,&GraphImage::findCR))
                return false;
        updateImporting(status);
        //moving to next pixel
        r+=step;
        if(r<0||r==image.rows){
            step*=-1;
            r+=step;
            c++;
        }
    }
    return true;
}
bool GraphImage::insertColsRowsZigZagNotConnected(cv::Mat const &image){
if(image.empty())
    return false;
setUpImporting(image.cols*image.rows);
int status=0;
if(root!=nullptr)
    deleteAll();
int c=0,r=0,step=1;
while(c<image.cols){
    //insert pixel
    if(image.at<uint8_t>(r,c)==0)
        if(!insert(c,r,&GraphImage::findNotConnected))
            return false;
    updateImporting(status);
    //moving to next pixel
    r+=step;
    if(r<0||r==image.rows){
        step*=-1;
        r+=step;
        c++;
    }
}
return true;
}
bool GraphImage::insertRowsColsZigZag(cv::Mat const &image){
    if(image.empty())
        return false;
    setUpImporting(image.cols*image.rows);
    int status=0;
    if(root!=nullptr)
        deleteAll();
    int c=0,r=0,step=1;
    while(r<image.rows){
        //insert pixel
        if(image.at<uint8_t>(r,c)==0)
            if(!insert(c,r,&GraphImage::findRC))
                return false;
        updateImporting(status);
        c+=step;
        if(c<0||c==image.cols){
            step*=-1;
            c+=step;
            r++;
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
    int status=0;
    int c=0,r=0,step=-1;
    while(r<image.rows){
        if(image.at<uint8_t>(r,c)==0)
            if(!insert(c,r,&GraphImage::findNotConnected))
                return false;
        updateImporting(status);
        c+=step;
        if(c<0||c==image.cols){
            step*=-1;
            c+=step;
            r++;
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
    int status=0;
    //bottom half
    for(int i=0;i<image.cols;i++){
    c=i; r=image.rows-1;
    while(c>=0){
        if(r>=0&&r<image.rows)
        if(image.at<uint8_t>(r,c)==0)
               if(!insert(c,r,&GraphImage::findNotConnected))
                   return false;
        updateImporting(status);
        c--;
        r--;
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
        updateImporting(status);
        c--;
        r--;
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
    int status=0;
    //bottom half
    for(int i=0;i<image.cols;i++){
        if(i%2==0){
            c=i; r=image.rows-1;
            while(c>=0){
                if(r>=0&&r<image.rows)
                if(image.at<uint8_t>(r,c)==0)
                       if(!insert(c,r,&GraphImage::findNotConnected))
                           return false;
                updateImporting(status);
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
                updateImporting(status);
                c++;
                r++;
            }

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
            updateImporting(status);
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
            updateImporting(status);
            c++;
            r++;
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
if((next->y)==(atm->y-1)){
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
        f<<(uint8_t)(commands::SET)<<next->x<<'y'<<next->y;
    }
}
//DOWN
else if((next->y)==(atm->y+1)){
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
        f<<(uint8_t)(commands::SET)<<next->x<<'y'<<next->y;
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
        f<<(uint8_t)(commands::SET)<<next->x<<'y'<<next->y;
    }
}
else{
    f<<(uint8_t)(commands::SET)<<next->x<<'y'<<next->y;
}
}


bool GraphImage::tooFileHeightWidth(QString const &path){
    std::ofstream f(path.toLocal8Bit().constData(),std::ios::out|std::ios::binary);
    if(!f)
        return false;
    int status=0;
    setUpExporting();
    pixel *tmp=root;
    f<<(uint8_t)commands::SET<<tmp->x<<'y'<<tmp->y;
    while(tmp!=nullptr){
        printCommand(tmp,tmp->next,f);
        tmp=tmp->next;
        updateExporting(status);
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
    int status=0;
    setUpExporting();
    tmp=root;
    while(tmp!=nullptr){
        if(tmp->status==0){ //unprocessed pixel
            f<<(uint8_t)(commands::SET)<<tmp->x<<'y'<<tmp->y;
            updateExporting(status);
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
                            temp->dest->status=1;
                            //left has priority
                            if((temp->dest->x-1==branch->x)&&(temp->dest->y==branch->y))
                                    next=temp->dest;
                            else{
                                stack.push(temp->dest);
                            }
                        }
                        temp=temp->link;
                    }
                //chosing next pixel
                if(next==nullptr){
                    next=stack.pop();
                }
                printCommand(branch,next,f);
                updateExporting(status);
                branch=next;
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
    int status=0;
    setUpExporting();
    while(tmp!=nullptr){
        commandContainerInsert(tmp,tmp->next,con);
        updateExporting(status);
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
    int status=0;
    setUpExporting();
    while(tmp!=nullptr){
        if(tmp->status==0){ //unprocessed pixel
            con.insertSet(tmp->x,tmp->y);
            updateExporting(status);
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
                            temp->dest->status=1;
                            //left has priority
                            if((temp->dest->x-1==branch->x)&&(temp->dest->y==branch->y))
                                    next=temp->dest;
                            else{
                                stack.push(temp->dest);
                            }
                        }
                        temp=temp->link;
                    }
                //chosing next pixel
                if(next==nullptr){
                    next=stack.pop();
                }
                updateExporting(status);
                commandContainerInsert(branch,next,con);
                branch=next;
                }
            }
        }
        tmp=tmp->next;
    }
    return true;
}

void GraphImage::commandContainerInsert(pixel *atm,pixel *next,CommandContainer &com){
    if(atm==nullptr||next==nullptr)
        return;
    //UP
    if((next->y)==(atm->y-1)){
        //UPLEFT
        if((next->x)==(atm->x-1)){
            com.insert(commands::UPLEFT);
        }
        //UPRIGHT
        else if((next->x)==(atm->x+1)){
            com.insert(commands::UPRIGHT);
        }
        //UP
        else if((next->x)==(atm->x)){
            com.insert(commands::UP);
        }
        else{
            com.insertSet(next->x,next->y);
        }
    }
    //DOWN
    else if((next->y)==(atm->y+1)){
        //DOWNLEFT
        if((next->x)==(atm->x-1)){
            com.insert(commands::DOWNLEFT);
        }
        //DOWNRIGHT
        else if((next->x)==(atm->x+1)){
            com.insert(commands::DOWNRIGHT);
        }
        //DOWN
        else if((next->x)==(atm->x)){
            com.insert(commands::DOWN);
        }
        else{
            com.insertSet(next->x,next->y);
        }
    }
    //SAME LEVEL
    else if((next->y)==(atm->y)){
        //LEFT
        if((next->x)==(atm->x-1))
            com.insert(commands::LEFT);
        //RIGHT
        else if((next->x)==(atm->x+1)){
            com.insert(commands::RIGHT);
        }
        //SAME PIXEL
        else if((next->x)==(atm->x)){
            return;
        }
        else{
            com.insertSet(next->x,next->y);
        }
    }
    else{
        com.insertSet(next->x,next->y);
    }
}

void GraphImage::setUpImporting(int maxNumber){
    if(importing!=nullptr){
       importing->setMaximum(maxNumber);
    }
}
void GraphImage::setUpExporting(){
    if(exporting!=nullptr){
        exporting->setMaximum(numOfElements);
    }
}
void GraphImage::updateImporting(int &status){
    status++;
    if(importing!=nullptr){
        importing->setValue(status);
        QApplication::processEvents();
    }
}
void GraphImage::updateExporting(int &status){
    status++;
    if(exporting!=nullptr){
        exporting->setValue(status);
        QApplication::processEvents();
    }
}
