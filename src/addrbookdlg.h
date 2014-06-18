#ifndef ADDRBOOKDLG_H
#define ADDRBOOKDLG_H

#include <QDialog>
#include "../src/share/utils.h"

namespace Ui {
    class AddrBookDlg;
}

class AddrBookDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddrBookDlg(AddrBook *addrBook, QWidget *parent = 0);
    ~AddrBookDlg();

private:
    Ui::AddrBookDlg *ui;

    AddrBook *addrBook;

private slots:
    void on_ui_cancelButton_clicked();
    void on_ui_addButton_clicked();
};

#endif // ADDRBOOKDLG_H
