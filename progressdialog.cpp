#include "progressdialog.h"
#include "ui_progressdialog.h"
#include <QTimer>

ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProgressDialog)
    , isCancelled(false)
    , timer(new QTimer(this))
{
    ui->setupUi(this);

    init();

    connect(ui->nextButton, &QPushButton::clicked, this, [this](){close();});
    connect(ui->cancelButton, &QPushButton::clicked, this, [this](){
        qDebug() << "Cancel Clicked";
        //emit cancel();
        isCancelled = true;
        ui->pleaseWaitLabel->setText("⚠ Cancelling... Please hold on a moment");
    });

    timer->setInterval(1);
    timer->start();
    connect(timer, &QTimer::timeout, this, [this](){
        if(estimatedTime > 0 && !ui->nextButton->isEnabled()){
            estimatedTime--;
            updateEstimatedTimeDisplay();
        }
    });
}

ProgressDialog::~ProgressDialog(){
    delete ui;
}

void ProgressDialog::init(){
    qDebug() << "init() called";
    ui->logScreen->clear();
    updateButtons(false);

    ui->completedCountLabel->setText("- / - Completed");
    ui->estimatedTime->setText("Estimated Remaining Time: --:--:--");
    ui->progressBar->setValue(0);
    ui->pleaseWaitLabel->clear();

    estimatedTime = 0;
    processedSize = 0;

    estimatedTime = 0;
}

void ProgressDialog::updateEstimatedTimeDisplay(){
    QString hour{QString::number(estimatedTime / 3600000).rightJustified(2, '0')};
    QString minute{QString::number(estimatedTime / 60000 % 60).rightJustified(2, '0')};
    QString second{QString::number(estimatedTime / 1000 % 60).rightJustified(2, '0')};

    QString estimatedTime{"Estimated Remaining Time: " + hour + ":" + minute + ":" + second};
    ui->estimatedTime->setText(estimatedTime);
}

void ProgressDialog::updateProgress(qint64 fileSize
                                    , qint64 totalFileSize
                                    , size_t totalElapsedTime
                                    , size_t processedFileNumber
                                    , size_t totalFiles
                                ){
    qDebug() << "updateProgress() called";
    ui->completedCountLabel->setText(QString::number(processedFileNumber) + " / " + QString::number(totalFiles) + " Completed");
    ui->progressBar->setValue(100 * processedFileNumber / totalFiles);

    processedSize += fileSize;
    estimatedTime = (double)totalElapsedTime / processedSize * (totalFileSize - processedSize);

    qDebug() << "totalElapsedTime: " << totalElapsedTime;
    qDebug() << "processedSize: " << processedSize;
    qDebug() << "totalFileSize: " << totalFileSize;
    qDebug() << "estimatedTime: " << estimatedTime;

    updateEstimatedTimeDisplay();
}

void ProgressDialog::updateLogScreen(const QString &message){
    qDebug() << "updateLogScreen() called with message: " << message;
    ui->logScreen->append(message);
}

void ProgressDialog::replaceCompleted(){
    qDebug() << "replaceCompleted() called";
    updateButtons(true);
}

void ProgressDialog::updateButtons(bool isFinished){
    qDebug() << "updateButton() called";

    ui->nextButton->setEnabled(isFinished);
    ui->cancelButton->setEnabled(!isFinished);

    if(isFinished){
        ui->pleaseWaitLabel->clear();
    }
}

