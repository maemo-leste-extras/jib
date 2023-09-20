// SPDX-License-Identifier: BSD-3-Clause

#include <QStandardItemModel>
#include <QTableWidget>
#include <QDesktopServices>

#include "mainwindow.h"
#include "webwidget.h"
#include "ui_webwidget.h"

WebWidget::WebWidget(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::WebWidget),
        m_contextMenu(new QMenu(this))
{
  ui->setupUi(this);

  m_ctx = MainWindow::getContext();
  auto mainWindow = MainWindow::getInstance();

  ui->webView->grabGesture(Qt::SwipeGesture);
  this->installEventFilter(this);

  ui->webView->show();
  ui->webView->page()->profile()->setSpellCheckEnabled(false);

  // security, user-settings
  ui->webView->settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
  ui->webView->settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
  auto allowInsecure = config()->get(ConfigKeys::allowInsecureContent).toBool();
  auto allowPDFViewer = config()->get(ConfigKeys::allowPdfViewer).toBool();
  auto allowWebGL = config()->get(ConfigKeys::allowWebGL).toBool();
  auto javascriptEnabled = config()->get(ConfigKeys::javascriptEnabled).toBool();
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, javascriptEnabled);
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::PdfViewerEnabled, allowPDFViewer);
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::WebGLEnabled, allowWebGL);
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::AllowRunningInsecureContent, allowInsecure);

  auto user_agent = config()->get(ConfigKeys::ua).toString();
  this->onSetUserAgent(user_agent);

  auto zoomFactor = config()->get(ConfigKeys::zoomFactor).toInt();
  ui->webView->setZoomFactor(zoomFactor / 100);

  QPixmap p_general_search("/usr/share/icons/hicolor/48x48/hildon/general_search.png");
  ui->iconSearch->setPixmap(p_general_search);

  ui->urlBar->setStyleSheet("QLineEdit { background-color: #8e8e8e; padding-left: 8px; border-radius: 8px; padding-bottom:2px; }");
  this->setStyleSheet("background-color: #575757;");

  connect(ui->btnFullscreen, &QPushButton::clicked, [=] {
    emit fullscreenClicked();
    this->setBottomBarHighlights();
  });

  connect(ui->btnBack, &QPushButton::clicked, [=] {
    ui->webView->triggerPageAction(QWebEnginePage::WebAction::Back);
  });

  connect(ui->btnForward, &QPushButton::clicked, [=] {
    ui->webView->triggerPageAction(QWebEnginePage::WebAction::Forward);
  });

  connect(ui->btnReload, &QPushButton::clicked, ui->webView, &QWebEngineView::reload);
  connect(ui->btnSettings, &QPushButton::clicked, [=] {
    emit settingsClicked();
  });

//  connect(ui->webView, &QWebEngineView::titleChanged, [=](const QString &title) {
//    qDebug() << "new title: " << title;
//  });

  connect(ui->webView, &QWebEngineView::urlChanged, [=](const QUrl &url){
    auto _url = url.toString();
    auto _host = url.host();

    if(!_url.startsWith("data:")) {
      auto title = ui->webView->title();
      m_ctx->historyModel->registerHistoryURL(_url, title);
      this->setURLBarText(_url);
      m_ctx->currentUrl = _url;

      auto zoomFactor = config()->get(ConfigKeys::zoomFactor).toInt();
      ui->webView->setZoomFactor(zoomFactor / 100);
    }
  });

  connect(ui->webView, &QWebEngineView::loadStarted, [=] {
    m_ctx->is_loading = true;
    setWindowTitle();
  });

  connect(ui->webView, &QWebEngineView::loadFinished, [=](bool status) {
    m_ctx->is_loading = false;
    setWindowTitle();
  });

  connect(ui->webView, &QWebEngineView::titleChanged, [=](const QString &title) {
    m_ctx->pageTitle = title;
    setWindowTitle();

    if(!title.startsWith("http://") && !title.startsWith("https://"))
      m_ctx->historyModel->SQLUpdateTitle(m_ctx->currentUrl, m_ctx->pageTitle);
  });

  connect(ui->urlBar, &QLineEdit::returnPressed, [=]{
    auto url = ui->urlBar->text().trimmed();
    if(!url.startsWith("http://") && !url.startsWith("https://") && url != "about:blank")
      url = "http://" + url;

    ui->webView->load(QUrl(url));
  });

  connect(ui->btnNav, &QPushButton::clicked, [=] {
    if(m_showing_navscreen) {
      ui->frameNav->hide();
    } else {
      ui->frameNav->show();
      ui->urlBar->setFocus();
      ui->urlBar->selectAll();
    }

    m_showing_navscreen = !m_showing_navscreen;
    this->setBottomBarHighlights();
  });

  connect(ui->webView, &QWebEngineView::iconChanged, this, &WebWidget::favIconChanged);

  this->setBottomBarHighlights();
  this->showSplash();
}

