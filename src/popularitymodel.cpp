#include "popularitymodel.h"

PopularityItem::PopularityItem(const QString &domain, const QString &title, const QString &scheme, unsigned int count, QObject *parent) :
    domain(domain),
    title(title),
    scheme(scheme),
    count(count),
    QObject(parent) {}

PopularSites::PopularSites(QSqlDatabase *db, QObject *parent) :
    m_db(db),
    m_timer(new QTimer(this)),
    model(new PopularityList(this)),
    QObject(parent) {
  connect(m_timer, &QTimer::timeout, this, &PopularSites::onUpdate);
  m_timer->setInterval(5000);
}

void PopularSites::start() {
  m_timer->start();
  this->onUpdate();
}

void PopularSites::stop() {
  m_timer->stop();
}

void PopularSites::onUpdate() {
  if(!dirty) return;

  model->clear();
  auto sql = QString("SELECT domain, title, scheme, count FROM popularity ORDER BY count DESC LIMIT 5;");

  QSqlQuery query(*m_db);
  query.prepare(sql);

  auto q = Utils::SqlExec(query);
  int total = 0;
  while (q.next()) {
    auto domain = q.value(0).toString();
    auto title = q.value(1).toString();
    auto scheme = q.value(2).toString();
    auto count = q.value(3).toInt();
    model->append(new PopularityItem(domain, title, scheme, count, this));
    total += 1;
  }

  if(total > 0)
    emit dataChanged();

  dirty = false;
}
