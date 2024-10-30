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
        replaceWord(path, replaceCount);
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

        fileList = directory.entryList(QDir::Files);
        qDebug() << "updated fileList to " << fileList;
    }
}

void ReplaceWord::replaceWord(const QString &path, size_t &totalReplaceCount){
    qDebug() << "replaceWord() called";
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "unable to open file " << path;
        return;
    }else{
        qDebug() << "opened " << path;
    }

    QTextStream in(&file);
    QString content{in.readAll()};
    file.close();

    size_t fileReplaceCount{0};

    auto isCaseSensitive{mainWindow->isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive};

    if(mainWindow->newWord.size() <= 1){
        for(const QString& oldWord : mainWindow->oldWord){
            fileReplaceCount += content.count(oldWord, isCaseSensitive);
            totalReplaceCount += fileReplaceCount;
            content.replace(oldWord, mainWindow->newWord[0], isCaseSensitive);
        }
    }else{
        for(size_t i{0}; i < mainWindow->oldWord.size(); i++){
            fileReplaceCount += content.count(mainWindow->oldWord[i], isCaseSensitive);
            totalReplaceCount += fileReplaceCount;
            content.replace(mainWindow->oldWord[i], mainWindow->newWord[i], isCaseSensitive);
        }
    }

    qDebug() << "replaced " << fileReplaceCount << " words in " << path;
}

void ReplaceWord::writeFile(const QString &content, const QString &path){
    qDebug() << "writeFile() called";
    QFile file(path);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        qDebug() << "unable to open file " << path;
        return;
    }
}