void WebWidget::favIconChanged(const QIcon &icon) {
  auto url = ui->webView->url();
  if(!m_ctx->currentUrl.startsWith("http")) return;

  auto host = QUrl(m_ctx->currentUrl).host();
  auto host_md5 = Utils::to_md5(host);
  auto out_path = m_ctx->iconCacheDirectory + "/" + host_md5 + ".png";

  if(!QFileInfo::exists(out_path)) {
    auto icon = ui->webView->icon();
    auto pixmap = icon.pixmap(32, 32);
    pixmap.save(out_path);
  }
}

void WebWidget::showSplash() {
  auto html = Utils::fileOpenQRC(":assets/splash.html");
  ui->webView->setHtml(html);

  this->setURLBarText("about:blank");

  ui->urlBar->selectAll();
}

void WebWidget::onVisitUrl(const QString &url) {
  ui->webView->load(QUrl(url));
}

void WebWidget::setURLBarText(const QString &url) {
  ui->urlBar->setText(url);
  ui->urlBar->setCursorPosition(0);
}

void WebWidget::setWindowTitle() {
  auto title = m_ctx->pageTitle;
  if(m_ctx->is_loading)
    title = "[@] " + m_ctx->pageTitle;

  emit windowTitleChanged(title);
}

void WebWidget::showContextMenu(const QPoint &pos) {
//    QModelIndex index = ui->webView->indexAt(pos);
//        if (!index.isValid()) {
//        return;
//    }
//    m_contextMenu->exec(ui->webView->viewport()->mapToGlobal(pos));
}

void WebWidget::setBottomBarHighlights() {
  auto activeColor = "color: lightblue;";
  auto inactiveColor = "color: transparent;";

  auto mainWindow = MainWindow::getInstance();
  ui->lineSettings->setStyleSheet(inactiveColor);
  ui->lineNav->setStyleSheet(m_showing_navscreen ? activeColor : inactiveColor);
  ui->lineFullscreen->setStyleSheet(mainWindow->is_fullscreen ? activeColor : inactiveColor);
  ui->lineBack->setStyleSheet(inactiveColor);
  ui->lineForward->setStyleSheet(inactiveColor);
  ui->lineReload->setStyleSheet(inactiveColor);
}

void WebWidget::onZoomFactorChanged(double amount) {
  ui->webView->setZoomFactor(amount);
}

void WebWidget::onJSEnabledChanged(bool status) {
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, status);
}

void WebWidget::onAllowScrollbarChanged(bool status) {
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::ShowScrollBars, status);
}

void WebWidget::onAllowPDFViewerChanged(bool status) {
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::PdfViewerEnabled, status);
}

void WebWidget::onAllowInsecureContentChanged(bool status) {
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::AllowRunningInsecureContent, status);
}

void WebWidget::onAllowWebGLEnabled(bool status) {
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::WebGLEnabled, status);
}

void WebWidget::onSetUserAgent(const QString &user_agent) {
  ui->webView->page()->profile()->setHttpUserAgent(user_agent);
}

WebWidget::~WebWidget() {
    delete ui;
}
