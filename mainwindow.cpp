#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QDir>
#include "customeditor.h"
#include <QDesktopServices>
#include <QRegularExpression>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isFile(true)
    , isCaseSensitive(false)
    , separator(",")
    , isReplacingFileName(false)
    , isAddingSuffix(true)
    , suffix("_modified")
    , isReplacingOriginalFile(false)
    , isCreatingBackup(false)
{
    ui->setupUi(this);

    initWarnings();
    initTips();
    initConnects();
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::initWarnings(){
    ui->inFileWarning->clear();
    ui->inFolderWarning->clear();
    ui->outFolderWarning->clear();
    ui->replaceWarningLabel->clear();
    ui->separatorWarningLabel->clear();
}

void MainWindow::initTips(){
    ui->oldWordLabel->setToolTip(
        "Enter the old word(s) you want to replace.\n\n"
        "You can replace multiple old words separated by the separator of your specify.\n"
        "The number and order of old words must match the number and order of new words,\n"
        "unless you are replacing all old words with one new word."
        );
    ui->newWordLabel->setToolTip(
        "Enter the new word(s) to replace the old word(s).\n\n"
        "You can input multiple new words separated by the separator.\n"
        "The number and order of old words must match the number and order of new words.\n"
        "If you input only one new word, it will replace all old word(s).\n"
        "If you leave the new word input blank, it will remove all the old words."
        );
    ui->separatorLabel->setToolTip(
        "You can specify a separator to separate multiple words.\n\n"
        "If you leave the separator box empty, the entire input will be treated as a single word.\n\n"
        "Note:\tSpaces are treated as regular words. If you enter a space in either the\n"
        "\told or new words, it will be replaced or added just like any other word.\n"
        "\tAvoid spaces between words unless you intend to replace or add them."
        );
    ui->isAddingSuffix->setToolTip(
        "Add suffix after the processed files' names."
        );
    ui->isCreatingBackup->setToolTip(
        "Create a backup folder within the output folder.\n\n"
        "If you are replacing the original file(s),\n"
        "the backup folder will be created in the same location as the input folder."
        );
}

void MainWindow::initConnects(){
    qDebug() << "Initiating Connects in Main Window...";

    connect(ui->selectInPath, &QTabWidget::currentChanged, this, &MainWindow::onInMethodChanged);

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

    qDebug() << "Main Window Connects Initiation Completed";
}

void MainWindow::onInMethodChanged(){
    qDebug() << "onInMethodChanged() called";
    isFile = ui->selectInPath->currentIndex() == 0  ? true : false;
    qDebug() << "updated isFile to " << isFile;
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
    updateInFileLine();
}

void MainWindow::updateInFileLine(){
    qDebug() << "updateInFileLine() called";
    QString path{ui->inFileLine->text()};

    if(path.isEmpty()){
        ui->inFileWarning->setText("⚠ No File Entered");
        filePath.clear();
        qDebug() << "cleared filePath: " << filePath;
        return;
    }else if(QDir(path).exists()){
        ui->inFileWarning->setText("⚠ This Looks Like A Folder");
        filePath.clear();
        qDebug() << "cleared filePath: " << filePath;
        return;
    }else if(!QFile::exists(path)){
        ui->inFileWarning->setText("⚠ File Does Not Exist");
        filePath.clear();
        qDebug() << "cleared filePath: " << filePath;
        return;
    }

    ui->inFileWarning->clear();
    filePath = path;
    qDebug() << "updated filePath to " << filePath;
}

void MainWindow::inFolderLineFocusLost(){
    qDebug() << "inFolderLine out of focus";
    updateInFolderLine();
}

void MainWindow::updateInFolderLine(){
    qDebug() << "updateInFolderLine() called";
    QString path{ui->inFolderLine->text()};

    if(path.isEmpty()){
        ui->inFolderWarning->setText("⚠ No Folder Entered");
        folderPath.clear();
        qDebug() << "cleared folderPath: " << folderPath;
        return;
    }else if(!QDir(path).exists()){
        ui->inFolderWarning->setText("⚠ Folder Does Not Exist");
        folderPath.clear();
        qDebug() << "cleared folderPath: " << folderPath;
        return;
    }

    ui->inFolderWarning->clear();
    folderPath = path;
    qDebug() << "updated folderPath to " << folderPath;
}

// Replace //
void MainWindow::oldWordLineFocusLost(){
    qDebug() << "oldWordLine out of focus";
    removeDuplicatedSeparatorFromSearchFor();
    checkReplacementAndUpdateIfValid();
}

void MainWindow::newWordLineFocusLost(){
    qDebug() << "newWordLine out of focus";
    checkReplacementAndUpdateIfValid();
}

void MainWindow::removeDuplicatedSeparatorFromSearchFor(){
    qDebug() << "removeDuplicatedSeparatorFromSearchFor() called";
    // The reason for only removing from the search for is
    // that this way, you can batch remove words from files.

    QString searchFor{ui->oldWordLine->text()};
    QString escapedSeparator{QString("(%1)+").arg(QRegularExpression::escape(separator))};

    // removing the duplicated separator
    searchFor.replace(QRegularExpression(escapedSeparator), separator);

    auto searchForLength{searchFor.length()};

    if(searchForLength > 1){
        // removing the dangling separator
        if(searchFor.at(searchForLength - 1) == separator){
            searchFor.remove(searchForLength - 1, 1);
        }
        // removing the leading separator
        if(searchFor.at(0) == separator){
            searchFor.remove(0, 1);
        }
    }else if(searchFor == separator){
        searchFor.clear();
    }

    ui->oldWordLine->setText(searchFor);
    qDebug() << "ui->oldWordLine->setText(" << searchFor << ")";
}

void MainWindow::checkReplacementAndUpdateIfValid(){
    qDebug() << "checkReplacementAndUpdateIfValid() called";

    if(separator.isEmpty()){
        qDebug() << "separator: " << separator;
        oldWord.clear();
        oldWord.append(ui->oldWordLine->text());
        newWord.clear();
        newWord.append(ui->newWordLine->text());
        qDebug() << "updated oldWord to " << oldWord;
        qDebug() << "updated newWord to " << newWord;
        return;
    }

    QStringList oldWds{ui->oldWordLine->text().split(separator)};
    QStringList newWds{ui->newWordLine->text().split(separator)};

    oldWds.removeAll("");

    if(oldWds.isEmpty()){
        ui->replaceWarningLabel->setText("⚠ Nothing to search for");
        return;
    }else if(newWds.size() > 1 && oldWds.size() > newWds.size()){
        ui->replaceWarningLabel->setText("⚠ Size doesn't match - Too many words to search for");
        return;
    }else if(newWds.size() > 1 && oldWds.size() < newWds.size()){
        ui->replaceWarningLabel->setText("⚠ Size doesn't match - Too many words to be replaced");
        return;
    }

    ui->replaceWarningLabel->clear();

    oldWord = oldWds;
    newWord = newWds;
    qDebug() << "updated oldWord to " << oldWord;
    qDebug() << "updated newWord to " << newWord;
}

// Option Row //
void MainWindow::onCaseSensitiveClicked(){
    qDebug() << "onCaseSensitiveClicked() called";
    isCaseSensitive = ui->isCaseSensitive->isChecked();
    qDebug() << "updated isCaseSensitive to " << isCaseSensitive;
}

void MainWindow::separatorLineFocusLost(){
    qDebug() << "separatorLine out of focus";
    QString oldSep{separator};
    QString newSep{ui->separatorLine->text()};

    if(oldSep != newSep && (!oldWord.isEmpty())){
        if(ui->oldWordLine->text().contains(newSep) || ui->newWordLine->text().contains(newSep)){
            ui->separatorWarningLabel->setText("⚠ New separator conflicts with existing character. Try another. ⓘ");
            ui->separatorWarningLabel->setToolTip(
                "Separator update failed\n\n"
                "The new separator is already part of the existing character.\n"
                "Please choose a different separator."
                );
            ui->separatorLine->setText(oldSep);
            return;
        }

        ui->separatorWarningLabel->clear();

        QString tempString{ui->oldWordLine->text()};
        tempString.replace(oldSep, newSep);
        ui->oldWordLine->setText(tempString);

        tempString = ui->newWordLine->text();
        tempString.replace(oldSep, newSep);
        ui->newWordLine->setText(tempString);

        separator = newSep;
        qDebug() << "updated separator to " << separator;
    }
}

// Setting Row //
void MainWindow::onReplaceFileClicked(){
    qDebug() << "onReplaceFileClicked() called";
    isReplacingFileName = ui->isReplacingFileName->isChecked();
    qDebug() << "updated isReplacingFileName to " << isReplacingFileName;
}

void MainWindow::onAddSuffixClicked(){
    qDebug() << "onAddSuffixClicked() called";
    isAddingSuffix = ui->isAddingSuffix->isChecked();
    qDebug() << "updated isAddingSuffix to " << isAddingSuffix;

    ui->suffixLine->setEnabled(isAddingSuffix);
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
    isCreatingBackup = ui->isCreatingBackup->isChecked();
    qDebug() << "updated isCreatingBackup to " << isCreatingBackup;
}

void MainWindow::onReplaceOriginalClicked(){
    qDebug() << "onReplaceOriginalClicked() called";

    auto buttonEnabled = [this](bool enabled){
        ui->isAddingSuffix->setEnabled(enabled);
        ui->suffixLine->setEnabled(enabled);
        ui->isReplacingFileName->setEnabled(enabled);
        ui->outPathButton->setEnabled(enabled);
        ui->outPathLine->setEnabled(enabled);
        ui->openOutFolderButton->setEnabled(enabled);

        ui->outFolderWarning->clear();
    };

    if(isReplacingOriginalFile){
        buttonEnabled(true);
        updateOutPath(false);
        ui->suffixLine->setEnabled(isAddingSuffix);
    }else{
        QMessageBox::StandardButton reply{
            QMessageBox::question(this, "Confirm File Replacement",
                                  "Do you really want to replace the original files?\n\nThis action cannot be undone.\n",
                                  QMessageBox::Yes | QMessageBox::No
                                  )
        };

        if(reply == QMessageBox::No){
            ui->isReplacingOriginal->setChecked(false);
            return;
        }else{
            ui->isCreatingBackup->setChecked(true);
            isCreatingBackup = true;
            qDebug() << "updated isCreatingBackup to " << isCreatingBackup;

            buttonEnabled(false);
        }
    }

    isReplacingOriginalFile = ui->isReplacingOriginal->isChecked();
    qDebug() << "updated isReplacingOriginalFile to " << isReplacingOriginalFile;
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
    updateOutPath(true);
}

void MainWindow::updateOutPath(bool isShowingWarning){
    qDebug() << "updateOutPath() called";
    QString path{ui->outPathLine->text()};

    if(path.isEmpty()){
        if(isShowingWarning){
            ui->outFolderWarning->setText("⚠ No Folder Entered");
        }
        ui->openOutFolderButton->setEnabled(false);
        return;
    }else if(!QDir(path).exists()){
        if(isShowingWarning){
            ui->outFolderWarning->setText("⚠ Folder Does Not Exist");
        }
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
    suffix = ui->suffixLine->text();
    qDebug() << "updated suffix to " << suffix;
}

// Bottom Row //
void MainWindow::onStartButtonClicked(){
    qDebug() << "onStartButtonClicked() called";
    checkStartConditions(false);

    ReplaceWord replacer(this);
}

void MainWindow::onPreviewButtonClicked(){
    qDebug() << "onPreviewButtonClicked() called";
    checkStartConditions(true);

    // preview();
}

bool MainWindow::checkStartConditions(bool isPreviewing){
    qDebug() << "updateStartButtonStatus() called";
    qDebug() << "filePath: " << filePath;
    qDebug() << "folderPath: " << folderPath;
    qDebug() << "outPath: " << outPath;
    qDebug() << "isFile: " << isFile;
    qDebug() << "oldWord: " << oldWord;
    qDebug() << "newWord: " << newWord;
    qDebug() << "isCaseSensitive: " << isCaseSensitive;
    qDebug() << "separator: " << separator;
    qDebug() << "isReplacingFileName: " << isReplacingFileName;
    qDebug() << "isAddingSuffix: " << isAddingSuffix;
    qDebug() << "suffix: " << suffix;
    qDebug() << "isReplacingOriginalFile: " << isReplacingOriginalFile;
    qDebug() << "isCreatingBackup: " << isCreatingBackup;

    isFile ? updateInFileLine() : updateInFolderLine();
    removeDuplicatedSeparatorFromSearchFor();
    checkReplacementAndUpdateIfValid();
    updateOutPath(true);

    if(
        (isFile && filePath.isEmpty()) ||
        (!isFile && folderPath.isEmpty()) ||
        (!isReplacingOriginalFile && !isPreviewing && outPath.isEmpty()) ||
        oldWord.isEmpty()
        ){
        return false;
    }

    return true;
}
