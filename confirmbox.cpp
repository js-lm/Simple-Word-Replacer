#include "confirmbox.h"
#include "ui_confirmbox.h"

#include <QMessageBox>

ConfirmBox::ConfirmBox(QWidget *parent, QString fileName)
    : QDialog(parent)
    , ui(new Ui::ConfirmBox)
    , fileName(fileName)
    , shouldRemember(false)
    , isCancelled(false)
{
    ui->setupUi(this);

    ui->warningLabel->setText(QString("The output folder already has a file named %1").arg(fileName));

    connect(ui->replaceButton, &QPushButton::clicked, this, [this](){accept();});
    connect(ui->skipButton, &QPushButton::clicked, this, [this](){reject();});
    connect(ui->rememberMyChoice, &QPushButton::clicked, this, [this](){
        shouldRemember = ui->rememberMyChoice->isChecked();
    });
    connect(ui->cancelButton, &QPushButton::clicked, this, [this](){
        QMessageBox::StandardButton reply{QMessageBox::question(this, "Are you sure you want to cancel?"
                                           , "Are you sure you want to cancel"
                                           , QMessageBox::Yes | QMessageBox::No
                                           )};
        if(reply == QMessageBox::Yes){
            isCancelled = true;
            reject();
        }
    });
}

ConfirmBox::~ConfirmBox(){
    delete ui;
}
