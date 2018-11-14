// Copyright (c) 2011-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
 #ifndef BITCOIN_QT_MINTINGPAGE_H
#define BITCOIN_QT_MINTINGPAGE_H
 #include <amount.h>
#include <QDialog>
#include <memory>
#include <QTableWidget>
class ClientModel;
class TxViewDelegate;
class PlatformStyle;
class WalletModel;
 namespace Ui {
    class MintingPage;
}
 QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE
 /** Overview ("home") page widget */
class MintingPage : public QDialog
{
    Q_OBJECT
 public:
    explicit MintingPage(const PlatformStyle *platformStyle, QDialog *parent = 0);
    ~MintingPage();
    void setClientModel(ClientModel *clientModel);
    void setWalletModel(WalletModel *walletModel);
    void showOutOfSyncWarning(bool fShow);
 Q_SIGNALS:
    void transactionClicked(const QModelIndex &index);
    void outOfSyncWarningClicked();
 private:
    Ui::MintingPage *ui;
    QTableWidgetItem *item;
 };
 #endif // BITCOIN_QT_MintingPage_H
