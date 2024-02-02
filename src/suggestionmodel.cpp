#include "suggestionmodel.h"
#include "mainwindow.h"

SuggestionItem::SuggestionItem(const QString &domain, const QString &url, const QString &title, QDateTime date, QObject *parent) :
    url(url),
    domain(domain),
    date(date),
    title(title),
    QObject(parent) {}

QIcon * SuggestionItem::thumb() const {
  auto ctx = MainWindow::getContext();
  auto thumb = ctx->getThumbIcon(domain);
  return thumb;
}

SuggestionModel::SuggestionModel(QSqlDatabase *db, QObject *parent) :
    m_db(db),
    QAbstractListModel(parent) {
}

void SuggestionModel::search(QString url) {
  beginResetModel();
  items.clear();
  auto sql = QString("SELECT url, domain, title FROM visits WHERE url LIKE :url GROUP BY url ORDER BY id DESC LIMIT 5;");

  url = url.replace("%", "");
  url = "%" + url + "%";

  QSqlQuery query(*m_db);
  query.prepare(sql);
  query.bindValue(":url", url);

  auto q = Utils::SqlExec(query);
  int total = 0;
  while (q.next()) {
    auto url = q.value(0).toString();
    auto domain = q.value(1).toString();
    auto title = q.value(2).toString();
    QString dt = q.value(1).toString();
    auto _dt = QDateTime::fromString(dt, "yyyy-MM-dd HH:mm:ss");

    items.prepend(new SuggestionItem(domain, url, title, _dt, this));
    total += 1;
  }

  emit dataChanged();
  endResetModel();
}

int SuggestionModel::rowCount(const QModelIndex & parent) const {
  Q_UNUSED(parent);
  return items.count();
}

QVariant SuggestionModel::data(const QModelIndex &index, int role) const {
  if (index.row() < 0 || index.row() >= items.count())
    return {};

  const SuggestionItem *acc = items[index.row()];
  if(role == Qt::DisplayRole) {
    switch(index.column()) {
      case NameRole: {
        auto name = acc->url;

        if(name.length() > 70)
          return name.left(70) + "...";

        return name;
      }
      case Qt::ForegroundRole:
        return QVariant::fromValue(QColor(Qt::white));
      default:
        return {};
    }
  } else if (role == Qt::DecorationRole) {
    switch(index.column()) {
      case SuggestionRoles::NameRole:
        return *acc->thumb();
    }
  }
  return {};
}

QVariant SuggestionModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role != Qt::DisplayRole) return QVariant();

  switch (section) {
    case NameRole:
      return QString("Title");
    default:
      return QVariant();
  }
  return QVariant();
}

int SuggestionModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return 0;
  return SuggestionRoles::COUNT;
}

QHash<int, QByteArray> SuggestionModel::roleNames() const {
  QHash<int, QByteArray> roles;
  roles[NameRole] = "name";
  return roles;
}
