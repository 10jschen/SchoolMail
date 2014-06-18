#ifndef SHOWMESSAGE_H
#define SHOWMESSAGE_H

#include <QWidget>
#include <Python.h>
#include <QStringList>

namespace Ui {
class ShowMessage;
}

class ShowMessage : public QWidget
{
    Q_OBJECT
    
public:
    explicit ShowMessage(PyObject *pClass, QString mailAddr, QString subject,
                         QString sender, QString date, QWidget *parent = 0);
    ~ShowMessage();

    void listMessage();
    void buildDir();
    void delDir();
    void decodeMsg();
    void getAttachList();
    void getTo();
    void getCc();

protected:

    
private:
    Ui::ShowMessage *ui;

    PyObject *pClass;

    QString mailAddr;
    QString subject;
    QString sender;
    QString date;
    QString msgUidl;
    QStringList attachList;
    QString recipientTo;
    QString recipientCc;

private slots:
    void on_ui_attachButton_clicked();
};

#endif // SHOWMESSAGE_H
