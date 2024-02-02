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


class PopularityItem : public QObject
{
Q_OBJECT
Q_PROPERTY(QString domain MEMBER domain)
Q_PROPERTY(int count MEMBER count)

public:
  explicit PopularityItem(const QString &domain, const QString &title, const QString &scheme, unsigned int count, QObject *parent = nullptr);

  QString domain;
  QString scheme;
  QString title;
  unsigned int count;
};

using PopularityList = qolm::QOlm<PopularityItem>;

class PopularSites : public QObject
{
Q_OBJECT
public:
  explicit PopularSites(QSqlDatabase *db, QObject *parent = nullptr);
  PopularityList *model;
  bool dirty = true;

  void start();
  void stop();

private slots:
  void onUpdate();

signals:
  void dataChanged();

private:
  QSqlDatabase *m_db;
  QTimer *m_timer;
};