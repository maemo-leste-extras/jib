// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <QMenu>
#include <QObject>
#include <QWidget>
#include <QProgressBar>
#include <QItemDelegate>

#include "ctx.h"
#include "mainwindow.h"

namespace Ui {
  class HistoryWidget;
}

class HistoryWidget : public QWidget
{
Q_OBJECT

public:
  explicit HistoryWidget(QWidget *parent = nullptr);
  ~HistoryWidget();

signals:
  void backClicked();
  void urlClicked(const QString &url);

private slots:
  void linkClicked();

private:
  void setupTable();
  void showContextMenu(const QPoint &pos);

  Ui::HistoryWidget *ui;
  AppContext *m_ctx;
  QMenu *m_contextMenu;
};

