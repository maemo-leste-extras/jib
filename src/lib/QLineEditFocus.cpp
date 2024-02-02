#include "QLineEditFocus.h"

QLineEditFocus::QLineEditFocus(QWidget *parent)
 : QLineEdit(parent) {}

QLineEditFocus::~QLineEditFocus() {}

void QLineEditFocus::focusInEvent(QFocusEvent *e) {
  QLineEdit::focusInEvent(e);
  emit(focussed(true));
}

void QLineEditFocus::focusOutEvent(QFocusEvent *e) {
  QLineEdit::focusOutEvent(e);
  emit(focussed(false));
}