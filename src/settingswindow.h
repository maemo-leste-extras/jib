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
  class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
Q_OBJECT

public:
  explicit SettingsWindow(QWidget *parent = nullptr);
  ~SettingsWindow();

signals:
  void windowClosing();
  void setUserAgent(QString user_agent);
  void zoomChanged(double zoomFactor);
  void JSEnabledChanged(bool status);
  void allowPDFViewerChanged(bool status);
  void allowInsecureContentChanged(bool status);
  void allowScrollbarChanged(bool status);
  void allowWebGLChanged(bool status);
  void adblockFiltersEnabledChanged(bool state);
  void adblockAbpEnabledChanged(bool state);
  void adblockPrivacyEnabledChanged(bool state);
  void adblockAbuseEnabledChanged(bool state);
  void adblockUnbreakEnabledChanged(bool state);

protected:
    void closeEvent(QCloseEvent* event);

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
  Ui::SettingsWindow *ui;
};