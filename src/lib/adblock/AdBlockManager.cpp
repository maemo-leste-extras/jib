#include "AdBlockManager.h"
#include "AdBlockLog.h"
#include "AdBlockModel.h"
#include "AdBlockRequestHandler.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkRequest>
#include <QtGlobal>

#include <QDebug>
#include "ctx.h"
#include "lib/config.h"

namespace adblock
{

AdBlockManager::AdBlockManager(QObject *parent) :
    QObject(parent),
    m_filterContainer(),
    m_enabled(true),
    m_configFile(),
    m_subscriptionDir(),
    m_cosmeticJSTemplate(),
    m_subscriptions(),
    m_resourceAliasMap (),
    m_resourceMap(),
    m_resourceContentTypeMap(),
    m_domainStylesheetCache(24),
    m_jsInjectionCache(24),
    m_emptyStr(),
    m_adBlockModel(nullptr),
    m_log(nullptr),
    m_requestHandler(nullptr)
{
    setObjectName(QLatin1String("AdBlockManager"));

    m_enabled = true;

    loadDynamicTemplate();
    loadUBOResources();

    // Instantiate the logger
    m_log = new AdBlockLog(this);

    // Instantiate the network request handler
    m_requestHandler = new RequestHandler(m_filterContainer, m_log, this);
}

void AdBlockManager::setEnabled(bool value)
{
    if (m_enabled == value)
        return;

    m_enabled = value;

    // Clear filters regardless of state, and re-extract
    // filter data from subscriptions if being set to enabled
    clearFilters();
    if (value)
        extractFilters();
}

void AdBlockManager::installResource(const QUrl &url)
{
    // if (!url.isValid())
    //     return;

    // QNetworkRequest request;
    // request.setUrl(url);

    // InternalDownloadItem *item = m_downloadManager->downloadInternal(request, m_subscriptionDir + QDir::separator() + QString("resources"), false);
    // connect(item, &InternalDownloadItem::downloadFinished, this, &AdBlockManager::loadResourceFile);

    loadResourceFile("@TODO");
}

void AdBlockManager::installSubscription(const QUrl &url)
{
    if (!url.isValid())
        return;

    QNetworkRequest request;
    request.setUrl(url);

    // after download
    QString filePath = "ok";
    Subscription subscription(filePath);
    subscription.setSourceUrl(url);

    // Update ad block model
    int rowNum = static_cast<int>(m_subscriptions.size());
    const bool hasModel = m_adBlockModel != nullptr;
    if (hasModel)
        m_adBlockModel->beginInsertRows(QModelIndex(), rowNum, rowNum);

    m_subscriptions.push_back(std::move(subscription));

    if (hasModel)
        m_adBlockModel->endInsertRows();

    // Reload filters
    clearFilters();
    extractFilters();

}

void AdBlockManager::createUserSubscription()
{
    // Associate new subscription with file "custom.txt"
    QString userFile = m_subscriptionDir;
    userFile.append(QDir::separator());
    userFile.append(QLatin1String("custom.txt"));
    QString userFileUrl = QString("file://%1").arg(QFileInfo(userFile).absoluteFilePath());

    Subscription subscription(userFile);
    subscription.setSourceUrl(QUrl(userFileUrl));

    // Update AdBlockModel if it is instantiated
    // Update ad block model
    int rowNum = static_cast<int>(m_subscriptions.size());
    const bool hasModel = m_adBlockModel != nullptr;
    if (hasModel)
        m_adBlockModel->beginInsertRows(QModelIndex(), rowNum, rowNum);

    m_subscriptions.push_back(std::move(subscription));

    if (hasModel)
        m_adBlockModel->endInsertRows();

    // Don't bother reloading filters until some data is set within the filter, through the editor widget
}

void AdBlockManager::loadStarted(const QUrl &url)
{
    m_requestHandler->loadStarted(url);
}

AdBlockLog *AdBlockManager::getLog() const
{
    return m_log;
}

AdBlockModel *AdBlockManager::getModel()
{
    if (m_adBlockModel == nullptr)
        m_adBlockModel = new AdBlockModel(this);

    return m_adBlockModel;
}

const QString &AdBlockManager::getStylesheet(const URL &url) const
{
    // Check generic hide filters
    QString requestUrl = url.toString(URL::FullyEncoded).toLower();
    QString secondLevelDomain = url.getSecondLevelDomain();
    if (secondLevelDomain.isEmpty())
        secondLevelDomain = url.host();

    if (m_filterContainer.hasGenericHideFilter(requestUrl, secondLevelDomain))
        return m_emptyStr;

    return m_filterContainer.getCombinedFilterStylesheet();
}

const QString &AdBlockManager::getDomainStylesheet(const URL &url)
{
    if (!m_enabled)
        return m_emptyStr;

    const QString domain = url.host().toLower();

    // Check for a cache hit
    std::string domainStdStr = domain.toStdString();
    if (m_domainStylesheetCache.has(domainStdStr))
        return m_domainStylesheetCache.get(domainStdStr);

    const static QString styleScript = QStringLiteral("(function() {\n"
                                       "var doc = document;\n"
                                       "if (!doc.head) { \n"
                                       " document.onreadystatechange = function(){ \n"
                                       "  if (document.readyState == 'interactive') { \n"
                                       "   var sheet = document.createElement('style');\n"
                                       "   sheet.type = 'text/css';\n"
                                       "   sheet.innerHTML = '%1';\n"
                                       "   document.head.appendChild(sheet);\n"
                                       "  }\n"
                                       " }\n"
                                       " return;\n"
                                       "}\n"
                                       "var sheet = document.createElement('style');\n"
                                       "sheet.type = 'text/css';\n"
                                       "sheet.innerHTML = '%1';\n"
                                       "doc.head.appendChild(sheet);\n"
                                   "})();");
    const static QString styleScriptAlt = QStringLiteral("(function() {\n"
                                          "  const queries = [ %1 ];\n"
                                          "  document.onreadystatechange = function() {\n"
                                          "    if (document.readyState == 'interactive' || document.readyState == 'complete') { \n"
                                          "      queries.forEach((q) => { \n"
                                          "        try { const hits = document.querySelectorAll(q); \n"
                                          "        hits.forEach((el) => { el.style.display = 'none'; }); } catch (ex) { console.error(`Cosmetic filter script: could not query for ${q} - ${ex.message}`); }\n"
                                          "      });\n"
                                          "    }\n"
                                          "  }\n"
                                          "})();");

    QString stylesheet;
    QString stylesheetCustom;
    std::vector<Filter*> domainBasedHidingFilters = m_filterContainer.getDomainBasedHidingFilters(domain);
    for (Filter *filter : domainBasedHidingFilters)
    {
        QString filterArg = filter->getEvalString();
        stylesheet.append(QString("'%1',").arg(filterArg.replace(QString("'"), QString("\\'"))));
    }

    // Check for custom stylesheet rules
    domainBasedHidingFilters = m_filterContainer.getDomainBasedCustomHidingFilters(domain);
    for (Filter *filter : domainBasedHidingFilters)
    {
        stylesheetCustom.append(filter->getEvalString());
    }

    if (!stylesheet.isEmpty())
    {
        stylesheet = stylesheet.left(stylesheet.size() - 1);
        stylesheet = styleScriptAlt.arg(stylesheet);
    }
    if (!stylesheetCustom.isEmpty())
    {
        stylesheetCustom = stylesheetCustom.replace(QLatin1String("'"), QLatin1String("\\'"));
        stylesheetCustom = styleScript.arg(stylesheetCustom);

        stylesheet.append(QString("\n%1").arg(stylesheetCustom));
    }

    // Insert the stylesheet into cache
    m_domainStylesheetCache.put(domainStdStr, stylesheet);
    return m_domainStylesheetCache.get(domainStdStr);
}

const QString &AdBlockManager::getDomainJavaScript(const URL &url)
{
    if (!m_enabled)
        return m_emptyStr;

    const static QString cspScript = QStringLiteral("(function() {\n"
                                       "var doc = document;\n"
                                       "if (!doc.head) { \n"
                                       " document.onreadystatechange = function(){ \n"
                                       "  if (document.readyState == 'interactive') { \n"
                                       "   var meta = document.createElement('meta');\n"
                                       "   meta.setAttribute('http-equiv', 'Content-Security-Policy');\n"
                                       "   meta.setAttribute('content', \"%1\");\n"
                                       "   document.head.appendChild(meta);\n"
                                       "  }\n"
                                       " }\n"
                                       " return;\n"
                                       "}\n"
                                       "var meta = doc.createElement('meta');\n"
                                       "meta.setAttribute('http-equiv', 'Content-Security-Policy');\n"
                                       "meta.setAttribute('content', \"%1\");\n"
                                       "doc.head.appendChild(meta);\n"
                                   "})();");

    QString domain = url.host().toLower();
    QString requestUrl = url.toString(QUrl::FullyEncoded).toLower();

    if (domain.isEmpty())
    {
        domain = requestUrl.mid(requestUrl.indexOf(QStringLiteral("://")) + 3);
        if (domain.contains(QChar('/')))
            domain = domain.left(domain.indexOf(QChar('/')));
    }

    // Check for cache hit
    std::string requestHostStdStr = url.host().toLower().toStdString();
    if (requestHostStdStr.empty())
        requestHostStdStr = domain.toStdString();

    if (m_jsInjectionCache.has(requestHostStdStr))
        return m_jsInjectionCache.get(requestHostStdStr);

    QString scriptlets;
    QString proceduralFilters;
    std::vector<QString> cspDirectives;

    std::vector<Filter*> domainBasedScripts = m_filterContainer.getDomainBasedScriptInjectionFilters(domain);
    for (Filter *filter : domainBasedScripts)
        scriptlets.append(filter->getEvalString());

    std::vector<Filter*> cosmeticProceduralFilters = m_filterContainer.getDomainBasedCosmeticProceduralFilters(domain);
    for (Filter *filter : cosmeticProceduralFilters)
        proceduralFilters.append(filter->getEvalString());

    const Filter *inlineScriptBlockingRule = m_filterContainer.findInlineScriptBlockingFilter(requestUrl, domain);
    if (inlineScriptBlockingRule != nullptr)
        cspDirectives.push_back(QLatin1String("script-src 'unsafe-eval' * blob: data:"));

    std::vector<Filter*> cspFilters = m_filterContainer.getMatchingCSPFilters(requestUrl, domain);
    for (Filter *filter : cspFilters)
        cspDirectives.push_back(filter->getContentSecurityPolicy());

    QString result;
    if (!cspDirectives.empty())
    {
        QString cspConcatenated;
        for (size_t i = 0; i < cspDirectives.size(); ++i)
        {
            cspConcatenated.append(cspDirectives.at(i));
            if (i + 1 < cspDirectives.size())
                cspConcatenated.append(QLatin1String("; "));
        }
        cspConcatenated.replace(QLatin1String("\""), QLatin1String("\\\""));
        scriptlets.append(cspScript.arg(cspConcatenated));
    }

    if (!scriptlets.isEmpty() || !proceduralFilters.isEmpty())
    {
        result = m_cosmeticJSTemplate;
        result.replace(QStringLiteral("{{ADBLOCK_INTERNAL_SCRIPTLET}}"), scriptlets);
        result.replace(QStringLiteral("{{ADBLOCK_INTERNAL_COSMETIC}}"), proceduralFilters);
    }

    m_jsInjectionCache.put(requestHostStdStr, result);
    return m_jsInjectionCache.get(requestHostStdStr);
}

bool AdBlockManager::shouldBlockRequest(QWebEngineUrlRequestInfo &info, const QUrl &firstPartyUrl)
{
    if (!m_enabled)
        return false;

    return m_requestHandler->shouldBlockRequest(info, firstPartyUrl);
}

quint64 AdBlockManager::getRequestsBlockedCount() const
{
    return m_requestHandler->getTotalNumberOfBlockedRequests();
}

int AdBlockManager::getNumberAdsBlocked(const QUrl &url) const
{
    return m_requestHandler->getNumberAdsBlocked(url);
}

QString AdBlockManager::getResource(const QString &key) const
{
    QString keyNoSuffix = key;
    keyNoSuffix = keyNoSuffix.replace(QRegularExpression("(\\.[a-zA-Z]+)$"), QString());
    const bool hasKey = m_resourceMap.contains(key);

    QString resource;

    if (!hasKey && !m_resourceMap.contains(keyNoSuffix))
        resource = m_resourceMap.value(getResourceFromAlias(key));

    if (resource.isEmpty())
        resource = hasKey ? m_resourceMap.value(key) : m_resourceMap.value(keyNoSuffix);

    return resource;
}

QString AdBlockManager::getResourceFromAlias(const QString &alias) const
{
    return m_resourceAliasMap.value(alias);
}

QString AdBlockManager::getResourceContentType(const QString &key) const
{
    if (m_resourceContentTypeMap.contains(key))
        return m_resourceContentTypeMap.value(key);
    if (m_resourceAliasMap.contains(key))
        return m_resourceContentTypeMap.value(m_resourceAliasMap.value(key));
    return QString();
}

int AdBlockManager::getNumSubscriptions() const
{
    return static_cast<int>(m_subscriptions.size());
}

const Subscription *AdBlockManager::getSubscription(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_subscriptions.size()))
        return nullptr;

    return &m_subscriptions.at(static_cast<std::size_t>(index));
}

