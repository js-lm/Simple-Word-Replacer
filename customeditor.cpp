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

QString CustomPlainTextEdit::text() const {
    return toPlainText();
}

void CustomPlainTextEdit::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter){
        event->ignore();
    }else{
        QPlainTextEdit::keyPressEvent(event);
    }
}

void CustomPlainTextEdit::setText(const QString &text){
    setPlainText(text);
}
