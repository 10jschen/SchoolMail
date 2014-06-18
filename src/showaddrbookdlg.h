#ifndef SHOWADDRBOOKDLG_H
#define SHOWADDRBOOKDLG_H

#include <QDialog>
#include <QList>
#include "../src/share/utils.h"
#include "../src/share/db.h"

namespace Ui {
    class ShowAddrBookDlg;
}

class ShowAddrBookDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ShowAddrBookDlg(QWidget *parent = 0);
    ~ShowAddrBookDlg();

    void showAddrBookList();

private:
    Ui::ShowAddrBookDlg *ui;

    QList<AddrBook> *selectAddrList;
    DB db;


private slots:
    void on_ui_delButton_clicked();
    void on_ui_shutdownButton_clicked();
    void on_ui_addButton_clicked();
};

#endif // SHOWADDRBOOKDLG_H
