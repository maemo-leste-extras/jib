// SPDX-License-Identifier: BSD-3-Clause
// Copyright (C) 2020 KeePassXC Team <team@keepassxc.org>
// Copyright (C) 2011 Felix Geyer <debfx@fobos.de>

#include "lib/config.h"
#include "lib/utils.h"

#define QS QStringLiteral

struct ConfigDirective
{
    QString name;
    QVariant defaultValue;
};

static const QHash<ConfigKeys::ConfigKey, ConfigDirective> configStrings = {
  {ConfigKeys::Test,{QS("Test"), "test"}},
  {ConfigKeys::zoomFactor,{QS("zoomFactor"), 100}},
  {ConfigKeys::allowPdfViewer,{QS("allowPdfViewer"), true}},
  {ConfigKeys::allowScrollbar,{QS("allowScrollbar"), true}},
  {ConfigKeys::allowWebGL,{QS("allowWebGL"), true}},
  {ConfigKeys::allowInsecureContent,{QS("allowInsecureContent"), true}},
  {ConfigKeys::ua,{QS("ua"), "Mozilla/5.0 (Linux; Android 10; SM-A205U) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/117.0.5938.60 Mobile Safari/537.36"}},
  {ConfigKeys::javascriptEnabled,{QS("javascriptEnabled"), true}},

  {ConfigKeys::adblockFiltersEnabled,{QS("adblockFiltersEnabled"), true}},
  {ConfigKeys::adblockAbpEnabled,{QS("adblockAbpEnabled"), false}},
  {ConfigKeys::adblockPrivacyEnabled,{QS("adblockPrivacyEnabled"), false}},
  {ConfigKeys::adblockAbuseEnabled,{QS("adblockAbuseEnabled"), false}},
  {ConfigKeys::adblockUnbreakEnabled,{QS("adblockUnbreakEnabled"), false}}
};

QPointer<Config> Config::m_instance(nullptr);

Q_INVOKABLE QVariant Config::get(unsigned int key) {
  return this->get(ConfigKeys::ConfigKey(key));
}

QVariant Config::get(ConfigKeys::ConfigKey key)
{
    auto cfg = configStrings[key];
    auto defaultValue = configStrings[key].defaultValue;

    return m_settings->value(cfg.name, defaultValue);
}

QString Config::getFileName()
{
    return m_settings->fileName();
}

void Config::set(ConfigKeys::ConfigKey key, const QVariant& value)
{
    if (get(key) == value) {
        return;
    }

    auto cfg = configStrings[key];
    m_settings->setValue(cfg.name, value);

    this->sync();
    emit changed(key);
}

/**
 * Sync configuration with persistent storage.
 *
 * Usually, you don't need to call this method manually, but if you are writing
 * configurations after an emitted \link QCoreApplication::aboutToQuit() signal,
 * use it to guarantee your config values are persisted.
 */
void Config::sync()
{
    m_settings->sync();
}

void Config::resetToDefaults()
{
    m_settings->clear();
}

Config::Config(const QString& fileName, QObject* parent)
        : QObject(parent) {
    init(fileName);
}

Config::Config(QObject* parent)
        : QObject(parent) {
    QString configPath;

    configPath = QDir::homePath();
    configPath += QString("/.config/%1/settings.json").arg(QCoreApplication::applicationName());
    qDebug() << configPath;
    init(QDir::toNativeSeparators(configPath));
}


Config::~Config()
{
}

void Config::init(const QString& configFileName) {
    const QSettings::Format jsonFormat = QSettings::registerFormat("json", Utils::readJsonFile, Utils::writeJsonFile);
    QSettings::setDefaultFormat(jsonFormat);
    m_settings.reset(new QSettings(configFileName, jsonFormat));

    connect(qApp, &QCoreApplication::aboutToQuit, this, &Config::sync);
}

Config* Config::instance() {
    if (!m_instance) {
        m_instance = new Config(qApp);
    }

    return m_instance;
}
