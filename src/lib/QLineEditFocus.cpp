#include "QLineEditFocus.h"


QLineEditFocus::QLineEditFocus(QWidget *parent) : QLineEdit(parent) {
  installEventFilter(this);
}

QLineEditFocus::~QLineEditFocus() = default;

bool QLineEditFocus::eventFilter(QObject* obj, QEvent* ev) {
  if(obj && ev->type() == QEvent::MouseButtonPress) {
   emit showSuggestions(true);
   return false;
  }
  return false;
}
