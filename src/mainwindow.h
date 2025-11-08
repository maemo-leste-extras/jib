#pragma once

#include <iostream>

#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QCompleter>
#include <QPushButton>
#include <QClipboard>
#include <QStringListModel>
#include <QGestureEvent>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <QQueue>
#include <QThread>
#include <QMutex>

#include "ctx.h"
#include "lib/utils.h"
#include "webwidget.h"
#include "historywidget.h"

namespace Ui {
  class MainWindow;
}

class AppContext;
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(AppContext *ctx, QString window_id, QString url, QWidget *parent = nullptr);
  static AppContext *getContext();
  static MainWindow *getInstance();
  void setupUIModels();
  void queueTask();
  void setupCompleter();
  void setURLBarText(const QString &url);
  void registerURLVisited(const QString &url);
  void resetWindowTitle();
  void showSplash();
  ~MainWindow();

  qreal screenDpiRef;
  QRect screenGeo;
  QRect screenRect;
  qreal screenDpi;
  qreal screenDpiPhysical;
  qreal screenRatio;

  bool is_fullscreen = false;

  void onQuitApplication();
  void showWebview();
  void showHistoryview();
  void showAboutview();
  void onWindowCountChanged(int count);

signals:
  void windowCountChanged(int count);
  void newWindowClicked();
  void windowClosed(QString window_id);
  void settingsClicked();
  void reloadClicked();

  void setUserAgent(QString user_agent);
  void zoomChanged(double zoomFactor);
  void JSEnabledChanged(bool status);
  void allowPDFViewerChanged(bool status);
  void allowInsecureContentChanged(bool status);
  void allowScrollbarChanged(bool status);
  void allowWebGLChanged(bool status);

protected:
  void closeEvent(QCloseEvent *event);

private:
  Ui::MainWindow *ui;
  static MainWindow *pMainWindow;
  AppContext *m_ctx = nullptr;
  QString m_window_id;

  QTimer *m_timerFavicon;
  QString m_faviconUrl;

  void resetView();
};
