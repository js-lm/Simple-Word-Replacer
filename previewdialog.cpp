#include "previewdialog.h"
#include "ui_previewdialog.h"

#include <QDir>
#include <QFile>
#include <QListWidgetItem>
#include <QRegularExpression>

PreviewDialog::PreviewDialog(QWidget *parent, MainWindow *mainWinow)
    : QDialog(parent)
    , ui(new Ui::PreviewDialog)
    , mainWindow(mainWinow)
{
    ui->setupUi(this);

    init();

    ui->splitter->setSizes(QList<int>() << 250 << 100);
    ui->fileSelect->setStyleSheet("QScrollBar:vertical {width: 0px;} QScrollBar:horizontal {height: 0px;}");

    connect(ui->backButton, &QPushButton::clicked, this, &PreviewDialog::onBackButtonClicked);
    connect(ui->fileSelect, &QListWidget::itemDoubleClicked, this, &PreviewDialog::onFileListDoubleClicked);
    connect(ui->isShowOriginal, &QPushButton::clicked, this, &PreviewDialog::onShowOriginalClicked);



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

    ui->previewWindow->clear();
}

void PreviewDialog::fetchFiles(){
    qDebug() << "fetchFiles() called";

    ui->fileSelect->clear();

    if(mainWindow->isFile){
        ui->fileSelect->addItem(mainWindow->filePath);
    }else{
        QDir dir(mainWindow->folderPath);

        for(const QFileInfo &file : dir.entryInfoList(QDir::Files)){
            ui->fileSelect->addItem(file.absoluteFilePath());
        }
    }
}

void PreviewDialog::onBackButtonClicked(){
    qDebug() << "onBackButtonClicked() called";
    this->hide();
}

void PreviewDialog::onFileListDoubleClicked(QListWidgetItem *selection){
    qDebug() << "onFileListDoubleClicked() called";

    currentSelection = selection->text();
    updatePreview();
}

void PreviewDialog::onShowOriginalClicked(){
    qDebug() << "onShowOriginalClicked() called";
    ui->isHighlighWord->setEnabled(!ui->isShowOriginal->isChecked());
    ui->isShowingDeletedWord->setEnabled(!ui->isShowOriginal->isChecked());
    updatePreview();
}

void PreviewDialog::updatePreview(){
    qDebug() << "updatePreview() called";

    init();

    QFile file(currentSelection);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "cannot open " << file.fileName();
    }

    QTextStream in(&file);
    QString preview{in.readAll()};
    file.close();

    ui->previewWindow->setText(preview);

    size_t replaceCount{0};
    auto isCaseSensitive{mainWindow->isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive};

    if(!ui->isShowOriginal->isChecked()){
        QString newWord{};
        QString deletedWord{};

        if(mainWindow->newWord.size() <= 1){
            for(const QString &oldWord : mainWindow->oldWord){
                replaceCount += preview.count(oldWord, isCaseSensitive);

                /*newWord = "<span style='color: "
                          + (mainWindow->newWord[0] == "" ? "red; text-decoration: line-through;'>" + oldWord
                                                          : "green;'>" + mainWindow->newWord[0])
                          + "</span>";*/


                deletedWord = ui->isShowingDeletedWord->isEnabled() ? "<span style='color: red; text-decoration: line-through;'>" + oldWord + "</span>"
                                                                    : "";
                newWord =
                    deletedWord + (ui->isHighlighWord->isEnabled()
                                ? "<span style='color: green;'>" + mainWindow->newWord[0] + "</span>"
                                : mainWindow->newWord[0]);

                preview.replace(oldWord, newWord, isCaseSensitive);
            }
        }else{
            for(size_t i{0}; i < mainWindow->oldWord.size(); i++){
                replaceCount += preview.count(mainWindow->oldWord[i], isCaseSensitive);

                qDebug() << "ui->isShowingDeletedWord->isEnabled() " << ui->isShowingDeletedWord->isEnabled();
                qDebug() << "ui->isHighlighWord->isEnabled() " << ui->isHighlighWord->isEnabled();

                deletedWord = ui->isShowingDeletedWord->isEnabled() ? "<span style='color: red; text-decoration: line-through;'>" + mainWindow->oldWord[i] + "</span>"
                                                                    : "";
                newWord =
                    deletedWord + (ui->isHighlighWord->isEnabled()
                                       ? "<span style='color: green;'>" + mainWindow->newWord[i] + "</span>"
                                       : mainWindow->newWord[i]);

                preview.replace(mainWindow->oldWord[i], newWord, isCaseSensitive);
            }
        }
    }

    preview.replace("\n", "<br>");

    ui->previewWindow->setHtml(preview);
    ui->wordReplaceCount->setText("Total Word(s) Replaced: " + QString::number(replaceCount));
}
