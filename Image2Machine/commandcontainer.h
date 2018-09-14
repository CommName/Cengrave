
#ifndef COMMANDCONTAINER_H
#define COMMANDCONTAINER_H
#include <QString>
#include <QListWidget>
#include <opencv2/opencv.hpp>
#include <QPlainTextEdit>
#include "Image2Machine/hwf.h"
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

};

class CommandContainer
{
private:
    container* root;
    container* last;
    container* current;
    long numOfElemets;
    cv::Mat *display;
    QPlainTextEdit *logs;
    HWF *hwf;
    bool secure;

public:
    CommandContainer();
    ~CommandContainer();

public:
    void setCurrent(int index);
    void setImageOutput(cv::Mat *image);
    void setLogOutput(QPlainTextEdit *console);
    void inline setHWF(HWF *h){hwf=h;}
    bool execute(bool simulation);
    bool insert(commands command);
    bool insertSet(int x,int y);
    bool insertAfterCurrent(commands command);
    bool insertAfterCurrentSet(int x,int y);
    bool insertBeforeCurrent(commands command);
    bool insertBeforeCurrentSet(int x,int y);
    void deleteCommand(int index);
    void deleteAll();
    bool loadFile(QString const &path);
    bool saveFile(QString const &path);
    void printToQListView(QListWidget *listView);

    //manual controls
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
};

#endif // COMMANDCONTAINER_H
