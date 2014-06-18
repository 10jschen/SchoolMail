#ifndef QUOTEDPRINTABLE_H
#define QUOTEDPRINTABLE_H

#include <QObject>

class QuotedPrintable : public QObject
{
    Q_OBJECT
public:
    QuotedPrintable();
    ~QuotedPrintable();

    static QString encode(const QByteArray& content);
    static QByteArray decode(const QString& content);
    
signals:
    
public slots:
    
};

#endif // QUOTEDPRINTABLE_H
