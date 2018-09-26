
#ifndef COMMANDCONTAINER_H
#define COMMANDCONTAINER_H
#include <QString>
#include <QListWidget>
#include <opencv2/opencv.hpp>
#include <QPlainTextEdit>
#include "Image2Machine/hwf.h"
#include <QProgressBar>
#include "tmcl.h"
class mainwindow;

enum commands{
    UP=0,
    LEFT,
    UPLEFT,
    UPRIGHT,
    SET,
    DOWNLEFT,
    DOWNRIGHT,
    RIGHT,
    DOWN

};
struct container{
    commands command;
    int x;
    int y;
    container* next;
    container* previous;
    long index;

};

class CommandContainer
{
private:
    container* root;
    container* last;
    container* current;
    long numOfElemets;

protected:
    int mode;
    HWF *hwf;
    Tmcl *tmclg;

private:
    long speed;
    int step;
    int engraveTime;

protected:
    bool secure;

protected:
    cv::Mat *display;
    QPlainTextEdit *logs;
    QProgressBar *progressbar;
    int *displayX;
    int *displayY;

public:

    CommandContainer();
    CommandContainer(cv::Mat *image,QPlainTextEdit *log,int *x,int *y,HWF *h,Tmcl *tmcl);
    ~CommandContainer();
protected:
    void displayAll(int x,int y, commands com);


public:
    void setCurrent(int index);
    void setImageOutput(cv::Mat *image);
    void inline setLogOutput(QPlainTextEdit *console){logs=console;}
    void inline setProgressBar(QProgressBar *bar){progressbar=bar;}

    bool execute(bool simulation);
    bool insert(commands command);
    bool insertSet(int x,int y);
    //bool insertAfterCurrent(commands command);
    //bool insertAfterCurrentSet(int x,int y);
    //bool insertBeforeCurrent(commands command);
    //bool insertBeforeCurrentSet(int x,int y);
    //void deleteCommand(int index);
    void deleteAll();
    bool loadFile(QString const &path);
    bool saveFile(QString const &path);
    void printToQListView(QListWidget *listView);
    void loadini();


    int  inline getStep(){return step;}
    void inline setHWF(HWF *h){hwf=h;}
    long getIndex();

    //manual controls
private:
    void workhorse(int x,int y);
public:
    void executeSet(int x,int y,int x_previous,int y_previous,bool simulation);
    void executeUP(int x_previous,int y_previous,bool simulation);
    void executeDOWN(int x_previous,int y_previous,bool simulation);
    void executeLEFT(int x_previous,int y_previous ,bool simulation);
    void executeRIGHT(int x_previous,int y_previous,bool simulation);
    void executeUPLEFT(int x_previous,int y_previous,bool simulation);
    void executeUPRIGHT(int x_previous,int y_previous,bool simulation);
    void executeDOWNLEFT(int x_previous,int y_previous,bool simulation);
    void executeDOWNRIGHT(int x_previous,int y_previous,bool simulation);
    void laser(bool on);
    void connect(bool on);
};

#endif // COMMANDCONTAINER_H
