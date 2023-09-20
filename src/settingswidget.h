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
  class SettingsWidget;
}

class SettingsWidget : public QWidget
{
Q_OBJECT

public:
  explicit SettingsWidget(QWidget *parent = nullptr);
  ~SettingsWidget();

signals:
  void setUserAgent(QString user_agent);
  void zoomChanged(double zoomFactor);
  void backClicked();
  void historyClicked();
  void aboutClicked();
  void JSEnabledChanged(bool status);
  void allowPDFViewerChanged(bool status);
  void allowInsecureContentChanged(bool status);
  void allowScrollbarChanged(bool status);
  void allowWebGLChanged(bool status);

private slots:
  void onZoomChanged(int idx);
  void onUAChanged(int idx);
  void onJSChanged(int idx);
  void onAllowInsecureContentChanged(int idx);
  void onAllowPDFViewerChanged(int idx);
  void onAllowScrollbarChanged(int idx);
  void onAllowWebGLChanged(int idx);

private:
  AppContext *m_ctx;
  Ui::SettingsWidget *ui;
};