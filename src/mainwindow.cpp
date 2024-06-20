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
  this->setAttribute(Qt::WA_DeleteOnClose);

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
  connect(ui->actionNew_window, &QAction::triggered, this, &MainWindow::newWindowClicked);
  connect(ui->actionReload_page, &QAction::triggered, ui->widgetWeb, &WebWidget::onReloadClicked);
  connect(ui->actionToggle_navigation, &QAction::triggered, ui->widgetWeb, &WebWidget::onToggleNavigation);
  connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::settingsClicked);
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
  connect(ui->widgetAbout, &AboutWidget::urlClicked, ui->widgetWeb, &WebWidget::onVisitUrl);
  connect(ui->widgetAbout, &AboutWidget::urlClicked, this, &MainWindow::showWebview);

  // settings connects
  connect(this, &MainWindow::zoomChanged, ui->widgetWeb, &WebWidget::onZoomFactorChanged);
  connect(this, &MainWindow::JSEnabledChanged, ui->widgetWeb, &WebWidget::onJSEnabledChanged);
  connect(this, &MainWindow::setUserAgent, ui->widgetWeb, &WebWidget::onSetUserAgent);
  connect(this, &MainWindow::allowInsecureContentChanged, ui->widgetWeb, &WebWidget::onAllowInsecureContentChanged);
  connect(this, &MainWindow::allowPDFViewerChanged, ui->widgetWeb, &WebWidget::onAllowPDFViewerChanged);
  connect(this, &MainWindow::allowScrollbarChanged, ui->widgetWeb, &WebWidget::onAllowScrollbarChanged);
  connect(this, &MainWindow::allowWebGLChanged, ui->widgetWeb, &WebWidget::onAllowWebGLEnabled);

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

void MainWindow::showAboutview() {
  this->resetView();
  ui->widgetAbout->show();
}

void MainWindow::resetView() {
  ui->widgetWeb->hide();
  ui->widgetHistory->hide();
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
