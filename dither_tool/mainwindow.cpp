#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QComboBox>
#include <QWidget>
#include <QObject>



MainWindow::MainWindow(dither dither,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      ditherInstance(dither)
{
    init();


    setSingle();
    setDarkStyle();
    serialDev = serial();
    setComboBox(serialDev.listCOM());

    bool PHD2state = ditherInstance.connect();
    if (PHD2state == false)
        setListViewItem("PHD2连接错误");
    else
        setListViewItem("成功连接PHD2");
}

void MainWindow::init()
{
    ui->setupUi(this);
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
    this->ItemModel = new QStandardItemModel(this);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setWindowTitle("dither tool");

    ui->lineEdit->setValidator(new QIntValidator(ui->lineEdit));
    ui->lineEdit_2->setValidator(new QIntValidator(ui->lineEdit_2));
    ui->lineEdit_3->setValidator(new QIntValidator(ui->lineEdit_3));
    ui->lineEdit_4->setValidator(new QIntValidator(ui->lineEdit_4));
    ui->lineEdit_5->setValidator(new QIntValidator(ui->lineEdit_5));

    std::vector<double> ditherParameter = ditherInstance.getDefaultParameter();
    setDefaultParameter(ditherParameter);

    timer = new QTimer();
    timer->setInterval(1000);
}

void MainWindow::startExposure(int exposureTime, int intervalTime, int expnum)
{
    expTime = exposureTime;
    totalTime = exposureTime;
    setExpTime = exposureTime;
    interval = intervalTime;
    setIntervalTime = intervalTime;
    exp_num = expnum;
    expState = state::exping;
    timer->start();
}

void MainWindow::onTimer()
{
    if (expState == state::exping)
    {
        expTime --;
    }
    if (expState == state::interval)
    {
        if (enableDither)
        {
           SettleProgress settleState =  ditherInstance.WaitForSettleDone();
           if (settleState.Done)
           {
               setListViewItem("抖动完成");
               ditherInstance.ditherFinish = true;
           }
           else
           {
               if (ditherInstance.ditherCounter == 0)
               {
                    setListViewItem("抖动尚未稳定, "+QString::fromStdString(std::to_string(settleState.Distance)));
                    ditherInstance.ditherCounter++;
               }
               else
               {
                   setListViewItem("抖动尚未稳定, "+QString::fromStdString(std::to_string(settleState.Distance)), true);
                   ditherInstance.ditherCounter++;
               }
           }
        }
        interval --;
    }

    if (expTime<=0 && expState == state::exping)
    {
        expState = state::exp_finish;
        exp_num --;
        expState = state::interval;
        interval = setIntervalTime;
        if (enableDither && exp_num>0)
        {
            setListViewItem("抖动!!!");
            ditherInstance.ditherFinish = false;
            ditherInstance.startDither();
        }
        if (exp_num <=0)
        {
            expState = state::all_finish;
        }
    }

    if (interval <=0 && expState == state::interval)
    {
        if (enableDither)
        {
            SettleProgress settleState =  ditherInstance.WaitForSettleDone();
            if (!settleState.Done)
            {
                setListViewItem("抖动尚未稳定, "+QString::fromStdString(std::to_string(settleState.Distance)), true);
                interval = 1;
                ditherInstance.ditherCounter++;
                return ;
            }
            if (ditherInstance.ditherFinish == false)
            {
                setListViewItem("抖动完成");
                ditherInstance.ditherFinish = true;
            }
            ditherInstance.ditherCounter = 0;
        }
        expState = state::interval_finish;
        if (exp_num>0)
        {
            expTime = setExpTime;
            expState = state::exping;
        }
        else
        {
            expState = state::all_finish;
        }
    }
    if (expState == state::all_finish)
    {
        timer->stop();
        setListViewItem("曝光完成");
    }

    std::string display;
    if (expState == state::exping || expState == state::all_finish)
    {
        QString N = "剩余/总爆";
        ui->label_8->setText(N);
        display = std::to_string(expTime) + "/" +std::to_string(totalTime);
        ui->label_7->setText(QString::fromStdString(display));
    }
    else if (expState == state::interval)
    {
        QString N = "剩余/等待";
        ui->label_8->setText(N);
        display = std::to_string(interval) + "/" +std::to_string(setIntervalTime);
        ui->label_7->setText(QString::fromStdString(display));
    }
    display = std::to_string(exp_num);
    ui->label_19->setText(QString::fromStdString(display));

}

void MainWindow::setSingle()
{
    typedef void (QComboBox:: *comboBoxSingle)(const QString&);
    connect(ui->comboBox, static_cast<comboBoxSingle>(&QComboBox::currentIndexChanged), this, &MainWindow::on_comboBox_currentIndexChanged);
//    connect(ui->comboBox, &QComboBox::)
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_exposure_clicked);
    connect(ui->pushButton_2,  &QPushButton::clicked, this, &MainWindow::on_cancel_exposure_clicked);
    connect(ui->pushButton_3,  &QPushButton::clicked, this, &MainWindow::enableOrDisableDither);
    connect(ui->pushButton_4,  &QPushButton::clicked, this, &MainWindow::refreshDev);
    connect(ui->pushButton_4,  &QPushButton::clicked, this, &MainWindow::openOrCloseSerial);
    connect(timer, &QTimer::timeout, this, &MainWindow::onTimer);
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &MainWindow::ditherPixalChanged);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &MainWindow::settlePixelsChanged);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &MainWindow::settleTimeChanged);
    connect(ui->lineEdit_4, &QLineEdit::textChanged, this, &MainWindow::settleTimeOutChanged);
    connect(ui->lineEdit_5, &QLineEdit::textChanged, this, &MainWindow::expTimeChanged);
    connect(ui->lineEdit_6, &QLineEdit::textChanged, this, &MainWindow::expIntervalChanged);
    connect(ui->lineEdit_7, &QLineEdit::textChanged, this, &MainWindow::expNumChanged);
}

