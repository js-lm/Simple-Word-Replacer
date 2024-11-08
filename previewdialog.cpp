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
    connect(ui->isHighlightingWord, &QPushButton::clicked, this, &PreviewDialog::onHighlightClicked);
    connect(ui->isShowingDeletedWord, &QPushButton::clicked, this, &PreviewDialog::onShowingDeletedWordClicked);
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
    ui->isHighlightingWord->setEnabled(!ui->isShowOriginal->isChecked());
    ui->isShowingDeletedWord->setEnabled(!ui->isShowOriginal->isChecked() && ui->isHighlightingWord->isChecked());

    qDebug() << "ui->isHighlighWord->isChecked() " << ui->isHighlightingWord->isChecked();
    qDebug() << "ui->isShowingDeletedWord->isChecked() " << ui->isShowingDeletedWord->isChecked();

    updatePreview();
}

void PreviewDialog::onHighlightClicked(){
    qDebug() << "onHighlightClicked() called";
    ui->isShowingDeletedWord->setEnabled(ui->isHighlightingWord->isChecked());
    updatePreview();
}

void PreviewDialog::onShowingDeletedWordClicked(){
    qDebug() << "onShowingDeletedWordClicked() called";
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
        QTextCursor cursor(ui->previewWindow->document());

        for(size_t i{0}; i < mainWindow->oldWord.size(); i++){
            const QString &searchForWord{mainWindow->oldWord[i]};
            const QString replaceWithWord{mainWindow->newWord.value(i)};

            cursor.setPosition(0);
            QTextDocument::FindFlags isCaseSensitiveFlag{isCaseSensitive ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags()};

            while(!cursor.isNull() && !cursor.atEnd()){
                cursor = ui->previewWindow->document()->find(searchForWord, cursor, isCaseSensitiveFlag);

                if(!cursor.isNull()){
                    replaceCount++;

                    if(ui->isShowingDeletedWord->isChecked() && ui->isShowingDeletedWord->isEnabled()){
                        QTextCharFormat oldWordFormat;
                        oldWordFormat.setForeground(Qt::red);
                        oldWordFormat.setFontStrikeOut(true);

                        cursor.mergeCharFormat(oldWordFormat);
                    }else{
                        cursor.select(QTextCursor::WordUnderCursor);
                        cursor.removeSelectedText();
                    }

                    if(!replaceWithWord.isEmpty()){
                        cursor.clearSelection();

                        QTextCharFormat newWordFormat;
                        if(ui->isHighlightingWord->isChecked()){
                            newWordFormat.setForeground(Qt::green);
                        }

                        cursor.insertText(replaceWithWord, newWordFormat);
                    }
                }
            }
        }
    }

    ui->wordReplaceCount->setText("Total Word(s) Replaced: " + QString::number(replaceCount));
}
