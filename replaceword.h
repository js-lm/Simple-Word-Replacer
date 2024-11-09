#pragma once

#include <QObject>

#include <QStringList>
#include <QString>
#include <QElapsedTimer>

namespace Ui{class ReplaceWord;}

class MainWindow;

class ProgressDialog;

class ReplaceWord : public QObject{
    Q_OBJECT

public:
    ReplaceWord(MainWindow *mainWindow, ProgressDialog *progressDialog);

    void start();

public:
    bool isCancelled;

private:
    void fetchFileList();

    bool copyFile(const QString &source, const QString &destination);
    size_t startReplacing(const QString &path);
    size_t replaceWord(QString &string, Qt::CaseSensitivity isCaseSensitive);
    void writeFile(const QString &content, const QString &filename);

private:
    MainWindow *mainWindow;
    ProgressDialog *progressDialog;

    QStringList fileList;

    qint64 totalSize;

signals:
    void messager(const QString &message);
    void progress(qint64 fileSize
                  , qint64 totalFileSize
                  , size_t totalElapsedTime
                  , size_t processedFileNumber
                  , size_t totalFiles
                );
    void finished(size_t totalElapsedTime);
};
