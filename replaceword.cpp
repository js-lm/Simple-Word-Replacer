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
    //auto totalFiles{fileList.size()};
    size_t fileIndex{0};

    QElapsedTimer totalTimer;
    totalTimer.start();

    emit messager("> File list:\n" + fileList.join("\n"));
    emit messager("> Searching for: " + mainWindow->oldWord.join(mainWindow->separator));
    emit messager("> Replacing with: " + mainWindow->newWord.join(mainWindow->separator));
    emit messager("> Separator: " + mainWindow->separator);
    emit messager("> Is case sensitive: " + QString::number(mainWindow->isCaseSensitive));

    for(const QString &path : fileList){
        fileIndex++;

        emit messager("\n> Opening File...");
        emit messager("> File [" + QString::number(fileIndex) + "] " + path);

        QFile file(path);

        if(!file.open(QFile::ReadWrite)){
            emit messager("Unable to open " + path + "\nSkipping this file...");
            continue;
        }

        qint64 size{file.size()};
        file.close();

        if(mainWindow->isCreatingBackup){
            emit messager("Creating backup... ");
            if(!copyFile(path, mainWindow->outPath)){
                emit messager("Unable to create back up for " + path + "\nSkipping this file...");
                continue;
            }
            emit messager("Created backup");
        }

        emit messager("Searching...");

        QElapsedTimer timer;
        timer.start();

        size_t wordReplaceCount{startReplacing(path)};
        replaceCount += wordReplaceCount;

        emit messager("Time elapsed: " + QString::number(timer.elapsed()) + "ms");

        emit progress(size
                      , totalSize
                      , totalTimer.elapsed()
                      , fileIndex
                      , fileList.size()
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

        for(const QString &filePath : fileList){
            QFile file(filePath);
            if(file.open(QIODevice::ReadOnly)){
                totalSize += file.size();
                file.close();
            }else{
                qDebug() << "unable to open file: " << filePath;
            }
        }
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

    // Read
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "unable to open file for reading " << path;
        emit messager("Unable to open file for reading: " + path);
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
    emit messager("Replaced " + QString::number(wordReplaceCount) + " word");

    if(mainWindow->isReplacingOriginalFile && wordReplaceCount == 0){
        return 0;
    }

    // Write
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
    emit messager("Created " + outFileName);

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
