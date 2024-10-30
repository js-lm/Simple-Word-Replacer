#pragma once

#include <QStringList>
#include <QString>

class MainWindow;

class ReplaceWord{
public:
    ReplaceWord(MainWindow *mainWindow);

    void start();

private:
    void fetchFileList();

    bool copyFile(const QString &source, const QString &destination);
    void startReplacing(const QString &path, size_t &totalReplaceCount);
    size_t replaceWord(QString &string, Qt::CaseSensitivity isCaseSensitive);
    void writeFile(const QString &content, const QString &filename);

private:
    MainWindow *mainWindow;

    QStringList fileList;

};
