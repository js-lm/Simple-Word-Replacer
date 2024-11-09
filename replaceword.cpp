#include "replaceword.h"
#include "mainwindow.h"
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QElapsedTimer>

ReplaceWord::ReplaceWord(MainWindow *mainWindow, ProgressDialog *progressDialog)
    : mainWindow(mainWindow)
    , progressDialog(progressDialog)
    , success("<b style='color:PaleGreen;'>")
    , failure("<b style='color:Salmon;'>")
    , b("</b>")
    , sp("      ")
    , oldWord(mainWindow->oldWord)
    , canCurrentLoopContinue(true)
    , totalReplaceCount(0)
    , isCaseSensitive(mainWindow->isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive)
{
    if(mainWindow->newWord.size() > 1){
        newWord += mainWindow->newWord;
    }else{
        for(size_t i{0}; i < mainWindow->oldWord.size(); i++){
            newWord.append(mainWindow->newWord.first());
        }
    }

    qDebug() << "ReplaceWord::ReplaceWord oldWord: " << oldWord;
    qDebug() << "ReplaceWord::ReplaceWord newWord: " << newWord;
}

void ReplaceWord::start(){
    qDebug() << "start() called";

    emit messager("");

    totalTimer.start();

    fetchFiles();
    checkOutputDirectory();

    printSettings();

    if(mainWindow->isCreatingBackup){
        createBackupFolder();
    }

    if(!checkList.isEmpty()){
        emit messager("");
        emit messager(">> Cannot Proceed Due To...");
        for(const QString &message : checkList){
            emit messager(QString("%1- %2").arg(sp, message));
        }
    }else{
        emit messager("");
        emit messager(">> [Start Replacement]");

        size_t index{1};
        for(const QString &path : filePaths){
            if(!canContinue()){
                progressDialog->isCancelled = false;
                emit messager("");
                emit messager(QString("%1>> User Cancelled%2").arg(failure, b));
                break;
            }

            QElapsedTimer timer;
            timer.start();

            QString content;
            QFileInfo file{path};
            QString newFileName{file.fileName()};
            qint64 size{file.size()};

            emit messager("");
            emit messager(QString(">> Processing File[%2] - %1").arg(newFileName, QString::number(index)));

            readFile(path, content);

            if(mainWindow->isCreatingBackup){
                createBackup(path, newFileName);
            }

            replaceWord(content);

            if(mainWindow->isReplacingFileName){
                replaceFileName(newFileName);
            }
            if(mainWindow->isAddingSuffix){
                addSuffix(newFileName);
            }

            writeFile(newFileName, content);

            emit messager(QString("%1* Time elapsed: %2ms").arg(sp, QString::number(timer.elapsed())));
            emit progress(size, totalSize, totalTimer.elapsed(), index, filePaths.size());

            canCurrentLoopContinue = true;
            index++;
        }
    }

    done(totalTimer.elapsed());
    this->deleteLater();
}

void ReplaceWord::done(size_t elapsedTime){
    auto totalElapsedTime{elapsedTime};

    emit messager("");
    emit messager(QString("%1>> Replacement Completed%2").arg(success, b));
    emit messager(QString(">> Replaced total %1 word(s)").arg(QString::number(totalReplaceCount)));

    if(totalElapsedTime < 4000){
        emit messager(QString(">> Total elasped time: %1ms").arg(QString::number(totalElapsedTime)));
    }else{
        QString hour{QString::number(totalElapsedTime / 3600000).rightJustified(2, '0')};
        QString minute{QString::number(totalElapsedTime / 60000 % 60).rightJustified(2, '0')};
        QString second{QString::number(totalElapsedTime / 1000 % 60).rightJustified(2, '0')};

        emit messager(QString(">> Total elasped time: %1:%2:%3").arg(hour, minute, second));
    }

    emit finished(totalElapsedTime);
}

