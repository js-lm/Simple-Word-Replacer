#pragma once

#include <QDialog>

namespace Ui{class ProgressDialog;}

class ProgressDialog : public QDialog{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();

    void init();

    void updateProgress(qint64 fileSize
                        , qint64 totalFileSize
                        , size_t totalElapsedTime
                        , size_t processedFileNumber
                        , size_t totalFiles
                        );

    void updateLogScreen(const QString &message);

    void replaceCompleted();

    void updateButtons(bool isFinished);

private:
    Ui::ProgressDialog *ui;

    size_t estimatedTime;

    qint64 processedSize;

private:

    void updateEstimatedTime();
};
