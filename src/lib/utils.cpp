#include <QScreen>
#include <QMessageBox>
#include <QJsonDocument>
#include <QClipboard>

#include "utils.h"
#include "lib/config.h"

bool Utils::readJsonFile(QIODevice &device, QSettings::SettingsMap &map) {
  QJsonDocument json = QJsonDocument::fromJson(device.readAll());
  map = json.object().toVariantMap();
  return true;
}

QString Utils::barrayToString(const QByteArray &data) {
  return QString(QTextCodec::codecForMib(106)->toUnicode(data));
}

bool Utils::writeJsonFile(QIODevice &device, const QSettings::SettingsMap &map) {
  device.write(QJsonDocument(QJsonObject::fromVariantMap(map)).toJson(QJsonDocument::Indented));
  return true;
}

bool Utils::fileExists(const QString &path) {
  QFileInfo check_file(path);
  return check_file.exists() && check_file.isFile();
}

bool Utils::dirExists(const QString &path) {
  QDir pathDir(path);
  return pathDir.exists();
}

QByteArray Utils::fileOpen(const QString &path) {
  QFile file(path);
  if(!file.open(QFile::ReadOnly | QFile::Text)) {
    return QByteArray();
  }

  QByteArray data = file.readAll();
  file.close();
  return data;
}

QByteArray Utils::fileOpenQRC(const QString &path) {
  QFile file(path);
  if(!file.open(QIODevice::ReadOnly)) {
    qDebug() << "error: " << file.errorString();
  }

  QByteArray data = file.readAll();
  file.close();
  return data;
}

QString Utils::to_md5(const QString &inp) {
  return QString(QCryptographicHash::hash((inp.toUtf8()), QCryptographicHash::Md5).toHex());
}

bool Utils::isCyrillic(const QString &inp) {
  QRegularExpression re("[А-Яа-яЁё]+");
  QRegularExpressionMatch match = re.match(inp);
  return match.hasMatch();
}

void Utils::removeFiles(const QString &path) {
  QDir dir(path);
  dir.setNameFilters(QStringList() << "*.*");
  dir.setFilter(QDir::Files);
  for(const auto &dirFile: dir.entryList())
    dir.remove(dirFile);
}

QSqlQuery Utils::SqlExec(QSqlDatabase &db, const QString &sql) {
  QSqlQuery q(db);
  auto res = q.exec(sql);
  if(!res) {
    auto err = q.lastError().text();
    if (!err.contains("already exists"))
      qCritical() << "SQL error: " << err;
  }
  return q;
}

QSqlQuery Utils::SqlExec(QSqlQuery &q) {
  auto res = q.exec();
  if(!res) {
    auto err = q.lastError().text();
    if (!err.contains("already exists"))
      qCritical() << "SQL error: " << err;
  }
  return q;
}
