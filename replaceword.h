#pragma once

#include <QObject>

#include <QStringList>
#include <QString>
#include <QElapsedTimer>

namespace Ui{class ReplaceWord;}

class MainWindow;

class ReplaceWord : public QObject{
    Q_OBJECT

public:
    ReplaceWord(MainWindow *mainWindow);

    void start();

private:
    void fetchFileList();

    bool copyFile(const QString &source, const QString &destination);
    size_t startReplacing(const QString &path);
    size_t replaceWord(QString &string, Qt::CaseSensitivity isCaseSensitive);
    void writeFile(const QString &content, const QString &filename);

private:
    MainWindow *mainWindow;

    QStringList fileList;

signals:
    void messager(const QString &message);
    void progress(size_t totalFiles
                  , size_t currentIndex
                  , const QString &filePath
                  , qint64 fileSize
                  , size_t elapsedTime
                  , size_t wordReplaceCount
                );
    void finished(size_t totalElapsedTime);
};
