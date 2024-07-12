#pragma once

#include <memory>
#include <QByteArray>
#include <QWebEngineUrlRequestInterceptor>

#include "lib/adblock/AdBlockManager.h"

namespace adblock {
    class AdBlockManager;
}

class WebPage;

/**
 * @class RequestInterceptor 
 * @brief Intercepts network requests before they are made, checking if they need special handlnig.
 */
class RequestInterceptor : public QWebEngineUrlRequestInterceptor {
Q_OBJECT

public:
  /// Constructs the request interceptor with an optional parent pointer
  RequestInterceptor(adblock::AdBlockManager *adblock, QObject *parent = nullptr);

protected:
  /// Intercepts the given request, potentially blocking it, modifying the header values or redirecting the request
  void interceptRequest(QWebEngineUrlRequestInfo &info) override;

private:
  /// Attempts to fetch the settings and adblock manager services
  void fetchServices();

private:
  /// Advertisement blocking system manager
  adblock::AdBlockManager *m_adBlockManager;

  /// Parent web page. Only used with QtWebEngine version 5.13 or greater.
  WebPage *m_parentPage;

  /// Flag indicating whether or not to send the "Do not track" (DNT) header with requests
  bool m_sendDoNotTrack;

  /// Flag indicating whether or not to send a custom user agent in network requests
  bool m_sendCustomUserAgent;

  /// Custom user agent (Qt does not send the custom user agent in many types of requests. We have to override when enabled by the user)
  QByteArray m_userAgent;
};