void ReplaceWord::fetchFiles(){
    qDebug() << "fetchFiles() called";

    emit messager("");
    emit messager(">> Fetching File(s)...");

    auto addFile = [this](const QString &file){
        QFileInfo checkFile{file};

        if(!checkFile.isFile()){
            emit messager("! This Looks Like A Folder");
            emit messager(QString("%1┕> %2").arg(sp, file));
        }else if(!checkFile.exists()){
            emit messager("! File does not exist");
            emit messager(QString("%1┕> %2").arg(sp, file));
        }else{
            emit messager(QString("%1-> %2").arg(sp, file));
            filePaths.append(file);
            totalSize += checkFile.size();
        }
    };

    if(mainWindow->isFile){
        addFile(mainWindow->filePath);
    }else{
        for(const QFileInfo &file : QDir(mainWindow->folderPath).entryInfoList(QDir::Files | QDir::NoDotAndDotDot)){
            addFile(file.absoluteFilePath());
        }
    }

    if(filePaths.isEmpty()){
        checkList.append("Unable to fetch any file");
    }

    qDebug() << "filePaths " << filePaths;
}

void ReplaceWord::checkOutputDirectory(){
    qDebug() << "checkOutputDirectory() called";

    emit messager("");
    emit messager(">> Checking Output Directory...");

    if(mainWindow->isReplacingOriginalFile){
        if(mainWindow->isFile){
            QString backupPath{mainWindow->filePath};
            QFileInfo fileInfo(backupPath);
            outputDirectory = fileInfo.absolutePath();
        }else{
            outputDirectory = mainWindow->folderPath;
        }
    }else{
        outputDirectory = mainWindow->outPath;
    }

    if(!QDir(outputDirectory).exists()){
        emit messager(QString("%1* Output folder does not exist, creating a new one...").arg(sp));

        if(!QDir().mkdir(outputDirectory)){
            emit messager(QString("%3%1! Failed to create output folder%2").arg(failure, b, sp));
            emit messager(QString("%1%1┕> %2").arg(sp, outputDirectory));

            checkList.append("Failed to create output folder");
        }else{
            emit messager(QString("%1! Created output folder").arg(sp));
            emit messager(QString("%1%1┕> %2").arg(sp, outputDirectory));
        }
    }else{
        emit messager(QString("%1! Found existed folder").arg(sp));
        emit messager(QString("%1%1┕> %2").arg(sp, outputDirectory));
    }
}

void ReplaceWord::printSettings(){
    qDebug() << "printSettings() called";

    emit messager("");
    emit messager(">> Search And Replace");

    for(size_t i{0}; i < oldWord.size(); i++){
        if(newWord[i] == ""){
            emit messager(QString("%4- Remove %1%2%3").arg(failure, oldWord[i], b, sp));
        }else{
            emit messager(QString("%6~ Replace %1%3%5 with %2%4%5").arg(failure, success, oldWord[i], newWord[i], b, sp));
        }
    }

    emit messager("");
    emit messager(">> Settings");

    QString yesNo{mainWindow->isCaseSensitive ? "Yes" : "No"};
    emit messager(QString("%1? Is case sensitive: %2").arg(sp, yesNo));
    yesNo = mainWindow->isReplacingOriginalFile ? "Yes" : "No";
    emit messager(QString("%1? Is replacing original file: %2").arg(sp, yesNo));
    if(yesNo == "No"){
        yesNo = mainWindow->isReplacingFileName ? "Yes" : "No";
        emit messager(QString("%1? Is replacing file name: %2").arg(sp, yesNo));
        yesNo = mainWindow->isAddingSuffix ? "Yes [" + mainWindow->suffix + "]" : "No";
        emit messager(QString("%1? Is adding suffix: %2").arg(sp, yesNo));
    }
}

void ReplaceWord::createBackupFolder(){
    qDebug() << "createBackupFolder() called";

    emit messager("");
    emit messager(">> Checking Backup Directory...");

    backupDestination = QDir(outputDirectory).filePath("Backup");

    if(!QDir(backupDestination).exists()){
        emit messager(QString("%1* Backup folder does not exist, creating a new one...").arg(sp));

        if(!QDir().mkdir(backupDestination)){
            emit messager(QString("%3%1! Failed to create backup folder%2").arg(failure, b, sp));
            emit messager(QString("%1%1┕> %2").arg(sp, backupDestination));

            checkList.append("Failed to create backup folder");
        }else{
            emit messager(QString("%1! Created backup folder").arg(sp));
            emit messager(QString("%1%1┕> %2").arg(sp, backupDestination));
        }
    }else{
        emit messager(QString("%1! Found existed folder").arg(sp));
        emit messager(QString("%1%1┕> %2").arg(sp, backupDestination));
    }
}

