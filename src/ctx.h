#pragma once

#include <QObject>
#include <QMainWindow>
#include <QCoreApplication>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QQueue>
#include <QThread>
#include <QMutex>

#include "lib/config.h"
#include "historymodel.h"


class AppContext : public QObject {
Q_OBJECT

public:
  explicit AppContext();
  ~AppContext() override;

  double zoomFactor = 1.0;
  bool isDebug;
  QString preloadModel;
  QString configDirectory;
  QString iconCacheDirectory;
  QString configRoot;
  QString homeDir;

  QString currentUrl;
  QString pageTitle = "";
  bool is_loading = false;

  QString uaDesktop = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/117.0";
  QString uaMobile = "Mozilla/5.0 (Linux; Android 10; SM-A205U) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.5938.60 Mobile Safari/537.36";
  QString ua = uaMobile;

  HistoryModel *historyModel;
  QString dbFile;
  QSqlDatabase db;

  void initdb();
  void findDomainIcon(const QString &domain);
  void SqlCreateSchema();
  static QSqlQuery SqlExec(QSqlQuery &q);
  QSqlQuery SqlExec(const QString &q);
  static void SqlExecError(const QSqlQuery &q);
  QIcon* getThumbIcon(const QString &domain);

private:
  void createConfigDirectory(const QString &dir);
  QMap<QString, QIcon*> m_cacheIcons;
};
