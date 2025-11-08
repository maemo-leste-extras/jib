#pragma once

#include <QObject>
#include <QSettings>
#include <QCryptographicHash>
#include <QStandardItemModel>
#include <QTextCharFormat>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class Utils
{

public:
    static bool readJsonFile(QIODevice &device, QSettings::SettingsMap &map);
    static bool writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map);
    static bool fileExists(const QString &path);
    static QByteArray fileOpen(const QString &path);
    static QByteArray fileOpenQRC(const QString &path);
    static bool isCyrillic(const QString &inp);
    static QString barrayToString(const QByteArray &data);
    static bool dirExists(const QString &path);
    static QString to_md5(const QString &inp);
    static void removeFiles(const QString &path);
    static QSqlQuery SqlExec(QSqlDatabase &db, const QString &sql);
    static QSqlQuery SqlExec(QSqlQuery &q);
};
