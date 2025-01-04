#pragma once
#include <QObject>
#include <QDebug>
#include <QEvent>
#include <QLineEdit>
#include <QMouseEvent>

class QLineEditFocus : public QLineEdit
{
  Q_OBJECT

public:
  explicit QLineEditFocus(QWidget *parent = nullptr);
  ~QLineEditFocus() override;

  signals:
    void showSuggestions(bool inFocus);

protected:
  bool eventFilter(QObject *obj, QEvent *ev) override;
};
