#pragma once

#include <QDialog>

namespace Ui{class PreviewDialog;}

class PreviewDialog : public QDialog{
    Q_OBJECT

public:
    explicit PreviewDialog(QWidget *parent = nullptr);
    ~PreviewDialog();

private:
    Ui::PreviewDialog *ui;
};
