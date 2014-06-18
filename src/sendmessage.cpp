#include "sendmessage.h"
#include "ui_sendmessage.h"
#include <QMessageBox>
#include <QStringList>
#include <QDebug>
#include "addattachmentdlg.h"
#include <QtConcurrentRun>
#include <QFileDialog>
#include <QTextImageFormat>
#include "../src/smtp/mimethread.h"
#include "../src/share/utils.h"
#include <QImageReader>

SendMessage::SendMessage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendMessage)
{
    ui->setupUi(this);
    setAttribute (Qt::WA_DeleteOnClose);
    QObject::connect(ui->ui_fontComboBox, SIGNAL(currentFontChanged(QFont)),
                     this, SLOT(setTextFont(QFont)));
    QObject::connect(ui->ui_sizeComboBox, SIGNAL(currentIndexChanged(QString)),
                     this, SLOT(setTextSize(QString)));
    QObject::connect(ui->ui_signature, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(setSignature(int)));
    init();
}

SendMessage::~SendMessage()
{
    delete ui;
}

void SendMessage::init()
{
    if (!db.openDB("./mail_db.db")) {
        QMessageBox::information(this, "提示", "无法打开数据库！");
        return;
    }

    initServerInfoList();
    //initAddrBookList();
    initSignature();
}

void SendMessage::initServerInfoList()
{
    if (!db.getMailServerInfoList(serverInfoList)) {
        qDebug() << "没有ServerInfo信息!";
        return;
    }

    for (int i = 0; i < serverInfoList.count(); ++i) {
        ui->ui_senderComboBox->addItem(serverInfoList.at(i).user);
    }
}

void SendMessage::initAddrBookList()
{
    if (!db.getAddrBookList(addrBookList)) {
        qDebug() << "initAddrBookList出错！";
        return;
    }
}

void SendMessage::initSignature() {
    ui->ui_signature->setCurrentIndex(0);
    if (!db.getSignatures(signatureList)) {
        qDebug() << "initSignature出错！";
        return;
    }

    for (int i = 0; i < signatureList.count(); ++i) {
        ui->ui_signature->addItem(signatureList.at(i).title);
    }
}

void SendMessage::setSignature(int index)
{
    if (index <= 0)
        return;

    if (signatureList.count() == 0)
        return;
    qDebug() << index;
    qDebug() << signatureList.at(index - 1).context;
    ui->ui_content->append("<br/><br/><br/>--------------------");
    ui->ui_content->append(signatureList.at(index - 1).context);
}

EmailAddress* SendMessage::stringToEmail(const QString &str)
{
    int p1 = str.indexOf("<");
    int p2 = str.indexOf(">");

    if (p1 == -1) {
        return new EmailAddress(str);
    }
    else {
        return new EmailAddress(str.mid(p1 + 1, p2 - p1 - 1), str.left(p1));
    }

}

void SendMessage::sendCommonMail()
{

}

void SendMessage::sendDifMail()
{

}

void SendMessage::showConnectHostError()
{
    QMessageBox::information(this, "提示", "连接服务器失败!");
}

void SendMessage::showLoginError()
{
    QMessageBox::information(this, "提示", "登陆失败!");
}

void SendMessage::showSendMailError()
{
    QMessageBox::information(this, "提示", "发送失败，请检查网络！");
}

void SendMessage::showSendMailSuccess()
{
    QMessageBox::information(this, "提示", "邮件发送成功!");
}