void AdBlockManager::toggleSubscriptionEnabled(int index)
{
    if (index < 0 || index >= static_cast<int>(m_subscriptions.size()))
        return;

    Subscription &sub = m_subscriptions.at(static_cast<std::size_t>(index));
    sub.setEnabled(!sub.isEnabled());

    // Reset filter data
    reloadSubscriptions();
}

void AdBlockManager::removeSubscription(int index)
{
    if (index < 0 || index >= static_cast<int>(m_subscriptions.size()))
        return;

    // Point iterator to subscription
    auto it = m_subscriptions.cbegin() + index;

    // Delete the subscription file before removal
    QFile subFile(it->getFilePath());
    if (subFile.exists())
    {
        if (!subFile.remove())
            qDebug() << "[Advertisement Blocker]: Could not remove subscription file " << subFile.fileName();
    }

    m_subscriptions.erase(it);

    reloadSubscriptions();
}

void AdBlockManager::reloadSubscriptions()
{
    m_domainStylesheetCache.clear();
    m_jsInjectionCache.clear();

    clearFilters();
    extractFilters();
}

void AdBlockManager::loadDynamicTemplate()
{
    QFile templateFile(QLatin1String(":assets/adblock/AdBlock.js"));
    if (!templateFile.open(QIODevice::ReadOnly)) {
        qWarning() << "welp";
        return;
    }

    m_cosmeticJSTemplate = templateFile.readAll();
    templateFile.close();
}

