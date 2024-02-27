#include "windowmanager.h"

WindowManager::WindowManager(AppContext *ctx) : m_ctx(ctx) {
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

void WindowManager::spawn() {
  auto window_id = QString::number(QDateTime::currentSecsSinceEpoch());
  auto window = new MainWindow(m_ctx, window_id);
  connect(window, &MainWindow::newWindowClicked, this, &WindowManager::spawn);
  connect(window, &MainWindow::windowClosed, this, &WindowManager::onWindowClosed);

  windows[window_id] = window;
  windows[window_id]->show();

  emit windowCountChanged(windows.size());
}

WindowManager::~WindowManager() {

}
