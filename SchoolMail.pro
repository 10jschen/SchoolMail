#-------------------------------------------------
#
# Project created by QtCreator 2013-11-26
#
#-------------------------------------------------

QT       += core gui network thread
QT       += sql

TARGET = SchoolMail
TEMPLATE = app

LIBS += -LD:/software/python2.7/libs -lpython27
INCLUDEPATH += D:/software/python2.7/include
#CONFIG += no_keywords

HEADERS += \
    src/schoolmail.h \
    src/smtp/emailaddress.h \
    src/smtp/mimeattachment.h \
    src/smtp/mimefile.h \
    src/smtp/mimehtml.h \
    src/smtp/mimeinlinefile.h \
    src/smtp/mimemessage.h \
    src/smtp/mimepart.h \
    src/smtp/mimetext.h \
    src/smtp/smtpclient.h \
    src/smtp/SmtpMime \
    src/smtp/quotedprintable.h \
    src/smtp/mimemultipart.h \
    src/smtp/mimecontentformatter.h \
    src/share/db.h \
    src/share/mime.h \
    src/share/utils.h\
    src/showmessage.h \
    src/sendmessage.h \
    src/pop3/pop3.h \
    src/addmailboxdlg.h \
    src/addrbookdlg.h \
    src/addsignaturedlg.h \
    src/showaddrbookdlg.h \
    src/addattachmentdlg.h \
    src/pythreadstatelock.h \
    src/mythread.h \
    src/downloadattachdlg.h \
    src/smtp/mimethread.h

SOURCES += \
    src/schoolmail.cpp \
    src/main.cpp \
    src/smtp/emailaddress.cpp \
    src/smtp/mimeattachment.cpp \
    src/smtp/mimefile.cpp \
    src/smtp/mimehtml.cpp \
    src/smtp/mimeinlinefile.cpp \
    src/smtp/mimemessage.cpp \
    src/smtp/mimepart.cpp \
    src/smtp/mimetext.cpp \
    src/smtp/smtpclient.cpp \
    src/smtp/quotedprintable.cpp \
    src/smtp/mimemultipart.cpp \
    src/smtp/mimecontentformatter.cpp \
    src/share/db.cpp \
    src/showmessage.cpp \
    src/sendmessage.cpp \
    src/pop3/pop3.cpp \
    src/addmailboxdlg.cpp \
    src/addrbookdlg.cpp \
    src/addsignaturedlg.cpp \
    src/showaddrbookdlg.cpp \
    src/addattachmentdlg.cpp \
    src/pythreadstatelock.cpp \
    src/mythread.cpp \
    src/downloadattachdlg.cpp \
    src/share/mime.cpp \
    src/smtp/mimethread.cpp

FORMS += \
    src/ui/schoolmail.ui \
    src/ui/showmessage.ui \
    src/ui/sendmessage.ui \
    src/ui/addmailboxdlg.ui \
    src/ui/addrbookdlg.ui \
    src/ui/addsignaturedlg.ui \
    src/ui/showaddrbookdlg.ui \
    src/ui/addattachmentdlg.ui \
    src/ui/downloadattachdlg.ui

RESOURCES += \
    src/qrc.qrc