void AdBlockManager::loadUBOResources()
{
    QDir resourceDir(QString("%1%2%3").arg(m_subscriptionDir, QDir::separator(), QStringLiteral("resources")));
    if (!resourceDir.exists())
        resourceDir.mkpath(QStringLiteral("."));

    // Iterate through files in directory, loading into m_resourceMap
    QDirIterator resourceItr(resourceDir.absolutePath(), QDir::Files);
    while (resourceItr.hasNext())
    {
        loadResourceFile(resourceItr.next());
    }

    // Load built-in resources as well
    loadResourceFile(QStringLiteral(":assets/adblock/AdBlockResources.txt"));

    // Finally, load resource aliases
    loadResourceAliases();
}

void AdBlockManager::loadResourceAliases()
{
    QFile f(QStringLiteral(":assets/adblock/AdBlockAliases.txt"));
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return;

    QString line;
    QTextStream stream(&f);
    while (stream.readLineInto(&line))
    {
        line = line.trimmed();

        int delimIdx = line.indexOf(QChar('='));
        if (line.isEmpty() || delimIdx < 1)
            continue;

        const QString alias = line.left(delimIdx);
        const QString resourceName = line.mid(delimIdx + 1);
        m_resourceAliasMap.insert(alias, resourceName);
    }
}

