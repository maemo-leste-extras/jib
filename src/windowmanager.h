#pragma once

#include <QObject>

#include "ctx.h"
#include "mainwindow.h"
#include "settingswindow.h"
#include "lib/adblock/AdBlockManager.h"

class WindowManager : public QObject {
  Q_OBJECT

public:
  explicit WindowManager(AppContext *ctx, QWidget* parent = nullptr);
  ~WindowManager() override;

public slots:
  void onSpawn();
  void onSpawnSettings();

signals:
  void windowCountChanged(int count);

private slots:
    void onWindowClosed(QString window_id);

private:
  QMap<QString, MainWindow*> windows;
  SettingsWindow* m_settingsWindow = nullptr;
  AppContext *m_ctx;
};