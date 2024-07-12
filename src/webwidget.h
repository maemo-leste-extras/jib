// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#include <QMenu>
#include <QTimer>
#include <QDebug>
#include <QObject>
#include <QWidget>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#include <QStringListModel>
#include <QGestureEvent>
#include <QItemDelegate>
#include <QProgressBar>
#include <QHeaderView>
#include <QFileInfo>

#include "ctx.h"
#include "lib/adblock/AdBlockManager.h"
#include "lib/QClickFrame.h"
#include "popularitymodel.h"

namespace Ui {
  class WebWidget;
}

// namespace adblock {
//   class AdBlockManager;
// }

class WebPage : public QWebEnginePage {
  Q_OBJECT
public:
  WebPage(adblock::AdBlockManager *adblock, QWebEngineProfile *profile, QObject *parent = nullptr);
  ~WebPage() = default;
protected:
  /// Called upon receiving a request to navigate to the specified url by means of the given navigation type. If the method returns true, the request is accepted
  bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame) override;
private slots:
  /// Called when a frame is finished loading
  void onLoadFinished(bool ok);
private:
  /// Advertisement blocking system manager
  adblock::AdBlockManager *m_adBlockManager;

  /// Contains the original URL of the current page, as passed to the WebPage in the acceptNavigationRequest method
  QUrl m_originalUrl;

  /// Scripts injected by ad block during load progress and load finish
  QString m_mainFrameAdBlockScript;

  /// Flag indicating whether or not we need to inject the adblock script into the DOM
  bool m_injectedAdblock;

  /// Stores feature permissions that were allowed by the user, in the current session, local to this page.
  /// TODO: Later we can extend this to persistent storage, across web pages, through a permission manager,
  ///       but only if the user "opts-in" to it.
  QHash<QUrl, std::vector<WebPage::Feature>> m_permissionsAllowed;

  /// Stores feature permissions that were denied by the user, in the current session, local to this page.
  QHash<QUrl, std::vector<WebPage::Feature>> m_permissionsDenied;
};


class WebWidget : public QWidget
{
Q_OBJECT

public:
  explicit WebWidget(QWidget *parent = nullptr);
  void setupUIModels();
  void queueTask();
  void setupCompleter();
  void setURLBarText(const QString &url);
  void registerURLVisited(const QString &url);
  void setWindowTitle();
  void showSplash();
  void popularItemFill();
  void popularItemsClear();
  ~WebWidget();

signals:
  void windowTitleChanged(const QString &title);
  void visit(QString url);
  void urlClicked(const QString &url);

public slots:
  void onSetUserAgent(const QString &user_agent);
  void favIconChanged(const QIcon &icon);
  void onVisitUrl(const QString &url);
  void onJSEnabledChanged(bool status);
  void onAllowInsecureContentChanged(bool status);
  void onAllowPDFViewerChanged(bool status);
  void onAllowScrollbarChanged(bool status);
  void onAllowWebGLEnabled(bool status);
  void onZoomFactorChanged(double amount);
  void onPopularSitesChanged();
  void onWindowCountChanged(int count);
  void onReloadClicked();
  void onToggleNavigation();

private:
  void showContextMenu(const QPoint &pos);
  void framePopularShow();
  void framePopularHide();
  void showSuggestions();
  void showWebview();
  void setZoomFactor();

  AppContext *m_ctx;
  Ui::WebWidget *ui;
  QMenu *m_contextMenu;
  QTimer *m_zoomTimer;
  QList<ClickFrame*> popItemFrames;
  bool m_showing_navscreen = true;
  WebPage *m_page;

  void handleLeftSwipe() {
    qDebug() << "handleLeftSwipe()";
  }

  void handleRightSwipe() {
    qDebug() << "handleRightSwipe()";
  }

  void handleUpSwipe() {
    qDebug() << "handleUpSwipe()";
  }

  void handleDownSwipe() {
    qDebug() << "handleDownSwipe()";
  }

protected:
  bool eventFilter(QObject *obj, QEvent *event) override {
    if (event->type() == QEvent::Gesture) {
      auto *gestureEvent = static_cast<QGestureEvent *>(event);
      if (QGesture *swipe = gestureEvent->gesture(Qt::SwipeGesture)) {
        if (swipe->state() == Qt::GestureFinished) {
          auto *swipeGesture = static_cast<QSwipeGesture *>(swipe);
          if (swipeGesture->horizontalDirection() == QSwipeGesture::Left) {
            // Handle left swipe gesture
            handleLeftSwipe();
          }
          else if (swipeGesture->horizontalDirection() == QSwipeGesture::Right) {
            // Handle right swipe gesture
            handleRightSwipe();
          }
          else if (swipeGesture->verticalDirection() == QSwipeGesture::Up) {
            // Handle up swipe gesture
            handleUpSwipe();
          }
          else if (swipeGesture->verticalDirection() == QSwipeGesture::Down) {
            // Handle down swipe gesture
            handleDownSwipe();
          }
        }
      }
    }
    return QWidget::eventFilter(obj, event);
  }
};
