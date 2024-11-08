#include "previewdialog.h"
#include "ui_previewdialog.h"

#include <QDir>
#include <QFile>
#include <QListWidgetItem>
#include <QRegularExpression>
#include <QScrollBar>

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

    int previousCursorPosition{ui->previewWindow->textCursor().position()};
    int previousScrollPosition{ui->previewWindow->verticalScrollBar()->value()};

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

    if(!ui->isShowOriginal->isChecked()){
        QTextDocument::FindFlags isCaseSensitive{mainWindow->isCaseSensitive ? QTextDocument::FindCaseSensitively : QTextDocument::FindFlags()};
        isCaseSensitive |= QTextDocument::FindWholeWords;

        QTextCursor cursor(ui->previewWindow->document());

        for(size_t i{0}; i < mainWindow->oldWord.size(); i++){
            cursor = ui->previewWindow->textCursor(); // cursor would be null otherwise for some reason
            cursor.setPosition(0);

            const QString &oldWord{mainWindow->oldWord[i]};
            const QString &newWord{mainWindow->newWord.size() <= 1 ? mainWindow->newWord[0] : mainWindow->newWord[i]};

            qDebug() << "Document text length: " << ui->previewWindow->document()->toPlainText().length();
            qDebug() << "cursor.isNull() " << cursor.isNull();
            qDebug() << "cursor.atEnd() " << cursor.atEnd();

            while(!cursor.isNull() && !cursor.atEnd()){
                replaceCount++;
                qDebug() << "Loop [" << i << "]";
                qDebug() << "Searching for " << oldWord;
                qDebug() << "Replacing with " << newWord;
                cursor = ui->previewWindow->document()->find(oldWord, cursor, isCaseSensitive);
                if(cursor.isNull()) break;

                qDebug() << "Cursor Position Before Removing: " << cursor.position();

                cursor.select(QTextCursor::WordUnderCursor);
                qDebug() << "Removing " << cursor.selectedText();
                cursor.removeSelectedText();
                cursor.clearSelection();

                qDebug() << "Cursor Position After Removing and Before Adding New Word: " << cursor.position();

                if(ui->isShowingDeletedWord->isChecked()
                    && ui->isShowingDeletedWord->isEnabled()){

                    QTextCharFormat oldWordFormat;
                    oldWordFormat.setForeground(Qt::red);
                    oldWordFormat.setFontStrikeOut(true);

                    cursor.insertText(oldWord, oldWordFormat);
                }

                cursor.clearSelection();
                qDebug() << "Cursor Position After Inserting Deleted Word: " << cursor.position();

                QTextCharFormat newWordFormat;
                if(ui->isHighlightingWord->isChecked()){
                    newWordFormat.setForeground(Qt::green);
                }
                qDebug() << "Cursor Selection Before Insertion " << cursor.selectedText();
                cursor.insertText(newWord, newWordFormat);
                qDebug() << "Cursor Selection After Insertion " << cursor.selectedText();


                qDebug() << "Cursor Position After Inserting New Word: " << cursor.position();
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, newWord.length());
                qDebug() << "Cursor Position After Moving: " << cursor.position();
            }
        }
    }

    QTextCursor cursor{ui->previewWindow->textCursor()};
    cursor.setPosition(previousCursorPosition);
    ui->previewWindow->setTextCursor(cursor);
    ui->previewWindow->verticalScrollBar()->setValue(previousScrollPosition);

    ui->wordReplaceCount->setText("Total Word(s) Replaced: " + QString::number(replaceCount));
}
