#include <QObject>
#include <QDebug>

#include "historymodel.h"
#include "lib/utils.h"
#include "ctx.h"
#include "mainwindow.h"


HistoryItem::HistoryItem(QDateTime date, const QString &name, const QString &url, const QString &domain) :
  m_date(date),
  m_name(name),
  m_domain(domain),
  m_url(url) {}
QDateTime HistoryItem::date() const { return m_date; }
QString HistoryItem::name() const { return m_name; }
void HistoryItem::setName(const QString &name) { m_name = name; }
QString HistoryItem::url() const { return m_url; }
QString HistoryItem::domain() const { return m_domain; }
QIcon* HistoryItem::thumb() const {
  auto ctx = MainWindow::getContext();
  return ctx->getThumbIcon(m_domain);
}

HistoryModel::HistoryModel(AppContext *ctx, QObject *parent) :
    m_ctx(ctx),
    historySaveTimer(new QTimer(this)),
    QAbstractListModel(parent) {

  historySaveTimer->setInterval(5000);
  connect(historySaveTimer, &QTimer::timeout, this, &HistoryModel::saveHistory);
  historySaveTimer->start();

  auto _items = this->load_db();
  this->appendItems(_items);
}

void HistoryModel::SQLUpdateTitle(const QString &url, const QString &title) {
  if(url.isEmpty() || title.isEmpty() || title.startsWith("http")) return;

  auto sql = "UPDATE visits set TITLE = :title "
              "WHERE id=(SELECT id FROM visits WHERE url=:url ORDER BY id DESC limit 1);";

  QSqlQuery query(m_ctx->db);
  query.prepare(sql);
  query.bindValue(":title", title);
  query.bindValue(":url", url);
  Utils::SqlExec(query);

  if(!items.isEmpty()) {
    auto item = items.first();
    if(item->url() == url) {
      item->setName(title);

      this->dataChanged(this->index(0,0), this->index(0,2));
    }
  }
}

void HistoryModel::registerHistoryURL(const QString &url, const QString &title) {
  auto *item = new HistoryItem(QDateTime::currentDateTime(), title, url, QUrl(url).host());
  m_urlHistoryBuffer << item;
  this->appendItem(item, false);
}

void HistoryModel::saveHistory() {
  if(m_urlHistoryBuffer.empty()) return;

  QMap<time_t, QString>::iterator i;
  for(const HistoryItem *item: m_urlHistoryBuffer) {
    auto _url = item->url();
    auto qurl = QUrl(_url);
    auto _host = qurl.host();
    auto _scheme = qurl.scheme();

    QSqlQuery query(m_ctx->db);
    query.prepare("INSERT INTO visits (domain, url, title) VALUES(:domain, :url, :title)");
    query.bindValue(":title", item->name());
    query.bindValue(":domain", _host);
    query.bindValue(":url", _url);
    Utils::SqlExec(query);

    query = QSqlQuery(m_ctx->db);
    query.prepare("INSERT OR IGNORE INTO popularity(domain, title, scheme, count) VALUES(:domain, :title, :scheme, 0);");
    query.bindValue(":domain", _host);
    query.bindValue(":scheme", _scheme);
    query.bindValue(":title", _host);
    Utils::SqlExec(query);

    query = QSqlQuery(m_ctx->db);
    query.prepare("UPDATE popularity SET count = count + 1 WHERE domain = :domain");
    query.bindValue(":domain", _host);
    Utils::SqlExec(query);
  }

  m_urlHistoryBuffer.clear();
}

int HistoryModel::rowCount(const QModelIndex & parent) const {
  Q_UNUSED(parent);
  return items.count();
}

void HistoryModel::appendItem(HistoryItem *item, bool append) {
  beginInsertRows(QModelIndex(), 0, 0);
  append ? items.append(item) : items.prepend(item);
  endInsertRows();
}

void HistoryModel::appendItems(const QList<HistoryItem*> &_items) {
  beginInsertRows(QModelIndex(), 0, _items.length());
  for(const auto item: _items) {
    items.append(item);
  }
  endInsertRows();
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid() || index.row() < 0 || index.row() >= items.count())
    return {};

  const HistoryItem *acc = items[index.row()];
  if (!acc)
    return {};

  switch (role) {
    case Qt::DisplayRole:
      switch (index.column()) {
        case NameRole: {
          QString name = acc->name();
          return name.length() > 48 ? name.left(48) + "..." : name;
        }
        case DomainRole:
          return acc->domain();
        case DateRole:
          return acc->date().toString("MM-dd HH:mm");
        default:
          return {};
      }

    case Qt::DecorationRole:
      if (index.column() == HistoryRoles::NameRole && acc->thumb())
        return *acc->thumb();
    return {};

    // text color
    case Qt::ForegroundRole:
      return QColor(Qt::white);

    // bg
    case Qt::BackgroundRole:
      return QColor(0, 0, 0, 0);

    default:
      return {};
  }
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) return {};

  switch (section) {
    case NameRole:
      return QString("Title");
    case DomainRole:
      return QString("Domain");
    case DateRole:
      return QString("Date");
    default:
      return {};
  }
  return {};
}

int HistoryModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return HistoryRoles::COUNT;
}

QHash<int, QByteArray> HistoryModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[NameRole] = "name";
  roles[DomainRole] = "domain";
  roles[DateRole] = "date";
  return roles;
}

QList<HistoryItem*> HistoryModel::load_db() {
  return this->load_db("");
}

QList<HistoryItem*> HistoryModel::load_db(QString needle) {
  QList<HistoryItem*> rtn;
  auto hasNeedle = !needle.isEmpty() && needle.length() > 2;
  auto sql = QString("SELECT id,dt,url,domain,title FROM visits _ ORDER BY dt DESC LIMIT 150;");

  if(hasNeedle) {
    needle = needle.replace("%", "");
    needle = "%" + needle + "%";
    sql = sql.replace("_", "WHERE url LIKE :needle OR title LIKE :needle");
  } else {
    sql = sql.replace("_", "");
  }

  QSqlQuery query(m_ctx->db);
  query.prepare(sql);
  if(!needle.isEmpty())
    query.bindValue(":needle", needle);

  auto q = Utils::SqlExec(query);
  while (q.next()) {
    QString id = q.value(0).toString();
    QString dt = q.value(1).toString();
    QString url = q.value(2).toString();
    QString domain = q.value(3).toString();
    QString title = q.value(4).toString();

    auto _dt = QDateTime::fromString(dt, "yyyy-MM-dd HH:mm:ss");
    auto *_item = new HistoryItem(_dt, title, url, domain);
    this->appendItem(_item);
  }
  return rtn;
}

void HistoryModel::clear() {
  qDebug() << "Clearing HistoryModel";
  beginResetModel();
  qDeleteAll(items.begin(), items.end());
  items.clear();
  endResetModel();
}
