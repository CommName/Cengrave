#ifndef GRAPHIMAGE_H
#define GRAPHIMAGE_H
#include <opencv2/opencv.hpp>
#include "Image2Machine/commandcontainer.h"
#include <QString>
#include <QProgressBar>
struct edge;

struct pixel{
    int x; //column
    int y; //row
    pixel* next;
    pixel* last;
    edge* link; //adjacent pixels
    int status;
};

struct edge{
    pixel* dest;
    edge* link;
};


class GraphImage
{
 private:
    pixel *root;
    pixel *end;
    long numOfElements;
private:
    QProgressBar *importing;
    QProgressBar *exporting;
protected:
    void setUpImporting(int maxNumber);
    void setUpExporting();
    void updateImporting(int &status);
    void updateExporting(int &status);
public:
    GraphImage();
    ~GraphImage();
public:
    void inline setImportProgressBar(QProgressBar *bar){importing=bar;}
    void inline setExportProgressBar(QProgressBar *bar){exporting=bar;}

protected:
    bool insert(int x,int y,pixel* (GraphImage::*findFunction)(int,int));
    pixel* findCR(int x, int y);
    pixel* findRC(int x, int y);
    pixel* findNotConnected(int,int);

    void deleteAll();

    void printCommand(pixel* atm, pixel*next ,std::ofstream &f,commands *lastCommand=nullptr);
    void commandContainerInsert(pixel *atm, pixel*next, CommandContainer &com,commands *lastCommand=nullptr);


public:
    bool insertColsRows(cv::Mat const &image);
    bool insertColsRowsNotConnected(cv::Mat const &image);
    bool insertRowsCols(cv::Mat const &image);
    bool insertRowsColsNotConnected(cv::Mat const&image);
    bool insertColsRowsZigZag(cv::Mat const &image);
    bool insertColsRowsZigZagNotConnected(cv::Mat const &image);
    bool insertRowsColsZigZag(cv::Mat const &image);
    bool insertRowsColsZigZagNotConnected(cv::Mat const &image);
    bool insertDiagonal(cv::Mat const &image);
    bool insertDiagonalZigZag(cv::Mat const &image);


public:
    bool tooFileHeightWidth(QString const &path);
    bool tooFileDepth(QString const &path);
    bool tooCommandContainerHeightWidth(CommandContainer &con);
    bool tooCommandContainerDepth(CommandContainer &con);

public:
    bool test(QString const &filePath);



};

#endif // GRAPHIMAGE_H
