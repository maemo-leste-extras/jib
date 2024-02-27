#pragma once
#include <QTableView>
#include <QAbstractItemView>
#include <QMouseEvent>

class ClickTable : public QTableView {
  Q_OBJECT
public:
  explicit ClickTable(QWidget *parent = nullptr) : QTableView(parent) {}
protected:
  void mousePressEvent(QMouseEvent *event) override {
    QTableView::mousePressEvent(event);
    QModelIndex modelIdx = this->indexAt(event->pos());
    if(!modelIdx.isValid()) {
      emit emptySpaceClicked();
    }
  }
signals:
  void clicked();
  void emptySpaceClicked();
};