void MainWindow::setDarkStyle()
{
    this->setStyleSheet("background-color:#222222");
    ui->label->setStyleSheet("color:#bb0000");
    ui->label_2->setStyleSheet("color:#bb0000");
    ui->label_3->setStyleSheet("color:#bb0000");
    ui->label_4->setStyleSheet("color:#bb0000");
    ui->label_5->setStyleSheet("color:#bb0000");
    ui->label_6->setStyleSheet("color:#bb0000");
    ui->label_7->setStyleSheet("color:#bb0000");
    ui->label_8->setStyleSheet("color:#bb0000");
    ui->label_9->setStyleSheet("color:#bb0000");
    ui->label_10->setStyleSheet("color:#bb0000");
    ui->label_11->setStyleSheet("color:#bb0000");
    ui->label_12->setStyleSheet("color:#bb0000");
    ui->label_13->setStyleSheet("color:#bb0000");
    ui->label_14->setStyleSheet("color:#bb0000");
    ui->label_15->setStyleSheet("color:#bb0000");
    ui->label_16->setStyleSheet("color:#bb0000");
    ui->label_17->setStyleSheet("color:#bb0000");
    ui->label_18->setStyleSheet("color:#bb0000");
    ui->label_19->setStyleSheet("color:#bb0000");
    ui->label_20->setStyleSheet("color:#bb0000");
    ui->label_21->setStyleSheet("color:#bb0000");
    ui->lineEdit->setStyleSheet("color:#cc0000");
    ui->lineEdit_2->setStyleSheet("color:#cc0000");
    ui->lineEdit_3->setStyleSheet("color:#cc0000");
    ui->lineEdit_4->setStyleSheet("color:#cc0000");
    ui->lineEdit_5->setStyleSheet("color:#cc0000");
    ui->lineEdit_6->setStyleSheet("color:#cc0000");
    ui->lineEdit_7->setStyleSheet("color:#cc0000");
    ui->pushButton->setStyleSheet("background-color:#222222; color:#bb0000");
    ui->pushButton_2->setStyleSheet("background-color:#222222; color:#bb0000");
    ui->pushButton_3->setStyleSheet("background-color:#222222; color:#bb0000");
    ui->pushButton_4->setIconSize(QSize(20,20));
    ui->pushButton_4->setIcon(QIcon("./re.png"));
    ui->pushButton_4->setStyleSheet("background-color:#222222; color:#bb0000");
    ui->pushButton_5->setStyleSheet("background-color:#222222; color:#bb0000");
    ui->comboBox->setStyleSheet("color:#bb0000");
    QListView* view = new QListView(this);
    view->setStyleSheet("color:#bb0000");
    ui->comboBox->setView(view);
    ui->listView->setStyleSheet("color:#bb0000");
    ui->frame->setStyleSheet("background-color:#bb0000");
    ui->frame_2->setStyleSheet("background-color:#bb0000");
}

