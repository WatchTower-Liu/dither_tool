#include "dither.h"

#include <thread>
#include <iostream>


dither::dither(const char *host)
{
    guider = new Guider(host);
}

bool dither::connect()
{
    if (!guider->Connect())
        return false;
    return true;
}

std::vector<double> dither::getDefaultParameter()
{

//    double settlePixels = 2.0;
//    double settleTime = 10.0;
//    double settleTimeout = 100.0;
//    double ditherPixels = 10.0;

    std::vector<double> parameter;
    parameter.push_back(ditherPixels);
    parameter.push_back(settlePixels);
    parameter.push_back(settleTime);
    parameter.push_back(settleTimeout);
    return parameter;
}

void dither::setDitherPixels(int ditherP){
    ditherPixels = ditherP;
}

void dither::setSettlePixels(int settleP)
{
    settlePixels = settleP;
}

void dither::setSettleTime(int settleT)
{
    settleTime = settleT;
}

void dither::setSettleTimeOut(int settleTO)
{
    settleTimeout = settleTO;
}

bool dither::startDither()
{
    return guider->Dither(ditherPixels, settlePixels, settleTime, settleTimeout);
}

SettleProgress dither::WaitForSettleDone()
{

    SettleProgress s;
    if (guider->CheckSettling(&s))
        return s;

    return s;
}
