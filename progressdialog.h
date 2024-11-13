#pragma once

#include <QDialog>
#include <QTime>
#include "mainwindow.h"

namespace Ui{class ProgressDialog;}

class MainWindow;

class ProgressDialog : public QDialog{
    Q_OBJECT

public:
    explicit ProgressDialog(MainWindow *mainWindow = nullptr);
    ~ProgressDialog();

    void init();
    void updateEstimatedTimeDisplay();
    void updateProgress(qint64 currentFileSize
                        , qint64 totalFileSize
                        , size_t totalElapsedTime
                        , size_t processedFileNumber
                        , size_t totalFiles
                        );
    void updateLogScreen(const QString &message);
    void replaceCompleted();
    void updateEstimatedTimerPause(bool isPausing);

public:
    bool isCancelled;

private:
    Ui::ProgressDialog *ui;
    MainWindow *mainWindow;

    QTimer *timer;

    size_t estimatedTime;
    qint64 totalProcessedSize;

    bool isPausingEstimatedTimer;

private:
    void updateButtons(bool isFinished);

    void closeEvent(QCloseEvent *event) override;
    void onNextButtonClicked();
};
