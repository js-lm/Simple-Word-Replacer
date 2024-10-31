#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);

    init();

    connect(ui->nextButton, &QPushButton::clicked, this, [this](){close();});
}

ProgressDialog::~ProgressDialog(){
    delete ui;
}

void ProgressDialog::init(){
    qDebug() << "init() called";
    ui->logScreen->clear();
    updateButtons(false);

    ui->completedCountLabel->setText("- / - Completed");
    ui->estimatedTime->setText("Estimated Remaining Time: -:--:--");

}

void ProgressDialog::updateProgress(size_t totalFiles
                                    , size_t processedFileNumber
                                    , const QString &filePath
                                    , qint64 fileSize
                                    , size_t elapsedTime
                                    , size_t wordReplaceCount
                                ){
    qDebug() << "updateProgress() called";
    ui->completedCountLabel->setText(QString::number(processedFileNumber) + " / " + QString::number(totalFiles) + " Completed");

    // currently it shows the elapsed time
    ui->estimatedTime->setText(QString::number(elapsedTime));
}

void ProgressDialog::updateLogScreen(const QString &message){
    qDebug() << "updateLogScreen() called";

    ui->logScreen->appendPlainText(message);
}

void ProgressDialog::updateEstimatedTime(){
    qDebug() << "updateLogScreen() called";

    //ui->estimatedTime->setText();
}

void ProgressDialog::replaceCompleted(){
    qDebug() << "replaceCompleted() called";
    updateButtons(true);
}

void ProgressDialog::updateButtons(bool isFinished){
    qDebug() << "updateButton() called";

    ui->nextButton->setEnabled(isFinished);
    ui->cancelButton->setEnabled(!isFinished);
}
