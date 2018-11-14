// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <qt/overviewpage.h>
#include <qt/mintingpage.h>
#include <qt/forms/ui_mintingpage.h>
#include <qt/bitcoinunits.h>
#include <qt/clientmodel.h>
#include <qt/guiconstants.h>
#include <qt/guiutil.h>
#include <qt/optionsmodel.h>
#include <qt/platformstyle.h>
#include <qt/transactionfilterproxy.h>
#include <qt/transactiontablemodel.h>
#include <qt/walletmodel.h>
#include <QAbstractItemDelegate>
#include <QPainter>
#define DECORATION_SIZE 54
#define NUM_ITEMS 5

 MintingPage::MintingPage(const PlatformStyle *platformStyle, QDialog *parent) :
    QDialog(parent),
    ui(new Ui::MintingPage)
{
    ui->setupUi(this);

    //テーブル個別設定
    QTableWidgetItem *item ;
    QTableWidget* mintingtableWidget = ui->mintingtableWidget;

    //uiファイルに書ける
    mintingtableWidget->verticalHeader()->hide();
    mintingtableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:silver }");

    //uiファイルに書けるか未分類
    mintingtableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //uiファイルには書けない
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeToContents);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(4,QHeaderView::ResizeToContents);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeToContents);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(6,QHeaderView::ResizeToContents);


    //実験
    int row = mintingtableWidget->rowCount();
    for (int i = 0; i < 20; ++i){
      mintingtableWidget->insertRow (row + i);
      mintingtableWidget->setItem(row + i,0,item = new QTableWidgetItem);
      item->setData(Qt::DisplayRole,"transaction:string");
      mintingtableWidget->setItem(row + i,1,item = new QTableWidgetItem);
      item->setData(Qt::DisplayRole,"Address:string");
      mintingtableWidget->setItem(row + i,2,item = new QTableWidgetItem);
      item->setData(Qt::DisplayRole, (row + i));
      mintingtableWidget->setItem(row + i,3,item = new QTableWidgetItem);
      item->setData(Qt::DisplayRole, (row + i));
      item->setBackground(QColor(100, 10, 125));
      mintingtableWidget->setItem(row + i,4,item = new QTableWidgetItem);
      item->setData(Qt::DisplayRole, (row + i) * 10);
      item->setBackground(QColor(100, 10, 125));
      mintingtableWidget->setItem(row + i,5,item = new QTableWidgetItem);
      item->setData(Qt::DisplayRole, (row + i) * 0.01);
      mintingtableWidget->setItem(row + i,6,item = new QTableWidgetItem);
      item->setData(Qt::DisplayRole, (row + i) * 100);

    }
}
 MintingPage::~MintingPage()
{
    delete ui;
}
