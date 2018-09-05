#ifndef GRAPHIMAGE_H
#define GRAPHIMAGE_H
#include <opencv2/opencv.hpp>
#include "Image2Machine/commandcontainer.h"
#include <QString>
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

public:
    GraphImage();
    ~GraphImage();

protected:
    bool insert(int x,int y,pixel* (GraphImage::*findFunction)(int,int));
    pixel* findCR(int x, int y);
    pixel* findRC(int x, int y);
    pixel* findNotConnected(int,int);

    void deleteAll();

    void printCommand(pixel* atm, pixel*next ,std::ofstream &f);



public:
    bool insertColsRows(cv::Mat const &image);
    bool insertColsRowsNotConnected(cv::Mat const &image);
    bool insertRowsCols(cv::Mat const &image);
    bool insertRowsColsNotConnected(cv::Mat const&image);
    bool insertColsRowsZigZag(cv::Mat const &image);
    bool insertColsRowsZigZagNotConnected(cv::Mat const &image);
    bool insertRowsColsZigZag(cv::Mat const &image);
    bool insertRowsColsZigZagNotConnected(cv::Mat const &image);

public:
    bool tooFileHeightWidth(QString const &path);

public:
    bool test(QString const &filePath);



};

#endif // GRAPHIMAGE_H
