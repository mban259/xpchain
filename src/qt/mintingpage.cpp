// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#include <qt/addresstablemodel.h>
#include <qt/overviewpage.h>
#include <qt/mintingpage.h>
#include <qt/mintingfilterproxy.h>
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
#include <rpc/blockchain.h>
#include <interfaces/wallet.h>
#include <QAbstractItemDelegate>
#include <QPainter>
#include <key_io.h>
#include <consensus/consensus.h>
#include <timedata.h>
#include <validation.h>
#include <stdint.h>
#include <chain.h>
#include <chainparams.h>
#include <math.h>
#include <uint256.h>
#include <base58.h>
#define DECORATION_SIZE 54
#define NUM_ITEMS 5

 MintingPage::MintingPage(const PlatformStyle *platformStyle, QDialog *parent) :
    QDialog(parent),
    ui(new Ui::MintingPage),
    walletModel(0)
{
    ui->setupUi(this);


}
 MintingPage::~MintingPage()
{
    delete ui;
}
void MintingPage::setWalletModel(WalletModel *model)
{
    this->walletModel = model;

    if(model && model->getOptionsModel() && model->getAddressTableModel())
    {
        MintingPage::update();
        interfaces::Wallet& wallet = model->wallet();
        interfaces::WalletBalances balances = wallet.getBalances();
        connect(model, SIGNAL(balanceChanged(interfaces::WalletBalances)), this, SLOT(update()));
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(update()));
    }
}
void MintingPage::update()
{
    if (!walletModel || !walletModel->getOptionsModel() || !walletModel->getAddressTableModel())
        return;

    //Table Initialize
    QTableWidgetItem *item ;
    QTableWidget* mintingtableWidget = ui->mintingtableWidget;
    mintingtableWidget->clearContents();
    mintingtableWidget->setRowCount(0);
    mintingtableWidget->setSortingEnabled(false);
    
    //Can write in ui
    mintingtableWidget->verticalHeader()->hide();
    mintingtableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color:silver }");

    //Unclassified
    mintingtableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //Can't write in ui
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Interactive);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Interactive);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Interactive);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(3,QHeaderView::Interactive);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(4,QHeaderView::Interactive);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(5,QHeaderView::Interactive);
    mintingtableWidget->horizontalHeader()->setSectionResizeMode(6,QHeaderView::Interactive);
    // Keep up to date with wallet
    //interfaces::Wallet& wallet = walletModel->wallet();
    int nDisplayUnit = walletModel->getOptionsModel()->getDisplayUnit();
    int i = 0;
    int row = mintingtableWidget->rowCount();
    for (const auto& coins : walletModel->wallet().listCoins()) {
      QString sWalletAddress = QString::fromStdString(EncodeDestination(coins.first));
      QString sWalletLabel = walletModel->getAddressTableModel()->labelForAddress(sWalletAddress);
      for (const auto& outpair : coins.second) {
        const COutPoint& output = std::get<0>(outpair);
        const interfaces::WalletTxOut& out = std::get<1>(outpair);

        // transaction hash
        QString hashstr = QString::fromStdString(output.hash.GetHex());

        // address
        CTxDestination outputAddress;
        QString sAddress = "";
        if(ExtractDestination(out.txout.scriptPubKey, outputAddress))
        {
          sAddress = QString::fromStdString(EncodeDestination(outputAddress));
        }

        // label
        /*if (!(sAddress == sWalletAddress)) // change
        {
          // tooltip from where the change comes from
          QString label_tp = tr("change from %1 (%2)").arg(sWalletLabel).arg(sWalletAddress));
          //itemOutput->setText(COLUMN_LABEL, tr("(change)"));
        }
        QString sLabel = walletModel->getAddressTableModel()->labelForAddress(sAddress);
        if (sLabel.isEmpty())
          sLabel = tr("(no label)");
        itemOutput->setText(COLUMN_LABEL, sLabel);*/

        // Balance
        
        // Age
        int64_t Age = (GetAdjustedTime() - out.time) / 86400;

        //CoinDay
        int64_t coinDay = 0;
        int64_t nWeight = 0;
        nWeight = GetAdjustedTime() - out.time - Params().GetConsensus().nStakeMinAge;
        if(nWeight >= 0){
          nWeight = (std::min)(nWeight, (int64_t)Params().GetConsensus().nStakeMaxAge);
          coinDay = ((out.txout.nValue * nWeight ) / (COIN * 86400));
        }

        //Reward
        int64_t PoSReward = 0;
        if( nWeight >=  Params().GetConsensus().nStakeMinAge){
        PoSReward = GetProofOfStakeReward(chainActive.Tip()->nHeight, out.txout.nValue, out.time, Params().GetConsensus());
        }

        //Probability
        //const CBlockIndex *p = GetLastBlockIndex(chainActive.Tip(), true);
        double difficulty = GetDifficulty(chainActive.Tip());

        /*double prob = wtx->getProbToMintWithinNMinutes(difficulty, mintingInterval);
        prob = prob * 100;
        return prob;*/

        mintingtableWidget->insertRow (row + i);
        mintingtableWidget->setItem(row + i,0,item = new QTableWidgetItem);
        item->setData(Qt::DisplayRole,hashstr);
        mintingtableWidget->setItem(row + i,1,item = new QTableWidgetItem);
        item->setData(Qt::DisplayRole,sAddress);
        mintingtableWidget->setItem(row + i,2,item = new QTableWidgetItem);
        item->setData(Qt::DisplayRole, BitcoinUnits::format(nDisplayUnit, out.txout.nValue).toInt());
        item->setData(Qt::UserRole, (qlonglong)out.txout.nValue);
        mintingtableWidget->setItem(row + i,3,item = new QTableWidgetItem);
        item->setData(Qt::DisplayRole, (qlonglong)Age);
        item->setBackground(QColor(100, 10, 125));
        mintingtableWidget->setItem(row + i,4,item = new QTableWidgetItem);
        item->setData(Qt::DisplayRole, (qlonglong)nWeight);
        item->setBackground(QColor(100, 10, 125));
        mintingtableWidget->setItem(row + i,5,item = new QTableWidgetItem);
        item->setData(Qt::DisplayRole, difficulty);
        mintingtableWidget->setItem(row + i,6,item = new QTableWidgetItem);
        item->setData(Qt::DisplayRole, BitcoinUnits::format(nDisplayUnit, PoSReward));
        item->setData(Qt::UserRole, (qlonglong)PoSReward);
        i++;






      }
    }
    
    mintingtableWidget->setSortingEnabled(true);
    /*for (const auto& wtx : wallet.getWalletTxs()) {
        //
        // Credit
        //
        for(unsigned int j = 0; j < wtx.tx->vout.size(); j++)
         {
          const CTxOut& txout = wtx.tx->vout[j];
          isminetype mine = wtx.txout_is_mine[j];
          if(mine)
           {
            if (wtx.txout_address_is_mine[j] == ISMINE_SPENDABLE)
              {
                    // Received by Bitcoin Address
              uint256 hash = wtx.tx->GetHash();
              QString hashstr = QString::fromStdString(hash.ToString());
              
              QString Addstr = QString::fromStdString(EncodeDestination(wtx.txout_address[j]));
              
              mintingtableWidget->insertRow (row + i);
              mintingtableWidget->setItem(row + i,0,item = new QTableWidgetItem);
              item->setData(Qt::DisplayRole,hashstr);
              mintingtableWidget->setItem(row + i,1,item = new QTableWidgetItem);
              item->setData(Qt::DisplayRole,"いずまいん2");
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
              i++;
              }
            else if (wtx.txout_address_is_mine[j] == ISMINE_WATCH_ONLY)
              {
                    // Received by Bitcoin Address
              uint256 hash = wtx.tx->GetHash();
              QString hashstr = QString::fromStdString(hash.ToString());
              
              QString Addstr = QString::fromStdString(EncodeDestination(wtx.txout_address[j]));
              
              mintingtableWidget->insertRow (row + i);
              mintingtableWidget->setItem(row + i,0,item = new QTableWidgetItem);
              item->setData(Qt::DisplayRole,hashstr);
              mintingtableWidget->setItem(row + i,1,item = new QTableWidgetItem);
              item->setData(Qt::DisplayRole,"いずまいん1");
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
              i++;
              }
            }
         }
    }
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

    }*/
}
