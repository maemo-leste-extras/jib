#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow * MainWindow::pMainWindow = nullptr;

MainWindow::MainWindow(AppContext *ctx, QString window_id, QString url, QWidget *parent) :
    QMainWindow(parent),
    m_ctx(ctx),
    m_window_id(window_id),
    m_timerFavicon(new QTimer),
    ui(new Ui::MainWindow)
{
  pMainWindow = this;
  ui->setupUi(this);
  ui->menuBar->hide();

#ifdef MAEMO
  this->ui->menuBar->hide();
#endif

  this->screenDpiRef = 128;
  this->screenGeo = QApplication::primaryScreen()->availableGeometry();
  this->screenRect = QGuiApplication::primaryScreen()->geometry();
  this->screenDpi = QGuiApplication::primaryScreen()->logicalDotsPerInch();
  this->screenDpiPhysical = QGuiApplication::primaryScreen()->physicalDotsPerInch();
  this->screenRatio = this->screenDpiPhysical / this->screenDpiRef;
  qDebug() << QString("%1x%2 (%3 DPI)").arg(
    this->screenRect.width()).arg(this->screenRect.height()).arg(this->screenDpi);

  this->showWebview();

  // menu
  connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::showSettingsview);
  connect(ui->actionNew_window, &QAction::triggered, this, &MainWindow::newWindowClicked);
  connect(ui->actionReload_page, &QAction::triggered, ui->widgetWeb, &WebWidget::onReloadClicked);
  connect(ui->actionToggle_navigation, &QAction::triggered, ui->widgetWeb, &WebWidget::onToggleNavigation);
  connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onQuitApplication);
  // @TODO: enable when fullscreen bug is fixed; void onFullscreen() {
  //     is_fullscreen ? this->showNormal() : this->showFullScreen();
  //    is_fullscreen = !is_fullscreen;
  // }

  // popular sites
  connect(m_ctx->popularSites, &PopularSites::dataChanged, this->ui->widgetWeb, &WebWidget::onPopularSitesChanged);
  m_ctx->popularSites->start();

  // web connects
  connect(ui->widgetWeb, &WebWidget::windowTitleChanged, this, &MainWindow::setWindowTitle);

  // about connects
  connect(ui->widgetAbout, &AboutWidget::backClicked, this, &MainWindow::showSettingsview);
  connect(ui->widgetAbout, &AboutWidget::urlClicked, ui->widgetWeb, &WebWidget::onVisitUrl);
  connect(ui->widgetAbout, &AboutWidget::urlClicked, this, &MainWindow::showWebview);

  // history connects
  connect(ui->widgetHistory, &HistoryWidget::urlClicked, ui->widgetWeb, &WebWidget::onVisitUrl);
  connect(ui->widgetHistory, &HistoryWidget::urlClicked, this, &MainWindow::showWebview);
  connect(ui->widgetHistory, &HistoryWidget::backClicked, this, &MainWindow::showSettingsview);

  // settings connects
  connect(ui->widgetSettings, &SettingsWidget::historyClicked, this, &MainWindow::showHistoryview);
  connect(ui->widgetSettings, &SettingsWidget::backClicked, this, &MainWindow::showWebview);
  connect(ui->widgetSettings, &SettingsWidget::aboutClicked, this, &MainWindow::showAboutview);
  connect(ui->widgetSettings, &SettingsWidget::zoomChanged, ui->widgetWeb, &WebWidget::onZoomFactorChanged);
  connect(ui->widgetSettings, &SettingsWidget::JSEnabledChanged, ui->widgetWeb, &WebWidget::onJSEnabledChanged);
  connect(ui->widgetSettings, &SettingsWidget::setUserAgent, ui->widgetWeb, &WebWidget::onSetUserAgent);
  connect(ui->widgetSettings, &SettingsWidget::backClicked, this, &MainWindow::showWebview);
  connect(ui->widgetSettings, &SettingsWidget::allowInsecureContentChanged, ui->widgetWeb, &WebWidget::onAllowInsecureContentChanged);
  connect(ui->widgetSettings, &SettingsWidget::allowPDFViewerChanged, ui->widgetWeb, &WebWidget::onAllowPDFViewerChanged);
  connect(ui->widgetSettings, &SettingsWidget::allowScrollbarChanged, ui->widgetWeb, &WebWidget::onAllowScrollbarChanged);
  connect(ui->widgetSettings, &SettingsWidget::allowWebGLChanged, ui->widgetWeb, &WebWidget::onAllowWebGLEnabled);

  connect(this, &MainWindow::windowCountChanged, ui->widgetWeb, &WebWidget::onWindowCountChanged);

  if(!url.isEmpty())
    ui->widgetWeb->onVisitUrl(url);
}

void MainWindow::onWindowCountChanged(int count) {
  emit windowCountChanged(count);
}

void MainWindow::showWebview() {
  this->resetView();
  ui->widgetWeb->show();
  ui->widgetWeb->setWindowTitle();
}

void MainWindow::showHistoryview() {
  this->resetView();
  ui->widgetHistory->show();
  setWindowTitle(QCoreApplication::applicationName() + "- History");
}

void MainWindow::showSettingsview() {
  this->resetView();
  ui->widgetSettings->show();
  setWindowTitle(QCoreApplication::applicationName() + "- Settings");
}

void MainWindow::showAboutview() {
  this->resetView();
  ui->widgetAbout->show();
}

void MainWindow::resetView() {
  ui->widgetWeb->hide();
  ui->widgetHistory->hide();
  ui->widgetSettings->hide();
  ui->widgetAbout->hide();
}

AppContext *MainWindow::getContext(){
    return pMainWindow->m_ctx;
}

MainWindow *MainWindow::getInstance() {
    return pMainWindow;
}

void MainWindow::onQuitApplication() {
  this->close();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event){
  emit windowClosed(m_window_id);
  event->accept();
}
