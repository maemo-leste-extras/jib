#include <filesystem>

#include <QObject>
#include <QDir>
#include <QStandardPaths>

#include "ctx.h"
#include "lib/utils.h"

using namespace std::chrono;

AppContext::AppContext(QCommandLineParser *cmdargs) : cmdargs(cmdargs) {
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
  popularSites = new PopularSites(&db, this);
  suggestionModel = new SuggestionModel(&db, this);
  connect(historyModel, &QAbstractListModel::dataChanged, [=]{
    popularSites->dirty = true;
  });
  connect(historyModel, &QAbstractListModel::rowsInserted, [=]{
    popularSites->dirty = true;
  });

  // fonts
  m_fonts = QFontDatabase::addApplicationFont(":/assets/fonts.ttf");
}

QFont AppContext::iconFont() const {
  QString family = QFontDatabase::applicationFontFamilies(m_fonts).at(0);
  return {family};
}

void AppContext::initdb() {
  db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(dbFile);
  if(!db.open())
    throw std::runtime_error("could not open " + dbFile.toStdString());

  this->SqlCreateSchema();
}

QPixmap AppContext::getThumbPixmap(const QString &domain, int height, int width) {
  auto *icon = this->getThumbIcon(domain);
  return icon->pixmap(icon->actualSize(QSize(height, width)));
}

QIcon* AppContext::getThumbIcon(const QString &domain) {
  if(m_cacheIcons.contains(domain))
    return m_cacheIcons[domain];

  auto domain_md5 = Utils::to_md5(domain);
  auto path = this->iconCacheDirectory + "/" + domain_md5 + ".png";
  if(Utils::fileExists(path)) {
    auto info = QFileInfo(path);
    if(info.size() >= 128) {
      auto *icon = new QIcon(path);
      m_cacheIcons.insert(domain, icon);
      return icon;
    }
  }

  // no icon, use maemo default
  auto *icon = new QIcon(":/assets/maemo.png");
  m_cacheIcons.insert(domain, icon);
  return icon;
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
  auto table_popularity = "create table popularity "
    "(id integer primary key, "
    "domain varchar(128) UNIQUE,"
    "title varchar(256),"
    "scheme varchar(16),"
    "count integer)";
  auto table_bookmark = "create table bookmarks "
    "(id integer primary key, "
    "url varchar(128) UNIQUE,"
    "title varchar(256))";

  auto idx_url = "CREATE INDEX visits_url ON visits (url);";
  auto idx_popularity_domain = "CREATE INDEX domain ON popularity (domain);";
  auto idx_popularity_count = "CREATE INDEX count ON popularity (count);";

  for(const auto table_item: {table_icons, table_visits, table_popularity, table_bookmark, idx_url, idx_popularity_domain, idx_popularity_count}) {
    Utils::SqlExec(db, table_item);
  }
}

void AppContext::findDomainIcon(const QString &domain) {
  QSqlQuery query;
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
