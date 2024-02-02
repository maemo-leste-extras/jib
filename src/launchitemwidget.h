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
#include <QPixmap>

#include "ctx.h"

namespace Ui {
  class LaunchItemWidget;
}

class LaunchItemWidget : public QWidget
{
Q_OBJECT

public:
  explicit LaunchItemWidget(AppContext *ctx, const QString &title, const QString &domain, QWidget *parent = nullptr);
  ~LaunchItemWidget();

signals:
  void backClicked();
  void urlClicked(const QString &url);

private:
  QString m_title;
  QString m_titleShort;
  QString m_domain;
  AppContext *m_ctx;
  Ui::LaunchItemWidget *ui;
  QList<LaunchItemWidget*> launchWidgets;
};
