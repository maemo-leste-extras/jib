#pragma once

#include <QAbstractListModel>
#include <QAbstractItemModel>
#include <QStringList>
#include <QDateTime>
#include <QLineEdit>
#include <QTimer>

class AppContext;
class HistoryItem
{
public:
  HistoryItem(QDateTime date, const QString &name, const QString &url, const QString &domain);
  QString name() const;
  void setName(const QString &name);
  QString url() const;
  QString domain() const;
  QDateTime date() const;
  QIcon* thumb() const;

private:
  QString m_name;
  QString m_url;
  QString m_domain;
  QDateTime m_date;
};

class HistoryModel : public QAbstractListModel
{
  Q_OBJECT
public:
  enum HistoryRoles {
    NameRole=0,
    DomainRole,
    DateRole,
    COUNT
  };

  QList<HistoryItem*> items;

  explicit HistoryModel(AppContext *ctx, QObject *parent = nullptr);
  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void clear();
  void appendItem(HistoryItem *item, bool append = true);
  void appendItems(const QList<HistoryItem*> &_items);
  QList<HistoryItem*> load_db();
  QList<HistoryItem*> load_db(QString needle);
  void registerHistoryURL(const QString &url, const QString &title);
  void SQLUpdateTitle(const QString &url, const QString &title);
  void saveHistory();

protected:
  QHash<int, QByteArray> roleNames() const;

private:
  QString m_searchFilter;
  QTimer *historySaveTimer;
  QList<HistoryItem*> m_urlHistoryBuffer;
  AppContext *m_ctx;
};
