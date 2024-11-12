#pragma once

#include <QObject>

#include <QStringList>
#include <QString>
#include <QElapsedTimer>
#include <qwidget.h>

namespace Ui{class ReplaceWord;}

class MainWindow;
class ProgressDialog;
class ConfirmBox;

class ReplaceWord : public QWidget{
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
    void writeFile(const QString &newFileName, const QString &content, quint64 &confirmBoxElapsedTime, qint64 &currentFileSize);

    bool canContinue();

    void done();

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
    bool replacementCancelled;

    QStringList filePaths;
    const QStringList &oldWord;
    QStringList newWord;
    QString outputDirectory;
    QString backupDestination;

    size_t totalReplaceCount;
    size_t totalActualNumberOfFileReplaced;
    quint64 totalTime;
    qint64 totalSize;

    bool shouldRemember;
    bool shouldAlwaysOverwrite;

signals:
    void messager(const QString &message);
    void progress(qint64 fileSize, qint64 totalFileSize, size_t totalElapsedTime, size_t processedFileNumber, size_t totalFiles);
    void finished(size_t totalElapsedTime);
    void pauseEstimatedTimer(bool isPausing);
};
