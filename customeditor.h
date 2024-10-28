#pragma once

#include <QLineEdit>
#include <QPlainTextEdit>
#include <QFocusEvent>

class CustomLineEdit : public QLineEdit{
    Q_OBJECT

public:
    explicit CustomLineEdit(QWidget *parent = nullptr);

signals:
    void focusLost();

protected:
    void focusOutEvent(QFocusEvent *event) override;
};

class CustomPlainTextEdit : public QPlainTextEdit{
    Q_OBJECT

public:
    explicit CustomPlainTextEdit(QWidget *parent = nullptr);

signals:
    void focusLost();

protected:
    void focusOutEvent(QFocusEvent *event) override;
};
