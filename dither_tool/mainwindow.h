#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <string>
#include <QStandardItemModel>
#include <vector>
#include <QTimer>

#include "dither.h"
#include "serial.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class state
{
    waiting,
    exping,
    exp_finish,
    interval,
    interval_finish,
    all_finish
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(dither ditherInstance, QWidget *parent = nullptr);
    void setComboBox(std::vector<int> devList);
    std::string getSelectComboBoxItem();
    void on_comboBox_currentIndexChanged(const QString &arg1);
    void on_exposure_clicked();
    void on_cancel_exposure_clicked();
    void setListViewItem(QString itemName, bool newline = false);
    void setDefaultParameter(std::vector<double> P);
    void expTimeChanged();
    void expIntervalChanged();
    void expNumChanged();
    void ditherPixalChanged();
    void settlePixelsChanged();
    void settleTimeChanged();
    void settleTimeOutChanged();
    void openOrCloseSerial();
    void setSingle();
    void setDarkStyle();
    void enableOrDisableDither();
    void refreshDev();
    void init();

    void startExposure(int exposureTime, int intervalTime, int expnum);
    void onTimer();

    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* ItemModel = nullptr;
    dither ditherInstance;

    int expTime = 0;
    int totalTime = 0;
    int exp_num = 0;
    int interval = 0;
    int setExpTime = 0;
    int setIntervalTime = 0;
    int setExpNum = 0;
    state expState = state::waiting;
    QTimer *timer = nullptr;

    bool enableDither = true;

    serial serialDev;
};
#endif // MAINWINDOW_H
