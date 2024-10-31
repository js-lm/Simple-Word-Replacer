#pragma once

#include <QDialog>

namespace Ui{class ProgressDialog;}

class ProgressDialog : public QDialog{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();

    void init();

    void updateProgress(size_t totalFiles
                        , size_t processedFileNumber
                        , const QString &filePath
                        , qint64 fileSize
                        , size_t elapsedTime
                        , size_t wordReplaceCount
                    );

    void updateLogScreen(const QString &message);

    void replaceCompleted();

    void updateButtons(bool isFinished);

private:
    Ui::ProgressDialog *ui;

    void updateEstimatedTime();
};
