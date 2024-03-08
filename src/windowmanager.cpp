#include "windowmanager.h"

WindowManager::WindowManager(AppContext *ctx, QWidget* parent) :
    QObject(parent),
    m_ctx(ctx) {
  connect(this, &WindowManager::windowCountChanged, [=](int count){
      for(auto window: windows) {
        window->onWindowCountChanged(count);
      }
  });
}

void WindowManager::onWindowClosed(QString window_id) {
  if(windows.contains(window_id))
    windows.remove(window_id);
  emit windowCountChanged(windows.size());
}

void WindowManager::onSpawn() {
  QString url = "";

  // visit argv[1]
  auto args = m_ctx->cmdargs->positionalArguments();
  if(!args.isEmpty() && windows.isEmpty() && args.at(0).startsWith("http"))
    url = args.at(0);

  auto window_id = QString::number(QDateTime::currentSecsSinceEpoch());
  auto window = new MainWindow(m_ctx, window_id, url);
  connect(window, &MainWindow::newWindowClicked, this, &WindowManager::onSpawn);
  connect(window, &MainWindow::settingsClicked, this, &WindowManager::onSpawnSettings);
  connect(window, &MainWindow::windowClosed, this, &WindowManager::onWindowClosed);

  windows[window_id] = window;
  windows[window_id]->show();

  emit windowCountChanged(windows.size());
}

void WindowManager::onSpawnSettings() {
  if(m_settingsWindow != nullptr) {
    m_settingsWindow->raise();
    return;
  }

  m_settingsWindow = new SettingsWindow(nullptr);
  connect(m_settingsWindow, &SettingsWindow::windowClosing, [=]{
      m_settingsWindow = nullptr;  // settingswindow does auto-delete
      qDebug() << "yay";
  });

  for(const auto &mainWindow: windows.values()) {
    connect(m_settingsWindow, &SettingsWindow::zoomChanged, mainWindow, &MainWindow::zoomChanged);
    connect(m_settingsWindow, &SettingsWindow::JSEnabledChanged, mainWindow, &MainWindow::JSEnabledChanged);
    connect(m_settingsWindow, &SettingsWindow::setUserAgent, mainWindow, &MainWindow::setUserAgent);
    connect(m_settingsWindow, &SettingsWindow::allowInsecureContentChanged, mainWindow, &MainWindow::allowInsecureContentChanged);
    connect(m_settingsWindow, &SettingsWindow::allowPDFViewerChanged, mainWindow, &MainWindow::allowPDFViewerChanged);
    connect(m_settingsWindow, &SettingsWindow::allowScrollbarChanged, mainWindow, &MainWindow::allowScrollbarChanged);
    connect(m_settingsWindow, &SettingsWindow::allowWebGLChanged, mainWindow, &MainWindow::allowWebGLChanged);
  }

  m_settingsWindow->show();
}

WindowManager::~WindowManager() {

}
