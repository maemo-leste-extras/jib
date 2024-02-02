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


class BookmarkItem : public QObject
{
Q_OBJECT
Q_PROPERTY(QString url MEMBER url)
Q_PROPERTY(QString title MEMBER title)

public:
  explicit BookmarkItem(const QString &url, const QString &title, QObject *parent = nullptr);

  QString url;
  QString title;
};

class Bookmarks : public QObject
{
Q_OBJECT
public:
  explicit Bookmarks(QSqlDatabase *db, QObject *parent = nullptr);
  QList<BookmarkItem*> *model;

  void addItem(BookmarkItem *item) const;
  void delItem(BookmarkItem *item) const;

private slots:
  void onUpdate();

signals:
  void dataChanged();

private:
  QSqlDatabase *m_db;
};