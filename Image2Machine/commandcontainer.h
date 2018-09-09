
#ifndef COMMANDCONTAINER_H
#define COMMANDCONTAINER_H
#include <QString>
#include <QListWidget>
#include <opencv2/opencv.hpp>

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
};

class CommandContainer
{
private:
    container* root;
    container* last;
    container* current;
    long numOfElemets;
    cv::Mat *display;
public:
    CommandContainer();
    ~CommandContainer();

public:
    void setCurrent(int index);
    void setImageOutput(cv::Mat *image);
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


};

#endif // COMMANDCONTAINER_H
