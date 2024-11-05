#pragma once

#include <QDialog>

#include "mainwindow.h"

namespace Ui{class PreviewDialog;}

class MainWindow;

class PreviewDialog : public QDialog{
    Q_OBJECT

public:
    explicit PreviewDialog(QWidget *parent = nullptr, MainWindow *mainWinow = nullptr);
    ~PreviewDialog();

    void init();

    void fetchFiles();

    void onBackButtonClicked();

private:
    Ui::PreviewDialog *ui;
    MainWindow *mainWindow;

    QStringList filesList;

};
