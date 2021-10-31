#ifndef DITHER_H
#define DITHER_H

#include "guider.h"
#include <vector>

class dither
{
public:
    dither(const char *host = "localhost");
    bool connect();
    void setDitherParameter(int ditherPixal, double settlePixels=2.0, double settleTime=10.0, double settleTimeout=100.0);
    std::vector<double> getDefaultParameter();
    SettleProgress WaitForSettleDone();
    bool startDither();
    void setDitherPixels(int ditherP);
    void setSettlePixels(int settleP);
    void setSettleTime(int settleT);
    void setSettleTimeOut(int settleTO);

    bool ditherCounter = 0;
    bool ditherFinish = false;

private:
    Guider *guider = nullptr;
    double settlePixels = 2.0;
    double settleTime = 10.0;
    double settleTimeout = 100.0;
    double ditherPixels = 10.0;


};

#endif // DITHER_H
