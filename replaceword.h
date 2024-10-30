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

    void replaceWord(const QString &path, size_t &totalReplaceCount);
    void writeFile(const QString &content, const QString &filename);

private:
    MainWindow *mainWindow;

    QStringList fileList;

};
