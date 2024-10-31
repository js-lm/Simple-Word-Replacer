#include "replaceword.h"
#include "mainwindow.h"
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QElapsedTimer>

ReplaceWord::ReplaceWord(MainWindow *mainWindow)
    : mainWindow(mainWindow)
{
    fetchFileList();
}

void ReplaceWord::start(){
    size_t replaceCount{0};
    auto totalFiles{fileList.size()};
    size_t fileIndex{0};

    QElapsedTimer totalTimer;
    totalTimer.start();

    for(const QString &path : fileList){
        fileIndex++;

        emit messager("Current File: " + path);

        QFile file(path);

        if(!file.open(QFile::ReadWrite)){
            emit messager("Unable to open " + path + "\nSkipping this file...");
            continue;
        }

        qint64 size{file.size()};
        file.close();

        if(mainWindow->isCreatingBackup){
            if(!copyFile(path, mainWindow->outPath)){
                emit messager("Unable to create back up for " + path + "\nSkipping this file...");
                continue;
            }
        }

        QElapsedTimer timer;
        timer.start();

        size_t wordReplaceCount{startReplacing(path)};
        replaceCount += wordReplaceCount;

        auto elapsedTime{timer.elapsed()};

        emit progress(totalFiles
                      , fileIndex
                      , path
                      , size
                      , elapsedTime
                      , wordReplaceCount
                    );
    }

    auto totalElapsedTime{totalTimer.elapsed()};

    emit finished(totalElapsedTime);
    delete this;
}

void ReplaceWord::fetchFileList(){
    qDebug() << "fetchFileList() called";

    fileList.clear();

    if(mainWindow->isFile){
        fileList.append(mainWindow->filePath);
    }else{
        QString folderPath{mainWindow->folderPath};
        QDir directory(folderPath);

        if(!directory.exists()){
            QMessageBox::warning(nullptr, "Folder Does Not Exist", "Folder " + folderPath + " does not exist.");
            qDebug() << "directory" << folderPath << " does not exist";
            return;
        }

        QStringList relativeFileList{directory.entryList(QDir::Files)};
        for(const QString &fileName : relativeFileList){
            fileList.append(directory.absoluteFilePath(fileName));
        }
        qDebug() << "updated fileList to " << fileList;
    }
}

bool ReplaceWord::copyFile(const QString &source, const QString &destination){
    qDebug() << "copyFile() called";
    QFileInfo fileInfo{source};

    if(QFile::copy(source, destination + "/Backup/" + fileInfo.fileName())){
        qDebug() << "File copied successfully " << source;
        return true;
    }

    qDebug() << "Failed to copy file " << source;
    return false;
}

size_t ReplaceWord::startReplacing(const QString &path){
    qDebug() << "startReplacing() called";
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "unable to open file for reading " << path;
        return 0;
    }else{
        qDebug() << "opened " << path;
    }

    QTextStream in(&file);
    QString content{in.readAll()};
    file.close();

    Qt::CaseSensitivity isCaseSensitive{mainWindow->isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive};

    size_t wordReplaceCount{replaceWord(content, isCaseSensitive)};
    qDebug() << "replaced " << wordReplaceCount << " words in " << path;

    QFileInfo fileInfo{path};
    QString outFileName{fileInfo.fileName()};
    qDebug() << "created outFileName = " << outFileName;

    if(mainWindow->isReplacingFileName){
        replaceWord(outFileName, isCaseSensitive);
    }

    qDebug() << "mainWindow->isAddingSuffix: " << mainWindow->isAddingSuffix;
    if(mainWindow->isAddingSuffix){
        auto dotIndex{outFileName.lastIndexOf('.')};

        if(dotIndex == -1){
            outFileName += mainWindow->suffix;
        }else{
            outFileName.insert(dotIndex, mainWindow->suffix);
        }
        qDebug() << "updated outFileName to " << outFileName;
        qDebug() << "mainWindow->suffix: " << mainWindow->suffix;
    }

    writeFile(content, mainWindow->outPath + QDir::separator() + outFileName);

    return wordReplaceCount;
}

size_t ReplaceWord::replaceWord(QString &string, Qt::CaseSensitivity isCaseSensitive){
    qDebug() << "replaceWord() called";
    size_t replaceCount{0};

    if(mainWindow->newWord.size() <= 1){
        for(const QString& oldWord : mainWindow->oldWord){
            replaceCount += string.count(oldWord, isCaseSensitive);
            string.replace(oldWord, mainWindow->newWord[0], isCaseSensitive);
        }
    }else{
        for(size_t i{0}; i < mainWindow->oldWord.size(); i++){
            replaceCount += string.count(mainWindow->oldWord[i], isCaseSensitive);
            string.replace(mainWindow->oldWord[i], mainWindow->newWord[i], isCaseSensitive);
        }
    }

    return replaceCount;
}

void ReplaceWord::writeFile(const QString &content, const QString &path){
    qDebug() << "writeFile() called";
    QFile file(path);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "unable to open file for writing " << path;
        return;
    }

    QTextStream out(&file);
    out << content;
    file.close();
}
