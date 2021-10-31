#include "mainwindow.h"

#include <QApplication>
#include <QTextCodec>
#include "dither.h"
#include "serial.h"

int main(int argc, char *argv[])
{
//    serial s;
//    std::cout << s.open(3, 9600)<< std::endl;

//    std::cout << s.setRTS()<< std::endl;
//    while(1);
//    std::cout << s.clearRTS()<< std::endl;
//    while(1);
//    return 0;
    QApplication a(argc, argv);
    dither ditherInstance = dither();
    MainWindow w(ditherInstance);
    w.show();
    return a.exec();
}
