#include <filesystem>

#include <QObject>
#include <QDir>
#include <QStandardPaths>

#include "ctx.h"
#include "lib/utils.h"

using namespace std::chrono;

AppContext::AppContext() {
  configRoot = QDir::homePath();
  isDebug = false;
  homeDir = QDir::homePath();
  configDirectory = QString("%1/.config/%2/").arg(configRoot, QCoreApplication::applicationName());
  iconCacheDirectory = configDirectory + "/cache/";
  dbFile = configDirectory + "/mbrowser.db";

  this->createConfigDirectory(configDirectory);
  this->createConfigDirectory(iconCacheDirectory);
  this->initdb();

  historyModel = new HistoryModel(this);
}

void AppContext::initdb() {
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbFile);
  if(!db.open())
    throw std::runtime_error("could not open " + dbFile.toStdString());

  this->SqlCreateSchema();
}

QIcon* AppContext::getThumbIcon(const QString &domain) {
  if(m_cacheIcons.contains(domain)) return m_cacheIcons[domain];

  auto domain_md5 = Utils::to_md5(domain);
  auto path = this->iconCacheDirectory + "/" + domain_md5 + ".png";
  if(Utils::fileExists(path)) {
    auto *icon = new QIcon(path);
    m_cacheIcons.insert(domain, icon);
    return icon;
  } else {
    return new QIcon("qrc:/assets/maemo.ico");
  }
}

QSqlQuery AppContext::SqlExec(QSqlQuery &q) {
  auto res = q.exec();
  if(!res) AppContext::SqlExecError(q);
  return q;
}

QSqlQuery AppContext::SqlExec(const QString &sql) {
  QSqlQuery q(db);
  auto res = q.exec(sql);
  if(!res) AppContext::SqlExecError(q);
  return q;
}

void AppContext::SqlExecError(const QSqlQuery &q) {
  auto err = q.lastError().text();
  if (!err.contains("already exists"))
    qCritical() << "SQL error: " << err;
}

void AppContext::SqlCreateSchema() {
  QSqlQuery query(db);

  auto table_icons = "create table icons "
    "(id integer primary key, "
    "domain varchar(128), "
    "icon BLOB)";
  auto table_visits = "create table visits "
    "(id integer primary key, "
    "dt datetime default current_timestamp, "
    "url varchar(256), "
    "domain varchar(64), "
    "title varchar(256))";
  auto idx_url = "CREATE INDEX visits_url ON visits (url);";

  for(const auto table_item: {table_icons, table_visits, idx_url}) {
    auto res = query.exec(table_item);
    AppContext::SqlExec(table_item);
  }
}

void AppContext::findDomainIcon(const QString &domain) {
  QSqlQuery query;
  query.exec("SELECT name, salary FROM employee WHERE salary > 50000");
}

void AppContext::createConfigDirectory(const QString &dir) {
  QStringList createDirs({dir});
  for(const auto &d: createDirs) {
    if(!std::filesystem::exists(d.toStdString())) {
      qDebug() << QString("Creating directory: %1").arg(d);
      if (!QDir().mkpath(d))
        throw std::runtime_error("Could not create directory " + d.toStdString());
    }
  }
}

AppContext::~AppContext() {}
