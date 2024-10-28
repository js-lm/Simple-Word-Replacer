#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include "customeditor.h"
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initTipsAndWarnings();
    initConnects();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::initTipsAndWarnings(){
    ui->inFileWarning->clear();
    ui->inFolderWarning->clear();
    ui->outFolderWarning->clear();
    ui->replaceWarningLabel->clear();

}

void MainWindow::initConnects(){
    qDebug() << "Initiating Connects in Main Window...";

    // Input Row //
    connect(ui->inFileButton, &QPushButton::clicked, this, &MainWindow::onInFileClicked);
    connect(ui->inFolderButton, &QPushButton::clicked, this, &MainWindow::onInFolderClicked);

    connect(ui->inFileLine, &CustomLineEdit::focusLost, this, &MainWindow::inFileLineFocusLost);
    connect(ui->inFolderLine, &CustomLineEdit::focusLost, this, &MainWindow::inFolderLineFocusLost);

    // Replace //
    connect(ui->oldWordLine, &CustomPlainTextEdit::focusLost, this, &MainWindow::oldWordLineFocusLost);
    connect(ui->newWordLine, &CustomPlainTextEdit::focusLost, this, &MainWindow::newWordLineFocusLost);

    // Option Row //
    connect(ui->isCaseSensitive, &QPushButton::clicked, this, &MainWindow::onCaseSensitiveClicked);

    connect(ui->separatorLine, &CustomLineEdit::focusLost, this, &MainWindow::separatorLineFocusLost);

    // Setting Row //
    connect(ui->isReplacingFileName, &QPushButton::clicked, this, &MainWindow::onReplaceFileClicked);
    connect(ui->isAddingSuffix, &QPushButton::clicked, this, &MainWindow::onAddSuffixClicked);

    // Output Row //
    connect(ui->openOutFolderButton, &QPushButton::clicked, this, &MainWindow::onOpenOutputFolderClicked);
    connect(ui->isCreatingBackup, &QPushButton::clicked, this, &MainWindow::onCreateBackupClicked);
    connect(ui->isReplacingOriginal, &QPushButton::clicked, this, &MainWindow::onReplaceOriginalClicked);
    connect(ui->outPathButton, &QPushButton::clicked, this, &MainWindow::onSelectOutputFolderClicked);

    connect(ui->outPathLine, &CustomLineEdit::focusLost, this, &MainWindow::outPathLineFocusLost);
    connect(ui->suffixLine, &CustomLineEdit::focusLost, this, &MainWindow::suffixLineFocusLost);

    // Bottom Row //
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    connect(ui->previewButton, &QPushButton::clicked, this, &MainWindow::onPreviewButtonClicked);

    qDebug() << "Main Window Connects Initiation Completed...";
}

// Input Row //
void MainWindow::onInFileClicked(){
    qDebug() << "onInFileClicked() called";
    QString path{QFileDialog::getOpenFileName(this, "Select a File", "", "Text Files (" + extensionFilters + ");;All Files (*.*)")};

    if(!path.isEmpty()){
        ui->inFileLine->setText(path);
    }else if(ui->inFileLine->text().isEmpty()){
        ui->inFileWarning->setText("⚠ No File Selected");
        return;
    }

    ui->inFileWarning->clear();
    filePath = ui->inFileLine->text();
    qDebug() << "updated filePath to " << filePath;
}

void MainWindow::onInFolderClicked(){
    qDebug() << "onInFolderClicked() called";
    QString path{QFileDialog::getExistingDirectory(this, "Select a Folder", "")};

    if(!path.isEmpty()){
        ui->inFolderLine->setText(path);
    }else if(ui->inFolderLine->text().isEmpty()){
        ui->inFolderWarning->setText("⚠ No Folder Selected");
        return;
    }

    ui->inFolderWarning->clear();
    folderPath = ui->inFolderLine->text();
    qDebug() << "updated folderPath to " << folderPath;
}

void MainWindow::inFileLineFocusLost(){
    qDebug() << "inFileLine out of focus";
    QString path{ui->inFileLine->text()};

    if(path.isEmpty()){
        ui->inFileWarning->setText("⚠ No File Entered");
        return;
    }else if(QDir(path).exists()){
        ui->inFileWarning->setText("⚠ This Looks Like A Folder");
        return;
    }else if(!QFile::exists(path)){
        ui->inFileWarning->setText("⚠ File Does Not Exist");
        return;
    }

    ui->inFileWarning->clear();
    filePath = ui->inFileLine->text();
    qDebug() << "updated filePath to " << filePath;
}