void SendMessage::on_ui_sendButton_clicked()
{
    MimeInfo mimeInfo;

    if (ui->ui_senderComboBox->count() == 0) {
        QMessageBox::information(this, "提示", "请选择发送者!");
        return;
    }
    mimeInfo.mailAddr = ui->ui_senderComboBox->currentText();

    if (ui->ui_recipient->text().isEmpty()
        && ui->ui_cc->text().isEmpty() && ui->ui_bcc->text().isEmpty()) {
        QMessageBox::information(this, "提示", "请输入必要的联系人!");
        return;
    }
    if (!ui->ui_recipient->text().isEmpty())
        mimeInfo.to = ui->ui_recipient->text();
    if (!ui->ui_cc->text().isEmpty())
        mimeInfo.cc = ui->ui_cc->text();
    if (!ui->ui_bcc->text().isEmpty())
        mimeInfo.bcc = ui->ui_bcc->text();

    if (ui->ui_subject->text().isEmpty()) {
        QMessageBox::information(this, "提示", "邮件主题不能为空！");
        return;
    }
    mimeInfo.subject = ui->ui_subject->text();

    if (!ui->ui_content->toHtml().isEmpty()) {
        mimeInfo.content = ui->ui_content->toHtml();
    }

    for (int i = 0; i < serverInfoList.count(); ++i) {
        if (serverInfoList.at(i).user == mimeInfo.mailAddr) {
            mimeInfo.passwd = serverInfoList.at(i).passwd;
            mimeInfo.smtpServer = serverInfoList.at(i).smtpServer;
            mimeInfo.smtpPort = serverInfoList.at(i).smtpServerPort;
            mimeInfo.ssl = serverInfoList.at(i).serverState;
            break;
        }
    }
    try {
        MimeThread *mimeThread = new MimeThread(mimeInfo, attachList, inlineFileList);

        QObject::connect(mimeThread, SIGNAL(connectHostError()),
                        this, SLOT(showConnectHostError()));
        QObject::connect(mimeThread, SIGNAL(loginError()),
                        this, SLOT(showLoginError()));
        QObject::connect(mimeThread, SIGNAL(sendMailError()),
                        this, SLOT(showSendMailError()));
        QObject::connect(mimeThread, SIGNAL(sendMailSuccess()),
                        this, SLOT(showSendMailSuccess()));
        mimeThread->start();
    } catch(...) {
        qDebug() << "^^^^^^^^^^ exception ^^^^^^^^^^^";
    }
}

void SendMessage::on_ui_leftButton_clicked()
{
    ui->ui_content->setAlignment(Qt::AlignLeft);
}

void SendMessage::on_ui_centerButton_clicked()
{
    ui->ui_content->setAlignment(Qt::AlignCenter);
}

void SendMessage::on_ui_rightButton_clicked()
{
    ui->ui_content->setAlignment(Qt::AlignRight);
}

void SendMessage::setTextFont(const QFont &font)
{
    ui->ui_content->setFont(font);
}

void SendMessage::setTextSize(const QString &sizeStr)
{
    ui->ui_content->setFontPointSize((qreal)sizeStr.toFloat());
}

void SendMessage::on_ui_BoldButton_clicked()
{
    QTextCharFormat fmt;

    QTextCursor cursor = ui->ui_content->textCursor();

    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);

    if (cursor.charFormat().fontWeight() == QFont::Bold)
        fmt.setFontWeight(QFont::Normal);
    else {
        fmt.setFontWeight(QFont::Bold);
    }

    cursor.mergeCharFormat(fmt);
    qDebug()<<"cursor.selectedText()*******************"<<cursor.selectedText();
    ui->ui_content->mergeCurrentCharFormat(fmt);
}

void SendMessage::on_ui_italicButton_clicked()
{
    QTextCharFormat fmt;

    QTextCursor cursor = ui->ui_content->textCursor();

    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);

    fmt.setFontItalic(true);

    cursor.mergeCharFormat(fmt);
    qDebug()<<"cursor.selectedText()*******************"<<cursor.selectedText();
    ui->ui_content->mergeCurrentCharFormat(fmt);
}

void SendMessage::on_ui_attachButton_clicked()
{
    AddAttachmentDlg *addAttachmentDlg = new AddAttachmentDlg(&attachList);

    addAttachmentDlg->show();
}

void SendMessage::on_ui_addPicButton_clicked()
{
    qDebug() << "supported format:" << QImageReader::supportedImageFormats();
    QString file = QFileDialog::getOpenFileName(this, tr("选择一张图片"),
                                ".", tr("Images (*.png *.gif *.jpg *jpeg)"));

    if (file.isEmpty())
        return;

    QTextCursor cursor = ui->ui_content->textCursor();

    QImage img(file);
    if(img.isNull())
        qDebug() << "image " << file << " is null.";
    cursor.insertImage(img, file);
    inlineFileList.append(file);
    qDebug() << "******" + file + "*******";
}

void SendMessage::on_ui_addNameButton_clicked()
{
    ui->ui_content->insertPlainText(QString("[/name/]"));
}
