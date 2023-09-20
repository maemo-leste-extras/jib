// SPDX-License-Identifier: BSD-3-Clause

#include <QStandardItemModel>
#include <QTableWidget>
#include <QDesktopServices>

#include "historywidget.h"
#include "ui_historywidget.h"

HistoryWidget::HistoryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HistoryWidget),
    m_contextMenu(new QMenu(this))
{
  m_ctx = MainWindow::getContext();
  auto mainWindow = MainWindow::getInstance();
  ui->setupUi(this);

  QPixmap p_general_search("/usr/share/icons/hicolor/48x48/hildon/general_search.png");
  ui->iconSearch->setPixmap(p_general_search);

  ui->searchBar->setStyleSheet("QLineEdit { background-color: #8e8e8e; padding-left: 8px; border-radius: 8px; padding-bottom:2px; }");
  this->setStyleSheet("background-color: #575757;");

  this->setupTable();

  connect(ui->historyView, &QTableView::doubleClicked, this, &HistoryWidget::linkClicked);

  ui->historyView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

  ui->historyView->verticalHeader()->setVisible(false);
  ui->historyView->horizontalHeader()->setVisible(false);
  ui->historyView->setSelectionBehavior(QAbstractItemView::SelectRows);

  //ui->historyView->setColumnWidth(2, 160);
  ui->historyView->setFont(QFont("Ubuntu", 18));
  ui->historyView->setModel(m_ctx->historyModel);

  ui->historyView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui->historyView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  ui->historyView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

  connect(ui->btnClear, &QPushButton::clicked, [=]{
    Utils::removeFiles(m_ctx->iconCacheDirectory);
    m_ctx->SqlExec("DELETE FROM visits");
    m_ctx->historyModel->clear();
  });

  connect(ui->btnBack, &QPushButton::clicked, [=]{
    emit backClicked();
  });

  connect(ui->searchBar, &QLineEdit::textChanged, [=](const QString &text) {
    if(text.length() > 2) {
      m_ctx->historyModel->clear();
      m_ctx->historyModel->appendItems(m_ctx->historyModel->load_db(text));
    } else {
      m_ctx->historyModel->clear();
      m_ctx->historyModel->appendItems(m_ctx->historyModel->load_db());
    }
  });

  connect(ui->historyView, &QTableView::doubleClicked, this, &HistoryWidget::linkClicked);
}

void HistoryWidget::linkClicked() {
  QModelIndex index = ui->historyView->currentIndex();
  auto item = m_ctx->historyModel->items.at(index.row());
  emit urlClicked(item->url());
}

void HistoryWidget::setupTable() {
  ui->historyView->verticalHeader()->setVisible(false);
  ui->historyView->setSelectionBehavior(QAbstractItemView::SelectRows);

  //ui->historyView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  ui->historyView->setColumnWidth(2, 160);
}

void HistoryWidget::showContextMenu(const QPoint &pos) {
//  QModelIndex index = ui->historyView->indexAt(pos);
//    if (!index.isValid()) {
//    return;
//  }
//
//  m_contextMenu->exec(ui->historyView->viewport()->mapToGlobal(pos));
}

HistoryWidget::~HistoryWidget() {
  delete ui;
}
