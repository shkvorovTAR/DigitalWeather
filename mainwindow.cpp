#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(managerFinished(QNetworkReply*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString city = ui->comboBox->currentText();
    QString addr = ui->lineEdit_Addr->text();
    QString appid = ui->lineEdit_AppKey->text();
    QString url = "http://" + addr + "?q=" + city + "&units=metric&appid=" + appid;
    qDebug() << url;
    request.setUrl(QUrl(url));
    manager->get(request);
}

void MainWindow::managerFinished(QNetworkReply *reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();
    QJsonDocument jsonResponse = QJsonDocument::fromJson(answer.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    QJsonObject sysObj = jsonObject["sys"].toObject();
    QJsonObject mainObj = jsonObject["main"].toObject();

    QDateTime dtSunRise;

    dtSunRise.setSecsSinceEpoch(sysObj["sunrise"].toInt());
    QTime tSunRise = dtSunRise.time();
    int iSunRiseInSec = tSunRise.msecsSinceStartOfDay()/1000;
    QString qsTemp = tSunRise.toString("HH:mm:ss");
    qDebug() << "Sun rise:" << qsTemp;
    qDebug() << "iSunRiseInSec "<< iSunRiseInSec;

    ui->lineEdit_SunRise->setText("Sun rise: "+qsTemp);

    QDateTime dtSunSet;
    dtSunSet.setSecsSinceEpoch(sysObj["sunset"].toInt());
    QTime tSunSet = dtSunSet.time();
    int iSunSetInSec = tSunSet.msecsSinceStartOfDay()/1000;
    qsTemp = tSunSet.toString("HH:mm:ss");
    qDebug() << "Sun set:" << qsTemp;
    qDebug() << "iSunSetInSec "<< iSunSetInSec;
    ui->lineEdit_SunSet->setText("Sun set: "+qsTemp);

    ui->progressBar->setRange(iSunRiseInSec,iSunSetInSec);

    QDateTime dt = QDateTime::currentDateTime();
    QTime tNow = dt.time();
    //tNow = tNow.addSecs(60*60*11); // shift for 11 hour in future
    int iNowInSec = tNow.msecsSinceStartOfDay()/1000;
    ui->progressBar->setValue(iNowInSec);
    qDebug() << "iNowInSec "<< iNowInSec;
    qsTemp = tNow.toString("HH:mm:ss");
    ui->label_Sun->setText("Now is: "+qsTemp);

    double tempC = mainObj["temp"].toDouble();
    ui->verticalSlider->setValue(static_cast<int>(tempC));
    ui->lcdNumber->display(tempC);
}
