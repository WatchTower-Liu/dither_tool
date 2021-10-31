#include "serial.h"
#include <tchar.h>

#define FC_RTS    0x02;

serial::serial()
{
    m_hiIDConDev = NULL;
    m_bOpened = false;
}

serial::~serial()
{
    close();
}

bool serial::open(int port, int nB)
{
    if (m_bOpened)
        return true;
    TCHAR szPort[50];
//    TCHAR szComParams{50};
    _sntprintf (szPort, sizeof(szPort), _T("COM%d"), port);

    m_hiIDConDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
    if (m_hiIDConDev == NULL || m_hiIDConDev == INVALID_HANDLE_VALUE)
        return false;

    m_bOpened = true;
    return true;
}

bool serial::close()
{
    CloseHandle(m_hiIDConDev);
    m_bOpened = false;
    m_hiIDConDev = NULL;
    return true;
}

std::vector<int> serial::listCOM()
{
    std::vector<int> allDev;
    for (int i=1; i<255;i++)
    {
        TCHAR szPort[50];
    //    TCHAR szComParams{50};
        _sntprintf (szPort, sizeof(szPort), _T("COM%d"), i);

        HANDLE opened = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
        if (opened != INVALID_HANDLE_VALUE)
        {
            allDev.push_back(i);
               CloseHandle(opened);
        }
    }
    return allDev;
}

bool serial::setRTS()
{
    return EscapeCommFunction(m_hiIDConDev, SETRTS);
}

bool serial::clearRTS()
{
    return EscapeCommFunction(m_hiIDConDev, CLRRTS);
}