void AdBlockManager::loadResourceFile(const QString &path)
{
    QFile f(path);
    if (!f.exists() || !f.open(QIODevice::ReadOnly))
        return;

    bool readingValue = false;
    QString currentKey, mimeType;
    QByteArray currentValue;
    QList<QByteArray> contents = f.readAll().split('\n');
    f.close();
    for (int i = 0; i < contents.size(); ++i)
    {
        const QByteArray &line = contents.at(i);
        if ((!readingValue && line.isEmpty()) || line.startsWith('#'))
            continue;

        // Extract key from line if not loading a value associated with a key
        if (!readingValue)
        {
            int sepIdx = line.indexOf(' ');
            if (sepIdx < 0)
                currentKey = line;
            else
            {
                currentKey = line.left(sepIdx);
                mimeType = line.mid(sepIdx + 1);
                m_resourceContentTypeMap.insert(currentKey, mimeType);
            }
            readingValue = true;
        }
        else
        {
            // Append currentValue with line if not empty.
            if (!line.isEmpty())
            {
                currentValue.append(line);
                if (mimeType.contains(QLatin1String("javascript")))
                    currentValue.append('\n');
            }
            else
            {
                // Insert key-value pair into map once an empty line is reached and search for next key
                m_resourceMap.insert(currentKey, QString(currentValue));
                currentValue.clear();
                readingValue = false;
            }
        }
    }
}

