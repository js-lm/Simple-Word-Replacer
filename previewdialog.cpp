#include "previewdialog.h"
#include "ui_previewdialog.h"

PreviewDialog::PreviewDialog(QWidget *parent, MainWindow *mainWinow)
    : QDialog(parent)
    , ui(new Ui::PreviewDialog)
    , mainWindow(mainWinow)
{
    ui->setupUi(this);

    init();

    connect(ui->backButton, &QPushButton::clicked, this, &PreviewDialog::onBackButtonClicked);
}

PreviewDialog::~PreviewDialog(){
    delete ui;
}

void PreviewDialog::init(){
    qDebug() << "init() called";

    qDebug() << "isAddingSuffix: " << mainWindow->isAddingSuffix;
    qDebug() << "isCaseSensitive: " << mainWindow->isCaseSensitive;
    qDebug() << "isReplacingFileName: " << mainWindow->isReplacingFileName;
    qDebug() << "suffix: " << mainWindow->suffix;
    qDebug() << "newWord: " << mainWindow->newWord.join(mainWindow->separator);
    qDebug() << "oldWord: " << mainWindow->oldWord.join(mainWindow->separator);


    ui->isAddingSuffix->setChecked(mainWindow->isAddingSuffix);
    ui->isCaseSensitive->setChecked(mainWindow->isCaseSensitive);
    ui->isReplacingFileName->setChecked(mainWindow->isReplacingFileName);
    ui->suffixLine->setText(mainWindow->suffix);
    ui->replaceLine->setText(mainWindow->newWord.join(mainWindow->separator));
    ui->searchLine->setText(mainWindow->oldWord.join(mainWindow->separator));
}

void PreviewDialog::fetchFiles(){
    qDebug() << "fetchFiles() called";

    ui->fileSelect->clear();

    if(mainWindow->isFile){
        for(const QString &path : mainWindow->folderPath){
            ui->fileSelect->addItem(path);
        }
    }else{
        ui->fileSelect->addItem(mainWindow->filePath);
    }

}

void PreviewDialog::onBackButtonClicked(){
    this->hide();
}
