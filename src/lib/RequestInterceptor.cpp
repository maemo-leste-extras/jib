#include <QByteArray>
#include <QWebEngineUrlRequestInfo>
#include <QtWebEngineCoreVersion>
#include <QtGlobal>

#include "lib/RequestInterceptor.h"
#include "webwidget.h"

const static QByteArray cUserAgentHeader = QByteArray("User-Agent");

RequestInterceptor::RequestInterceptor(adblock::AdBlockManager *adblock, QObject *parent) :
  QWebEngineUrlRequestInterceptor(parent),
  m_adBlockManager(adblock),
  m_parentPage(qobject_cast<WebPage*>(parent)),
  m_sendDoNotTrack(false),
  m_sendCustomUserAgent(false),
  m_userAgent() {
    setObjectName(QStringLiteral("RequestInterceptor"));
    m_userAgent = "Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:127.0) Gecko/20100101 Firefox/127.0";
}

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info) {
  const QString requestScheme = info.requestUrl().scheme();
  if (requestScheme != QStringLiteral("about")
      && requestScheme != QStringLiteral("blocked"))
      // && info.requestMethod() == QString("GET"))
  {
    QUrl firstPartyUrl { info.firstPartyUrl() };

#if (QTWEBENGINECORE_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    if (m_parentPage) {
      if (!m_parentPage->url().isEmpty())
        firstPartyUrl = m_parentPage->url();
      else if (!m_parentPage->requestedUrl().isEmpty())
        firstPartyUrl = m_parentPage->requestedUrl();
    }
#endif

    if (m_adBlockManager && m_adBlockManager->shouldBlockRequest(info, firstPartyUrl))
      info.block(true);
  }

  // Check if we need to send the do not track header
  if (m_sendDoNotTrack)
    info.setHttpHeader("DNT", "1");

  if (m_sendCustomUserAgent)
    info.setHttpHeader(cUserAgentHeader, m_userAgent);
}
