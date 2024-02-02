#pragma once

#include <QObject>
#include <QCommandLineParser>
#include <QSortFilterProxyModel>
#include <QNetworkAccessManager>
#include <QStorageInfo>
#include <QApplication>
#include <QModelIndex>
#include <QFileInfo>
#include <algorithm>
#include <QVariant>
#include <iostream>
#include <QProcess>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <iostream>
#include <fstream>
#include <QTimer>
#include <random>
#include <chrono>
#include <QBrush>
#include <string>
#include <array>

#include "lib/utils.h"
#include <QOlm/QOlm.hpp>


class SuggestionItem : public QObject
{
Q_OBJECT

public:
  explicit SuggestionItem(const QString &domain, const QString &url, const QString &title, QDateTime date, QObject *parent = nullptr);
  QIcon * thumb() const;

  QString url;
  QString title;
  QString domain;
  QDateTime date;
};

class SuggestionModel : public QAbstractListModel
{
Q_OBJECT
public:
  enum SuggestionRoles {
    NameRole=0,
    COUNT
  };

  explicit SuggestionModel(QSqlDatabase *db, QObject *parent = nullptr);
  [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
  [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
  [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
  [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void search(QString url);
  QList<SuggestionItem*> items;

protected:
  QHash<int, QByteArray> roleNames() const;

signals:
  void dataChanged();

private:
  QSqlDatabase *m_db;
};