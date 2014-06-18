#include <QtGui/QApplication>
#include <QTextCodec>
#include "schoolmail.h"
#include <QMessageBox>
#include <QDir>

void buildDir()
{
    QDir *dir = new QDir;

    if (!dir->exists("./schoolmail")) {
        dir->mkdir("./schoolmail");
    }
    if (!dir->exists("./schoolmail/mail")) {
        dir->mkdir("./schoolmail/mail");
    }
    if (!dir->exists("./schoolmail/tmp")) {
        dir->mkdir("./schoolmail/tmp");
    }
    delete dir;
}

int
main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    buildDir();
    SchoolMail   schoolMail(NULL);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
    schoolMail.setWindowTitle("校园邮件客户端");
    schoolMail.showMaximized();



    a.exec();
}
