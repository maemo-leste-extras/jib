// SPDX-License-Identifier: BSD-3-Clause

#include <QStandardItemModel>
#include <QTableWidget>
#include <QDesktopServices>
#include <QSizePolicy>

#include "mainwindow.h"
#include "webwidget.h"
#include "launchitemwidget.h"
#include "ui_webwidget.h"

WebWidget::WebWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WebWidget),
    m_zoomTimer(new QTimer(this)),
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
  auto scrollbarEnabled = config()->get(ConfigKeys::allowScrollbar).toBool();
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::ShowScrollBars, scrollbarEnabled);
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::JavascriptEnabled, javascriptEnabled);
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::PdfViewerEnabled, allowPDFViewer);
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::WebGLEnabled, allowWebGL);
  ui->webView->settings()->setAttribute(QWebEngineSettings::WebAttribute::AllowRunningInsecureContent, allowInsecure);

  auto user_agent = config()->get(ConfigKeys::ua).toString();
  this->onSetUserAgent(user_agent);

  QPixmap p_general_search("/usr/share/icons/hicolor/48x48/hildon/general_search.png");
  ui->iconSearch->setPixmap(p_general_search);

  ui->urlBar->setStyleSheet("QLineEdit { background-color: #8e8e8e; padding-left: 8px; border-radius: 8px; padding-bottom:2px; }");
  this->setStyleSheet("background-color: #575757;");

  connect(ui->urlBar, &QLineEditFocus::focussed, this, [=](bool inFocus) {
    if(inFocus) {
      showSuggestions();
      QTimer::singleShot(0, [=]{
          ui->urlBar->selectAll();
      });
    }
  });

  connect(ui->btnFullscreen, &QPushButton::clicked, [=] {
    emit fullscreenClicked();
    this->setBottomBarHighlights();
  });

  connect(ui->urlBar, &QLineEdit::textChanged, this, [=](QString text) {
    if(!text.isEmpty() && text.length() > 2 && !text.startsWith("about:"))
      m_ctx->suggestionModel->search(text);
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

  connect(ui->btnWindows, &QPushButton::clicked, [=] {
    emit newWindowClicked();
  });

//  connect(ui->webView, &QWebEngineView::titleChanged, [=](const QString &title) {
//    qDebug() << "new title: " << title;
//  });

  connect(ui->webView, &QWebEngineView::urlChanged, [=](const QUrl &url) {
    auto _url = url.toString();
    if(_url.startsWith("data:") || _url.startsWith("about:"))
      return;

    auto title = ui->webView->title();
    this->setURLBarText(_url);
    m_ctx->currentUrl = _url;
    m_ctx->pageTitle = title;

    setZoomFactor();
  });

  connect(ui->webView, &QWebEngineView::loadStarted, [=] {
    m_ctx->is_loading = true;
    setWindowTitle();
  });

  connect(ui->webView, &QWebEngineView::loadFinished, [=](bool status) {
    m_ctx->is_loading = false;

    if(status && !m_ctx->currentUrl.isEmpty())
      m_ctx->historyModel->registerHistoryURL(m_ctx->currentUrl, m_ctx->pageTitle);

    this->setZoomFactor();
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
    if(!url.startsWith("http://") && !url.startsWith("https://") && url != "about:blank") {
      if(!url.contains(".")) {
        auto duck = QString("https://duckduckgo.com/?q=%1").arg(url);
        ui->webView->load(QUrl(duck));
        showWebview();
        return;
      }

      url = "http://" + url;
    }

    ui->webView->load(QUrl(url));
    showWebview();
  });

  connect(this, &WebWidget::visit, [=](QString url){
    ui->webView->load(QUrl(url));
    showWebview();
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

  ui->suggestionsTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui->suggestionsTable->verticalHeader()->setVisible(false);
  ui->suggestionsTable->horizontalHeader()->setVisible(false);
  ui->suggestionsTable->setShowGrid(false);
  ui->suggestionsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->suggestionsTable->setFont(QFont("Ubuntu", 22));
  ui->suggestionsTable->setModel(m_ctx->suggestionModel);
  ui->suggestionsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui->suggestionsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->suggestionsTable->setFocusPolicy(Qt::NoFocus);
  ui->suggestionsTable->setSelectionMode(QAbstractItemView::NoSelection);
  ui->suggestionsTable->setStyleSheet("QTableView::item { padding-bottom: 32px; }");

  connect(ui->suggestionsTable, &ClickTable::emptySpaceClicked, [=] {
    ui->urlBar->clearFocus();
    this->showWebview();
  });

  connect(ui->suggestionsTable, &QTableView::clicked, [=] {
    qDebug() << "clicked";

    QModelIndex index = ui->suggestionsTable->currentIndex();
    auto item = m_ctx->suggestionModel->items.at(index.row());
    emit urlClicked(item->url);
  });

//  connect(ui->suggestionsTable, &QTableView::clicked, [=] {
//    ui->frameSuggestions->hide();
//    ui->webView->show();
//    this->linkClicked();
//  });
  connect(this, &WebWidget::urlClicked, this, &WebWidget::onVisitUrl);

  showWebview();

  // button icons
  auto iconFont = m_ctx->iconFont();
  iconFont.setPointSize(16);
  ui->btnSettings->setFont(iconFont);
  ui->btnSettings->setText(QChar(0xE804));
  ui->btnNav->setFont(iconFont);
  ui->btnNav->setText(QChar(0xE80B));
  ui->btnFullscreen->setFont(iconFont);
  ui->btnFullscreen->setText(QChar(0xF108));
  ui->btnFullscreen->hide();
  ui->lineFullscreen->hide();
  //ui->lineWindows->hide();
  ui->btnReload->setFont(iconFont);
  ui->btnReload->setText(QChar(0xE80C));
  ui->btnBack->setFont(iconFont);
  ui->btnBack->setText(QChar(0xE80D));
  ui->btnForward->setFont(iconFont);
  ui->btnForward->setText(QChar(0xE80E));

  m_zoomTimer->setInterval(1000);
  auto zoomFactor = config()->get(ConfigKeys::zoomFactor).toDouble();
  ui->webView->setZoomFactor(zoomFactor / 100);
  connect(m_zoomTimer, &QTimer::timeout, this, [=] {
    auto _zoomFactor = ui->webView->zoomFactor();
    if(_zoomFactor * 100 != zoomFactor)
      ui->webView->setZoomFactor(zoomFactor / 100);
  });
  m_zoomTimer->start();

  onPopularSitesChanged();
}

void WebWidget::popularItemsClear() {
  while (QLayoutItem* item = ui->popularLayout->takeAt(0)) {
    if(QWidget *widget = item->widget())
      widget->deleteLater();
  }

  popItemFrames.clear();
}

void WebWidget::popularItemFill() {
  auto _width = 128;
  auto _height = 78;
  auto count = 0;

  for(const auto &item: *m_ctx->popularSites->model) {
    auto *f = new ClickFrame(this);
    f->setMaximumHeight(_height);
    f->setMaximumWidth(_width);
    f->setMinimumHeight(_height);
    f->setMaximumWidth(_width);
    f->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    auto *launchItem = new LaunchItemWidget(m_ctx, item->title, item->domain, this);
    launchItem->setMaximumHeight(_height);
    launchItem->setMaximumWidth(_width);
    launchItem->setMinimumHeight(_height);
    launchItem->setMinimumWidth(_width);
    launchItem->setContentsMargins(0, 0, 0, 0);
    launchItem->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    auto layout = new QGridLayout(this);
    layout->setContentsMargins(0, 4, 0, 0);
    layout->addWidget(launchItem);
    f->setLayout(layout);
    ui->popularLayout->insertWidget(count, f);
    count += 1;

    popItemFrames << f;
    connect(f, &ClickFrame::clicked, this, [=]() {
      auto _url = QString("%1://%2").arg(item->scheme, item->domain);
      ui->urlBar->setText(_url);
      ui->urlBar->clearFocus();
      emit visit(_url);
    });
  }

  ui->popularLayout->addStretch();
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
  showWebview();
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
  ui->lineWindows->setStyleSheet(inactiveColor);
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

void WebWidget::onPopularSitesChanged() {
  this->popularItemsClear();
  this->popularItemFill();
}

void WebWidget::framePopularShow() {
//  ui->framePopular->show();
//  ui->framePopular->setMaximumHeight(90);
}

void WebWidget::framePopularHide() {
//  ui->framePopular->hide();
//  ui->framePopular->setMaximumHeight(6);
}

void WebWidget::showSuggestions() {
  ui->webView->hide();
  ui->frameSuggestions->show();
  ui->framePopular->show();
  ui->framePopular->setMaximumHeight(90);
}

void WebWidget::showWebview() {
  ui->webView->show();
  ui->frameSuggestions->hide();
  ui->framePopular->hide();
}

void WebWidget::setZoomFactor() {
  auto zoomFactor = config()->get(ConfigKeys::zoomFactor).toDouble();
  ui->webView->setZoomFactor(zoomFactor / 100);
}

void WebWidget::onWindowCountChanged(int count) {
  ui->btnWindows->setText(QString::number(count));
}

WebWidget::~WebWidget() {
  delete ui;
}
