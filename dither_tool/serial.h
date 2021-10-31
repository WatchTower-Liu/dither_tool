#ifndef SERIAL_H
#define SERIAL_H

#include <iostream>
#include <windows.h>
#include <vector>

class serial
{
public:
    serial();
    ~serial();
    bool open(int port, int nB);
    bool close();
    std::vector <int> listCOM();
    bool setRTS();
    bool clearRTS();
private:
    HANDLE m_hiIDConDev;
    bool m_bOpened;
};

#endif // SERIAL_H
