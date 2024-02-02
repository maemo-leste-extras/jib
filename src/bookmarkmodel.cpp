#include "bookmarkmodel.h"

BookmarkItem::BookmarkItem(const QString &url, const QString &title, QObject *parent) :
    url(url),
    title(title),
    QObject(parent) {}

Bookmarks::Bookmarks(QSqlDatabase *db, QObject *parent) :
    m_db(db),
    model(new QList<BookmarkItem*>),
    QObject(parent) {
}

void Bookmarks::addItem(BookmarkItem *item) const {
  QSqlQuery query(*m_db);
  query.prepare("INSERT INTO bookmarks (url, title) VALUES(:url, :title)");
  query.bindValue(":title", item->title);
  query.bindValue(":url", item->url);
  Utils::SqlExec(query);
  model->append(item);
}

void Bookmarks::delItem(BookmarkItem *item) const {
  QSqlQuery query(*m_db);
  query.prepare("DELETE FROM bookmarks WHERE URL = :url");
  query.bindValue(":url", item->url);
  Utils::SqlExec(query);
  model->removeOne(item);
  item->deleteLater();
}

void Bookmarks::onUpdate() {
//  model->clear();
//  auto sql = QString("SELECT domain, title, scheme, count FROM popularity ORDER BY count DESC LIMIT 5;");
//
//  QSqlQuery query(*m_db);
//  query.prepare(sql);
//
//  auto q = Utils::SqlExec(query);
//  int total = 0;
//  while (q.next()) {
//    auto domain = q.value(0).toString();
//    auto title = q.value(1).toString();
//    auto scheme = q.value(2).toString();
//    auto count = q.value(3).toInt();
//    model->append(new BookmarkItem(domain, title, scheme, count, this));
//    total += 1;
//  }
//
//  if(total > 0)
//    emit dataChanged();
}
