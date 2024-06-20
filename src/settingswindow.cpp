// SPDX-License-Identifier: BSD-3-Clause
#include <QStandardItemModel>
#include <QTableWidget>
#include <QDesktopServices>

#include "mainwindow.h"
#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
  ui->setupUi(this);
  this->setAttribute(Qt::WA_DeleteOnClose);

  setProperty("X-Maemo-Orientation", 2);

  m_ctx = MainWindow::getContext();

  // prepare UI from user config
  auto javascriptEnabled = config()->get(ConfigKeys::javascriptEnabled).toBool();
  javascriptEnabled ? ui->radio_js_enabled->setChecked(true) : ui->radio_js_disabled->setChecked(true);

  int zoomFactor = config()->get(ConfigKeys::zoomFactor).toInt();
  switch(zoomFactor) {
    case 75: {
      ui->radio_zoom_75->setChecked(true);
      break;
    }
    case 100: {
      ui->radio_zoom_100->setChecked(true);
      break;
    }
    case 125: {
      ui->radio_zoom_125->setChecked(true);
      break;
    }
    case 150: {
      ui->radio_zoom_150->setChecked(true);
      break;
    }
    case 200: {
      ui->radio_zoom_200->setChecked(true);
      break;
    }
    default: {
      ui->radio_zoom_100->setChecked(true);
      break;
    }
  }

  auto ua = config()->get(ConfigKeys::ua).toString();
  auto ua_mob = ua.contains("Android");
  ua_mob ? ui->radio_ua_mobile->setChecked(true) : ui->radio_ua_desktop->setChecked(true);

  auto allowInsecureContent = config()->get(ConfigKeys::allowInsecureContent).toBool();
  allowInsecureContent ? ui->radio_insecurecontent_enabled->setChecked(true) : ui->radio_insecurecontent_disabled->setChecked(true);

  auto allowPdfViewer = config()->get(ConfigKeys::allowPdfViewer).toBool();
  allowPdfViewer ? ui->radio_pdfviewer_enabled->setChecked(true) : ui->radio_pdfviewer_disabled->setChecked(true);

  auto allowWebGL = config()->get(ConfigKeys::allowWebGL).toBool();
  allowWebGL ? ui->radio_webgl_enabled->setChecked(true) : ui->radio_webgl_disabled->setChecked(true);

  auto allowScrollbar = config()->get(ConfigKeys::allowScrollbar).toBool();
  allowScrollbar ? ui->radio_scrollbar_enabled->setChecked(true) : ui->radio_scrollbar_disabled->setChecked(true);

  connect(ui->buttonGroup, &QButtonGroup::idClicked, this, &SettingsWindow::onUAChanged);
  connect(ui->buttonGroup_2, &QButtonGroup::idClicked, this, &SettingsWindow::onZoomChanged);
  connect(ui->buttonGroup_3, &QButtonGroup::idClicked, this, &SettingsWindow::onJSChanged);
  connect(ui->buttonGroup_4, &QButtonGroup::idClicked, this, &SettingsWindow::onAllowInsecureContentChanged);
  connect(ui->buttonGroup_5, &QButtonGroup::idClicked, this, &SettingsWindow::onAllowPDFViewerChanged);
  connect(ui->buttonGroup_6, &QButtonGroup::idClicked, this, &SettingsWindow::onAllowScrollbarChanged);
  connect(ui->buttonGroup_7, &QButtonGroup::idClicked, this, &SettingsWindow::onAllowWebGLChanged);
}

void SettingsWindow::onJSChanged(int idx) {
  idx == -2 ? emit JSEnabledChanged(true) : emit JSEnabledChanged(false);
}

void SettingsWindow::onZoomChanged(int idx) {
  switch(idx) {
    case -2: {
      emit zoomChanged(0.75);
      config()->set(ConfigKeys::zoomFactor, 75);
      return;
    }
    case -3: {
      emit zoomChanged(1.0);
      config()->set(ConfigKeys::zoomFactor, 100);
      return;
    }
    case -4: {
      emit zoomChanged(1.25);
      config()->set(ConfigKeys::zoomFactor, 125);
    }
    case -5: {
      emit zoomChanged(1.50);
      config()->set(ConfigKeys::zoomFactor, 150);
      return;
    }
    case -6: {
      emit zoomChanged(2.0);
      config()->set(ConfigKeys::zoomFactor, 200);
      return;
    }
    default: {
      emit zoomChanged(1.0);
      config()->set(ConfigKeys::zoomFactor, 100);
      return;
    }
  }
}

void SettingsWindow::onUAChanged(int idx) {
  if(idx == -2) {
    emit setUserAgent(m_ctx->uaDesktop);
    config()->set(ConfigKeys::ua, m_ctx->uaDesktop);
  }
  else {
    emit setUserAgent(m_ctx->uaMobile);
    config()->set(ConfigKeys::ua, m_ctx->uaMobile);
  }
}

void SettingsWindow::onAllowScrollbarChanged(int idx) {
  if(idx == -2) {
    emit allowScrollbarChanged(true);
    config()->set(ConfigKeys::allowScrollbar, true);
  }
  else {
    emit allowScrollbarChanged(false);
    config()->set(ConfigKeys::allowScrollbar, false);
  }
}

void SettingsWindow::onAllowPDFViewerChanged(int idx) {
  if(idx == -2) {
    emit allowPDFViewerChanged(true);
    config()->set(ConfigKeys::allowPdfViewer, true);
  }
  else {
    emit allowPDFViewerChanged(false);
    config()->set(ConfigKeys::allowPdfViewer, false);
  }
}

void SettingsWindow::onAllowWebGLChanged(int idx) {
  if(idx == -2) {
    emit allowWebGLChanged(true);
    config()->set(ConfigKeys::allowWebGL, false);
  }
  else {
    emit allowWebGLChanged(false);
    config()->set(ConfigKeys::allowWebGL, false);
  }
}

void SettingsWindow::onAllowInsecureContentChanged(int idx) {
  if(idx == -2) {
    emit allowInsecureContentChanged(true);
    config()->set(ConfigKeys::allowInsecureContent, true);
  }
  else {
    emit allowInsecureContentChanged(false);
    config()->set(ConfigKeys::allowInsecureContent, false);
  }
}

void SettingsWindow::closeEvent(QCloseEvent* event) {
    emit windowClosing();
    event->accept();
}

SettingsWindow::~SettingsWindow() {
  delete ui;
}
