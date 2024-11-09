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

private:
    void fetchFiles();
    void checkOutputDirectory();
    void printSettings();
    void createBackupFolder();

    void readFile(const QString &path, QString &content);
    void createBackup(const QString &path, const QString &newFileName);
    void replaceWord(QString &content);
    void replaceFileName(QString &newFileName);
    void addSuffix(QString &newFileName);
    void writeFile(const QString &newFileName, const QString &content);

    bool canContinue();

    void done(size_t elapsedTime);

private:
    MainWindow *mainWindow;
    ProgressDialog *progressDialog;

    QString success;
    QString failure;
    QString b;
    QString sp;

    QStringList checkList;

    bool isFile;
    Qt::CaseSensitivity isCaseSensitive;
    bool canCurrentLoopContinue;

    QStringList filePaths;
    const QStringList &oldWord;
    QStringList newWord;
    QString outputDirectory;
    QString backupDestination;

    size_t totalReplaceCount;
    QElapsedTimer totalTimer;
    qint64 totalSize;

signals:
    void messager(const QString &message);
    void progress(qint64 fileSize, qint64 totalFileSize, size_t totalElapsedTime, size_t processedFileNumber, size_t totalFiles);
    void finished(size_t totalElapsedTime);
};
