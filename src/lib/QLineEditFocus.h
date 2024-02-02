#pragma once
#include <QObject>
#include <QLineEdit>
#include <QMouseEvent>

class QLineEditFocus : public QLineEdit {
  Q_OBJECT

public:
  QLineEditFocus(QWidget *parent = nullptr);
  ~QLineEditFocus();

signals:
  void focussed(bool hasFocus);

protected:
  virtual void focusInEvent(QFocusEvent *e);
  virtual void focusOutEvent(QFocusEvent *e);
};