// void AdBlockManager::onSettingChanged(BrowserSetting setting, const QVariant &value)
// {
//     if (setting == BrowserSetting::AdBlockPlusEnabled)
//     {
//         setEnabled(value.toBool());
//     }
// }

void AdBlockManager::loadSubscriptions()
{
    if (!m_enabled)
        return;

    AppContext *parent = qobject_cast<AppContext*>(this->parent());
    auto adblockDirectory = parent->adblockDirectory;
    auto adblockFiltersEnabled = config()->get(ConfigKeys::adblockFiltersEnabled).toBool();
    auto adblockAbpEnabled = config()->get(ConfigKeys::adblockAbpEnabled).toBool();
    auto adblockPrivacyEnabled = config()->get(ConfigKeys::adblockPrivacyEnabled).toBool();
    auto adblockAbuseEnabled = config()->get(ConfigKeys::adblockAbuseEnabled).toBool();
    auto adblockUnbreakEnabled = config()->get(ConfigKeys::adblockUnbreakEnabled).toBool();

    QList<AdBlockRuleConfig> filters;
    filters << AdBlockRuleConfig("abp", adblockAbpEnabled);
    filters << AdBlockRuleConfig("filters", adblockFiltersEnabled);
    filters << AdBlockRuleConfig("privacy", adblockPrivacyEnabled);
    filters << AdBlockRuleConfig("resource-abuse", adblockAbuseEnabled);
    filters << AdBlockRuleConfig("unbreak", adblockUnbreakEnabled);
    

    for(AdBlockRuleConfig &filter: filters) {
        QString path_adblock_file_qrc = filter.filePathQRC();
        QString path_adblock_file = filter.filePath(adblockDirectory);

        auto data = Utils::fileOpenQRC(path_adblock_file_qrc);
        QFile file_adblock(path_adblock_file);
        if (!file_adblock.open(QIODevice::WriteOnly)) {
            qDebug() << "Error: Could not open file for writing: " << path_adblock_file;
            continue;
        }

        file_adblock.write(data);
        file_adblock.close();

        Subscription subscription(filter.name);
        subscription.setEnabled(filter.enabled);
        subscription.setName(filter.name);
        subscription.setFilePath(path_adblock_file);
        subscription.setLastUpdate(QDateTime::fromSecsSinceEpoch(0));
        subscription.setSourceUrl(QUrl("about:blank"));

        m_subscriptions.push_back(std::move(subscription));
    }

    extractFilters();
}

void AdBlockManager::clearFilters()
{
    m_filterContainer.clearFilters();
}

void AdBlockManager::extractFilters()
{
    for (Subscription &s : m_subscriptions) {
        // calling load() does nothing if subscription is disabled
        s.load(this);
    }

    m_filterContainer.extractFilters(m_subscriptions);
}

AdBlockManager::~AdBlockManager()
{

}

void AdBlockManager::onFilterChanged(const QString &name, bool state) {
    qDebug() << "adblock filter changed:" << name << "|" << state;
    for(Subscription &s: m_subscriptions) {
        if(s.getName() == name) {
            // if(s.getNumFilters() == 0 && state)
            //     s.load(this);
            s.setEnabled(state);
            reloadSubscriptions();
            return;
        }
    }

    qWarning() << "filter" << name << "not found";
}

}

