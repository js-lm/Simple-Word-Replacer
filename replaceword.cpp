#include "replaceword.h"
#include "mainwindow.h"
#include <QDir>
#include <QMessageBox>
#include <QString>

ReplaceWord::ReplaceWord(MainWindow *mainWindow)
    : mainWindow(mainWindow)
{
    qDebug() << "constructed ReplaceWord";

    fetchFileList();

    size_t replaceCount{0};

    for(const QString &path : fileList){
        if(mainWindow->isCreatingBackup){
            if(!copyFile(path, mainWindow->outPath)){
                QMessageBox::information(nullptr, "Unable to create backup",
                                         "Unable to create backup for" + path + "\nSkipping this file."
                                         );
                continue;
            }
        }
        startReplacing(path, replaceCount);
    }
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

void ReplaceWord::startReplacing(const QString &path, size_t &totalReplaceCount){
    qDebug() << "startReplacing() called";
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "unable to open file for reading " << path;
        return;
    }else{
        qDebug() << "opened " << path;
    }

    QTextStream in(&file);
    QString content{in.readAll()};
    file.close();

    Qt::CaseSensitivity isCaseSensitive{mainWindow->isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive};

    size_t fileReplaceCount{replaceWord(content, isCaseSensitive)};
    totalReplaceCount += fileReplaceCount;

    qDebug() << "replaced " << fileReplaceCount << " words in " << path;

    QFileInfo fileInfo{path};
    QString outFileName{fileInfo.fileName()};
    qDebug() << "created outFilePath = " << outFileName;

    if(mainWindow->isReplacingFileName){
        replaceWord(outFileName, isCaseSensitive);
    }

    if(mainWindow->isAddingSuffix){
        auto dotIndex{outFileName.lastIndexOf('.')};

        if(dotIndex == -1){
            outFileName += mainWindow->suffix;
        }else{
            outFileName.insert(dotIndex, mainWindow->suffix);
        }
    }

    writeFile(content, mainWindow->outPath + QDir::separator() + outFileName);
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