void MainWindow::setDefaultParameter(std::vector<double> P)
{
    std::string settlePixels = std::to_string(static_cast<int>(P[0]));
    ui->lineEdit->setText(QString::fromStdString(settlePixels));
    std::string settleTime = std::to_string(static_cast<int>(P[1]));
    ui->lineEdit_2->setText(QString::fromStdString(settleTime));
    std::string settleTimeout = std::to_string(static_cast<int>(P[2]));
    ui->lineEdit_3->setText(QString::fromStdString(settleTimeout));
    std::string ditherPixels = std::to_string(static_cast<int>(P[3]));
    ui->lineEdit_4->setText(QString::fromStdString(ditherPixels));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setComboBox(std::vector<int> devList)
{
    ui->comboBox->clear();
    for (auto item : devList)
    {
        std::string dev = "COM"+std::to_string(item);
        ui->comboBox->addItem(QString::fromStdString(dev));
    }
}

std::string MainWindow::getSelectComboBoxItem()
{
    QString selectItem = ui->comboBox->currentData(0).toString();
    std::string item = selectItem.toStdString();
    return item;

}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    std::cout << getSelectComboBoxItem()<<std::endl;
}



void MainWindow::on_exposure_clicked()
{
    std::cout << "pushButton" <<std::endl;
    setListViewItem("进行曝光");

    if (setExpNum<1 || setExpTime<1 || setIntervalTime<1)
    {
         setListViewItem(QString("曝光参数设置错误!"));
         return ;
    }
    startExposure(setExpTime, setIntervalTime, setExpNum);
    std::string display;
    display = std::to_string(expTime) + "/" +std::to_string(totalTime);
    ui->label_7->setText(QString::fromStdString(display));
    display = std::to_string(exp_num);
    ui->label_19->setText(QString::fromStdString(display));
}

void MainWindow::on_cancel_exposure_clicked()
{
    setListViewItem("取消曝光");
    timer->stop();
}

void MainWindow::expTimeChanged()
{
    std::string lineEditText = ui->lineEdit_5->text().toStdString();
    if (lineEditText != "")
        setExpTime = std::stoi(lineEditText);
    else
        setExpTime = 0;
}

void MainWindow::expIntervalChanged()
{
    std::string lineEditText = ui->lineEdit_6->text().toStdString();
    if (lineEditText != "")
        setIntervalTime = std::stoi(lineEditText);
    else
        setIntervalTime = 0;
}

void MainWindow::expNumChanged()
{
    std::string lineEditText = ui->lineEdit_7->text().toStdString();
    if (lineEditText != "")
        setExpNum = std::stoi(lineEditText);
    else
        setExpNum = 0;
}

void MainWindow::ditherPixalChanged()
{
    std::string lineEditText = ui->lineEdit->text().toStdString();
    if (lineEditText != "")
        ditherInstance.setDitherPixels(std::stoi(lineEditText));
    else
        ditherInstance.setDitherPixels(0);
}

void MainWindow::settlePixelsChanged()
{
    std::string lineEditText = ui->lineEdit_2->text().toStdString();
    if (lineEditText != "")
        ditherInstance.setSettlePixels(std::stoi(lineEditText));
    else
        ditherInstance.setSettlePixels(0);
}

void MainWindow::settleTimeChanged()
{
    std::string lineEditText = ui->lineEdit_3->text().toStdString();
    if (lineEditText != "")
        ditherInstance.setSettleTime(std::stoi(lineEditText));
    else
        ditherInstance.setSettleTime(0);
}

void MainWindow::settleTimeOutChanged()
{
    std::string lineEditText = ui->lineEdit_4->text().toStdString();
    if (lineEditText != "")
        ditherInstance.setSettleTimeOut(std::stoi(lineEditText));
    else
        ditherInstance.setSettleTimeOut(0);
}

void MainWindow::setListViewItem(QString itemName, bool oldline)
{
    QStandardItem *item = new QStandardItem(itemName);

    if (!oldline)
    {
        ItemModel->appendRow(item);
        ui->listView->setModel(ItemModel);
    }
    else
    {
        int rownum = ItemModel->rowCount();
        ItemModel->setItem(rownum-1, item);
        ui->listView->setModel(ItemModel);
    }

}

void MainWindow::openOrCloseSerial()
{

}

void MainWindow::refreshDev()
{
    setComboBox(serialDev.listCOM());
}

void MainWindow::enableOrDisableDither()
{
    if (enableDither == true)
    {
        enableDither = false;
        ui->pushButton_3->setText("disable");
    }
    else
    {
        enableDither = true;
        ui->pushButton_3->setText("enable");
    }
}
