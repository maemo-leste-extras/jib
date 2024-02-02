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
#include <QStringListModel>
#include <QGestureEvent>
#include <QItemDelegate>
#include <QProgressBar>
#include <QHeaderView>
#include <QFileInfo>

#include "ctx.h"
#include "lib/QClickFrame.h"
#include "popularitymodel.h"

namespace Ui {
  class WebWidget;
}


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
  void fullscreenClicked();
  void settingsClicked();
  void windowTitleChanged(const QString &title);
  void visit(QString url);
  void urlClicked(const QString &url);

public slots:
  void onSetUserAgent(const QString &user_agent);
  void setBottomBarHighlights();
  void favIconChanged(const QIcon &icon);
  void onVisitUrl(const QString &url);
  void onJSEnabledChanged(bool status);
  void onAllowInsecureContentChanged(bool status);
  void onAllowPDFViewerChanged(bool status);
  void onAllowScrollbarChanged(bool status);
  void onAllowWebGLEnabled(bool status);
  void onZoomFactorChanged(double amount);
  void onPopularSitesChanged();

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
