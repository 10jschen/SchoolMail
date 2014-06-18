#ifndef ADDSIGNATUREDLG_H
#define ADDSIGNATUREDLG_H

#include <QDialog>
#include "../src/share/db.h"
#include "../src/share/utils.h"

namespace Ui {
    class AddSignatureDlg;
}

class AddSignatureDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddSignatureDlg(QWidget *parent = 0);
    ~AddSignatureDlg();

    void addSignatureItems();
    void openDB();

private:
    Ui::AddSignatureDlg *ui;

    DB db;

private slots:
    void on_ui_addButton_clicked();
    void on_ui_closeButton_clicked();
    void showSignature(int index);

};

#endif // ADDSIGNATUREDLG_H
