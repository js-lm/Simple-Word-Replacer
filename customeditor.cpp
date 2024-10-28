#include "customeditor.h"

CustomLineEdit::CustomLineEdit(QWidget *parent) : QLineEdit(parent){}

void CustomLineEdit::focusOutEvent(QFocusEvent *event){
    QLineEdit::focusOutEvent(event);
    emit focusLost();
}

CustomPlainTextEdit::CustomPlainTextEdit(QWidget *parent) : QPlainTextEdit(parent){}

void CustomPlainTextEdit::focusOutEvent(QFocusEvent *event){
    QPlainTextEdit::focusOutEvent(event);
    emit focusLost();
}
