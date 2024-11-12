#pragma once

#include <QDialog>

namespace Ui{class ConfirmBox;}

class ConfirmBox : public QDialog{
    Q_OBJECT

public:
    explicit ConfirmBox(QWidget *parent = nullptr, QString fileName = "");
    ~ConfirmBox();

private:
    Ui::ConfirmBox *ui;

    QString fileName;

public:
    bool shouldRemember;
    bool isCancelled;
};
