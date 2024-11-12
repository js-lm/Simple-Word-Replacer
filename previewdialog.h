#pragma once

#include <QDialog>

#include "mainwindow.h"
#include <QListWidgetItem>

namespace Ui{class PreviewDialog;}

class MainWindow;

class PreviewDialog : public QDialog{
    Q_OBJECT

public:
    explicit PreviewDialog(QWidget *parent = nullptr, MainWindow *mainWinow = nullptr);
    ~PreviewDialog();

    void init();
    void fetchFiles();
    void updatePreview();

private:

    void onBackButtonClicked();
    void onFileListDoubleClicked(QListWidgetItem *selection);
    void onShowOriginalClicked();
    void onHighlightClicked();
    void onShowingDeletedWordClicked();
    void onSelectButtonClicked();

    void closeEvent(QCloseEvent *event) override;

private:
    Ui::PreviewDialog *ui;
    MainWindow *mainWindow;

    QStringList filesList;

    QString currentSelection;
};
