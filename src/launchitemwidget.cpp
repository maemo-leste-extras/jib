// SPDX-License-Identifier: BSD-3-Clause

#include <QStandardItemModel>
#include <QTableWidget>
#include <QDesktopServices>

#include "mainwindow.h"
#include "launchitemwidget.h"
#include "ui_launchitemwidget.h"

LaunchItemWidget::LaunchItemWidget(AppContext *ctx, const QString &title, const QString &domain, QWidget *parent) :
    m_ctx(ctx),
    m_title(title),
    m_titleShort(title),
    m_domain(domain),
    QWidget(parent),
    ui(new Ui::LaunchItemWidget)
{
  ui->setupUi(this);
  if(m_titleShort.length() >= 10)
    m_titleShort = m_titleShort.left(8) + "...";

  auto pixmap = m_ctx->getThumbPixmap(m_domain, 40, 40);
  ui->label_img->setPixmap(pixmap);
  ui->label_title->setText(m_titleShort);
}

LaunchItemWidget::~LaunchItemWidget() {
  delete ui;
}

