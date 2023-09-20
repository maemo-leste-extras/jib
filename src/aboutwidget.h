// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <QMenu>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QWebEngineProfile>
#include <QStringListModel>
#include <QGestureEvent>
#include <QItemDelegate>
#include <QProgressBar>
#include <QHeaderView>
#include <QFileInfo>

#include "ctx.h"

namespace Ui {
  class AboutWidget;
}

class AboutWidget : public QWidget
{
Q_OBJECT

public:
  explicit AboutWidget(QWidget *parent = nullptr);
  ~AboutWidget();

signals:
  void backClicked();
  void urlClicked(const QString &url);

private:
  AppContext *m_ctx;
  Ui::AboutWidget *ui;
};