void ReplaceWord::readFile(const QString &path, QString &content){
    qDebug() << "readFile() called";
    if(!canCurrentLoopContinue || !canContinue()) return;

    QFile file{path};

    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        content = in.readAll();
        file.close();

        emit messager(QString("%1* Successfully read the file").arg(sp));
    }else{
        emit messager(QString("%1%2* Unable to read the file%3").arg(sp, failure, b));

        canCurrentLoopContinue = false;
    }
}

void ReplaceWord::createBackup(const QString &path, const QString &newFileName){
    qDebug() << "createBackup() called";
    if(!canCurrentLoopContinue || !canContinue()) return;

    QString destination{QDir(backupDestination).filePath(newFileName)};

    if(QFile(destination).exists()){
        QFile::remove(destination);
    }

    if(QFile::copy(path, destination)){
        emit messager(QString("%1* Created backup in").arg(sp));
        emit messager(QString("%1%1┕> %2").arg(sp, destination));
    }else{
        emit messager(QString("%3%1* Unable to create back up for%2").arg(failure, b, sp));
        emit messager(QString("%1%1┕> %2").arg(sp, destination));
        emit messager(QString("%1%1- Skipping this file...").arg(sp));
        canCurrentLoopContinue = false;
    }
}

void ReplaceWord::replaceWord(QString &content){
    qDebug() << "replaceWord() called";
    if(!canCurrentLoopContinue || !canContinue()) return;

    size_t replaceCount{0};

    for(size_t i{0}; i < oldWord.size() && canContinue(); i++){
        replaceCount = content.count(oldWord[i], isCaseSensitive);

        if(replaceCount == 0){
            emit messager(QString("%1* Didn't find [%2]").arg(sp, oldWord[i]));
        }else{
            content.replace(oldWord[i], newWord[i], isCaseSensitive);
            emit messager(QString("%1* Found %2 instance%3 of [%4]").arg(sp, QString::number(replaceCount), replaceCount == 1 ? "" : "s", oldWord[i], newWord[i]));

            if(newWord[i] == ""){
                emit messager(QString("%1%1┕> Removed").arg(sp));
            }else{
                emit messager(QString("%1%1┕> Replaced with [%2]").arg(sp, newWord[i]));
            }
        }
    }

    totalReplaceCount += replaceCount;
}

void ReplaceWord::replaceFileName(QString &newFileName){
    qDebug() << "replaceFileName() called";
    if(!canCurrentLoopContinue || !canContinue()) return;

    for(size_t i{0}; i < oldWord.size(); i++){
        newFileName.replace(oldWord[i], newWord[i], isCaseSensitive);
    }
    emit messager(QString("%1* Renamed file to \"%2\"").arg(sp, newFileName));
}

void ReplaceWord::addSuffix(QString &newFileName){
    qDebug() << "addSuffix() called";
    if(!canCurrentLoopContinue || !canContinue()) return;

    auto dotIndex{newFileName.lastIndexOf('.')};

    if(dotIndex == -1){
        newFileName += mainWindow->suffix;
    }else{
        newFileName.insert(dotIndex, mainWindow->suffix);
    }

    emit messager(QString("%1* Added suffix to \"%2\"").arg(sp, newFileName));
}

void ReplaceWord::writeFile(const QString &newFileName, const QString &content){
    qDebug() << "writeFile() called";
    if(!canCurrentLoopContinue) return;

    QString outPath{QDir(outputDirectory).filePath(newFileName)};
    QFile file{outPath};

    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        out << content;
        file.close();

        emit messager(QString("%1* Successfully wrote the file").arg(sp));
    }else{
        emit messager(QString("%1%2* Unable to write the file%3").arg(sp, failure, b));

        canCurrentLoopContinue = false;
    }
}

bool ReplaceWord::canContinue(){
    if(progressDialog->isCancelled){
        return false;
    }
    return true;
}
