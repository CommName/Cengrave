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
class commandpreview;

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
    friend class commandpreview;
    container* root;
    container* last;
    container* current;
    long numOfElemets;
    int height;
    int width;

protected:
    int mode;
    HWF *hwf;
    Tmcl *tmclg;

private:
    long speed;
    int step;
    int engraveTime;
    int minStrenght;
    int maxStrength;

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
    void inline setHWF(HWF *h)          {hwf=h;}

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
    bool loadGCode(QString const &path);
    bool saveFile(QString const &path);
    void printToQListView(QListWidget *listView);

    void displayPreview(cv::Mat *image);
    void displayPreviewBGR(cv::Mat *image,uint8_t blue,uint8_t green,uint8_t red);

    void loadini();


    int  inline getStep()               {return step;}
    int  inline getHeight()             {return height;}
    int  inline getWidth()              {return width;}
    int  inline getNumberOfElements()   {return numOfElemets;}
    int  inline getEngravingTime()      {return engraveTime;}
    int  inline getSpeed()              {return speed;}
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
    void laser(bool on,bool simulation);
    void connect(bool on);

    static QString commandToString(commands com);


};

#endif // COMMANDCONTAINER_H
