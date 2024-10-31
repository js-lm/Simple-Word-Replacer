#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);

    init();
}

ProgressDialog::~ProgressDialog(){
    delete ui;
}

void ProgressDialog::init(){
    qDebug() << "init() called";
    ui->logScreen->clear();
    ui->nextButton->setEnabled(false);
    ui->cancelButton->setEnabled(true);

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


}

void ProgressDialog::updateLogScreen(const QString &message){
    qDebug() << "updateLogScreen() called";

    ui->logScreen->appendPlainText(message);
}
