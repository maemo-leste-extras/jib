// SPDX-License-Identifier: BSD-3-Clause

#include <QStandardItemModel>
#include <QTableWidget>
#include <QDesktopServices>

#include "mainwindow.h"
#include "aboutwidget.h"
#include "ui_aboutwidget.h"

AboutWidget::AboutWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutWidget)
{
  ui->setupUi(this);

  m_ctx = MainWindow::getContext();
  auto mainWindow = MainWindow::getInstance();

  connect(ui->btnBack, &QPushButton::clicked, this, &AboutWidget::backClicked);
  connect(ui->btnCode, &QPushButton::clicked, this, [=] {
    emit urlClicked("https://github.com/maemo-leste-extras/jib");
  });
}

AboutWidget::~AboutWidget() {
  delete ui;
}

