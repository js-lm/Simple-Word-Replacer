#include "previewdialog.h"
#include "ui_previewdialog.h"

#include <QDir>
#include <QFile>
#include <QListWidgetItem>
#include <QRegularExpression>
#include <QScrollBar>
#include <QMessageBox>

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
    connect(ui->selectButton, &QPushButton::clicked, this, &PreviewDialog::onSelectButtonClicked);
}

PreviewDialog::~PreviewDialog(){
    delete ui;
}

void PreviewDialog::init(){
    qDebug() << "init() called";

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

    ui->fileCountLabel->setText("Total File Number(s): " + QString::number(ui->fileSelect->count()));
}

void PreviewDialog::onBackButtonClicked(){
    qDebug() << "onBackButtonClicked() called";
    mainWindow->show();
    hide();
}

void PreviewDialog::closeEvent(QCloseEvent *event){
    qDebug() << "closeEvent() called";
    mainWindow->show();
}

void PreviewDialog::onFileListDoubleClicked(QListWidgetItem *selection){
    qDebug() << "onFileListDoubleClicked() called";

    currentSelection = selection->text();
    updatePreview();
    ui->previewWindow->verticalScrollBar()->setValue(0);
}

void PreviewDialog::onShowOriginalClicked(){
    qDebug() << "onShowOriginalClicked() called";
    ui->isHighlightingWord->setEnabled(!ui->isShowOriginal->isChecked());
    ui->isShowingDeletedWord->setEnabled(!ui->isShowOriginal->isChecked() && ui->isHighlightingWord->isChecked());

    qDebug() << "ui->isHighlighWord->isChecked() " << ui->isHighlightingWord->isChecked();
    qDebug() << "ui->isShowingDeletedWord->isChecked() " << ui->isShowingDeletedWord->isChecked();

    if(ui->previewWindow->toPlainText().isEmpty()) return;
    updatePreview();
}

void PreviewDialog::onHighlightClicked(){
    qDebug() << "onHighlightClicked() called";
    ui->isShowingDeletedWord->setEnabled(ui->isHighlightingWord->isChecked());
    if(ui->previewWindow->toPlainText().isEmpty()) return;
    updatePreview();
}

void PreviewDialog::onShowingDeletedWordClicked(){
    qDebug() << "onShowingDeletedWordClicked() called";
    if(ui->previewWindow->toPlainText().isEmpty()) return;
    updatePreview();
}

void PreviewDialog::onSelectButtonClicked(){
    qDebug() << "onSelectButtonClicked() called";

    QListWidgetItem *currentItem{ui->fileSelect->currentItem()};
    if(currentItem){
        currentSelection = currentItem->text();
        updatePreview();
        ui->previewWindow->verticalScrollBar()->setValue(0);
    }
}

void PreviewDialog::updatePreview(){
    qDebug() << "updatePreview() called";

    int previousCursorPosition{ui->previewWindow->textCursor().position()};
    int previousScrollPosition{ui->previewWindow->verticalScrollBar()->value()};
    init();

    QFile file(currentSelection);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "cannot open " << file.fileName();
        return;
    }
    QTextStream in(&file);
    QString preview{in.readAll()};
    file.close();

    size_t replaceCount{0};

    QTextCharFormat defaultFormat;

    if(!ui->isShowOriginal->isChecked()){
        if(preview.size() > 524288){ // ~1MB
            QString message{(ui->isHighlightingWord->isChecked() && ui->isHighlightingWord->isEnabled()) ?
                            "The file is quite large, and previewing it with highlighting may take a long time.\n\nDo you want to only show the first 512 KiB of text?" :
                            "The file is quite large, and previewing it may take a long time.\n\nDo you want to only show the first 512 KiB of text?",
                            };
            QMessageBox::StandardButton isProceed;
            isProceed = QMessageBox::warning(this, "Do You Really Want To Proceed?", message, QMessageBox::Yes | QMessageBox::No);
            if(isProceed == QMessageBox::Yes){
                preview = preview.left(262144);
            }
        }

        QTextDocument text;
        QTextCursor cursor(&text);

        size_t currentPosition{0};
        while(currentPosition < preview.length()){
            bool foundMatch{false};

            for(size_t i{0}; i < mainWindow->oldWord.size(); i++){
                const QString &oldWord{mainWindow->oldWord[i]};
                const QString &newWord{mainWindow->newWord.size() <= 1 ? mainWindow->newWord[0] : mainWindow->newWord[i]};

                bool isMatch{false};
                if(mainWindow->isCaseSensitive){
                    isMatch = preview.mid(currentPosition, oldWord.length()) == oldWord;
                }else{
                    isMatch = preview.mid(currentPosition, oldWord.length()).compare(oldWord, Qt::CaseInsensitive) == 0;
                }

                if(isMatch){
                    foundMatch = true;
                    replaceCount++;

                    if(ui->isShowingDeletedWord->isChecked() && ui->isShowingDeletedWord->isEnabled()){
                        QTextCharFormat oldWordFormat;
                        oldWordFormat.setForeground(QColor(250, 128, 114));
                        oldWordFormat.setFontStrikeOut(true);
                        cursor.insertText(oldWord, oldWordFormat);
                    }

                    if(ui->isHighlightingWord->isChecked()){
                        QTextCharFormat newWordFormat;
                        newWordFormat.setForeground(QColor(152, 251, 152));
                        cursor.insertText(newWord, newWordFormat);
                    }else{
                        cursor.insertText(newWord);
                    }

                    currentPosition += oldWord.length();
                    break;
                }
            }

            if(!foundMatch){
                cursor.setCharFormat(defaultFormat);
                cursor.insertText(QString(preview[currentPosition]));
                currentPosition++;
            }
        }

        ui->previewWindow->setDocument(text.clone());
    }else{
        ui->previewWindow->setText(preview);
    }

    QTextCursor finalCursor{ui->previewWindow->textCursor()};
    finalCursor.setPosition(previousCursorPosition);
    ui->previewWindow->setTextCursor(finalCursor);
    ui->previewWindow->verticalScrollBar()->setValue(previousScrollPosition);
    ui->wordReplaceCount->setText("Total Word(s) Replaced: " + QString::number(replaceCount));
}
