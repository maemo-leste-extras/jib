#pragma once

#include <QObject>

#include "ctx.h"
#include "mainwindow.h"

class WindowManager : public QObject {
  Q_OBJECT

public:
  explicit WindowManager(AppContext *ctx);
  ~WindowManager() override;

  void spawn();

signals:
  void windowCountChanged(int count);

private slots:
    void onWindowClosed(QString window_id);

private:
  QMap<QString, MainWindow*> windows;
  AppContext *m_ctx;
};