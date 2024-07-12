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
#include <QFontDatabase>

#include "lib/config.h"
#include "lib/adblock/AdBlockManager.h"
#include "historymodel.h"
#include "popularitymodel.h"
#include "suggestionmodel.h"


class AppContext : public QObject {
Q_OBJECT

public:
  explicit AppContext(QCommandLineParser *cmdargs);
  ~AppContext() override;

  double zoomFactor = 1.0;
  bool isDebug;
  QCommandLineParser *cmdargs;
  QString preloadModel;
  QString configDirectory;
  QString adblockDirectory;
  QString iconCacheDirectory;
  QString configRoot;
  QString homeDir;

  QString currentUrl;
  QString pageTitle = "";
  bool is_loading = false;

  QString uaDesktop = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/117.0";
  QString uaMobile = "Mozilla/5.0 (Linux; Android 10; SM-A205U) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.5938.60 Mobile Safari/537.36";
  QString ua = uaMobile;

  adblock::AdBlockManager *adblock;
  HistoryModel *historyModel;
  PopularSites *popularSites;
  SuggestionModel *suggestionModel;
  QString dbFile;
  QSqlDatabase db;

  void initdb();
  QFont iconFont() const;
  void findDomainIcon(const QString &domain);
  void SqlCreateSchema();
  static QSqlQuery SqlExec(QSqlQuery &q);
  QSqlQuery SqlExec(const QString &q);
  static void SqlExecError(const QSqlQuery &q);
  QIcon* getThumbIcon(const QString &domain);
  QPixmap getThumbPixmap(const QString &domain, int height, int width);

private:
  int m_fonts;
  void createConfigDirectories(QStringList dirs);
  QMap<QString, QIcon*> m_cacheIcons;
};