void MainWindow::inFolderLineFocusLost(){
    qDebug() << "inFolderLine out of focus";
    QString path{ui->inFolderLine->text()};

    if(path.isEmpty()){
        ui->inFolderWarning->setText("⚠ No Folder Entered");
        return;
    }else if(!QDir(path).exists()){
        ui->inFolderWarning->setText("⚠ Folder Does Not Existed");
        return;
    }

    ui->inFolderWarning->clear();
    folderPath = ui->inFolderLine->text();
    qDebug() << "updated folderPath to " << folderPath;
}

// Replace //
void MainWindow::oldWordLineFocusLost(){
    qDebug() << "oldWordLine out of focus";
    checkReplacementAndUpdateIfValid();
}

void MainWindow::newWordLineFocusLost(){
    qDebug() << "newWordLine out of focus";
    checkReplacementAndUpdateIfValid();
}

void MainWindow::checkReplacementAndUpdateIfValid(){
    qDebug() << "checkReplacementAndUpdateIfValid() called";
    QStringList oldWds{ui->oldWordLine->text().split(ui->separatorLine->text())};
    QStringList newWds{ui->newWordLine->text().split(ui->separatorLine->text())};

    oldWds.removeAll("");

    if(oldWds.isEmpty()){
        ui->replaceWarningLabel->setText("⚠ Nothing to search for");
        return;
    }else if(newWds.size() > 1 && oldWds.size() > newWds.size()){
        ui->replaceWarningLabel->setText("⚠ Size doesn't match - Too many old word(s)");
        return;
    }else if(newWds.size() > 1 && oldWds.size() < newWds.size()){
        ui->replaceWarningLabel->setText("⚠ Size doesn't match - Too many new word(s)");
        return;
    }

    ui->replaceWarningLabel->clear();
    oldWord = oldWds;
    newWord = newWds;
    qDebug() << "updated oldWds to " << oldWds;
    qDebug() << "updated newWds to " << newWds;
}

// Option Row //
void MainWindow::onCaseSensitiveClicked(){
    qDebug() << "onCaseSensitiveClicked() called";
}

void MainWindow::separatorLineFocusLost(){
    qDebug() << "separatorLine out of focus";
}

// Setting Row //
void MainWindow::onReplaceFileClicked(){
    qDebug() << "onReplaceFileClicked() called";
}

void MainWindow::onAddSuffixClicked(){
    qDebug() << "onAddSuffixClicked() called";
}

// Output Row //
void MainWindow::onOpenOutputFolderClicked(){
    qDebug() << "onOpenOutputFolderClicked() called";

    if(!QDesktopServices::openUrl(QUrl::fromLocalFile(outPath))){
        qDebug() << "failed to open " << outPath;
    }else{
        qDebug() << "opened " << outPath;
    }
}

void MainWindow::onCreateBackupClicked(){
    qDebug() << "onCreateBackupClicked() called";
}

void MainWindow::onReplaceOriginalClicked(){
    qDebug() << "onReplaceOriginalClicked() called";
}

void MainWindow::onSelectOutputFolderClicked(){
    qDebug() << "onSelectOutputFolderClicked() called";
    QString path{QFileDialog::getExistingDirectory(this, "Select a Folder", "")};

    if(!path.isEmpty()){
        ui->outPathLine->setText(path);
    }else if(ui->outPathLine->text().isEmpty()){
        ui->outFolderWarning->setText("⚠ No Folder Selected");
        ui->openOutFolderButton->setEnabled(false);
        return;
    }

    ui->outFolderWarning->clear();
    ui->openOutFolderButton->setEnabled(true);
    outPath = ui->outPathLine->text();
    qDebug() << "updated outPath to " << outPath;
}

void MainWindow::outPathLineFocusLost(){
    qDebug() << "outPathLine out of focus";
    QString path{ui->outPathLine->text()};

    if(path.isEmpty()){
        ui->outFolderWarning->setText("⚠ No Folder Entered");
        ui->openOutFolderButton->setEnabled(false);
        return;
    }else if(!QDir(path).exists()){
        ui->outFolderWarning->setText("⚠ Folder Does Not Existed");
        ui->openOutFolderButton->setEnabled(false);
        return;
    }

    ui->outFolderWarning->clear();
    ui->openOutFolderButton->setEnabled(true);
    outPath = ui->outPathLine->text();
    qDebug() << "updated outPath to " << outPath;
}

void MainWindow::suffixLineFocusLost(){
    qDebug() << "suffixLine out of focus";
}

// Bottom Row //
void MainWindow::onStartButtonClicked(){
    qDebug() << "onStartButtonClicked() called";
}

void MainWindow::onPreviewButtonClicked(){
    qDebug() << "onPreviewButtonClicked() called";
}
