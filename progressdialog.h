#pragma once

#include <QDialog>

namespace Ui{class ProgressDialog;}

class ProgressDialog : public QDialog{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = nullptr);
    ~ProgressDialog();

private:
    Ui::ProgressDialog *ui;
};
