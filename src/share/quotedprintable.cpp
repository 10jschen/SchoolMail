#include "quotedprintable.h"

QuotedPrintable::QuotedPrintable()
{
}

QuotedPrintable::~QuotedPrintable()
{

}

QString
QuotedPrintable::encode(const QByteArray &content)
{
    QString ret;
    char    byte;

    char    hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    for (int i = 0; i < content.length(); i++) {
        byte = content[i];

        if ((content[i] == 0x20) || ((byte >= 33) && (byte <= 126) && (byte != 61))) {
            ret.append(content[i]);
        } else {
            ret.append('=');
            ret.append(hex[(byte >> 4) & 0x0F]);
            ret.append(hex[byte & 0x0F]);
        }
    }

    return ret;
}

QByteArray
QuotedPrintable::decode(const QString &content)
{
    QByteArray  ret;
    int codeValue[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, 10, 11, 12, 13, 14, 15};
    //                 0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?  @  A   B   C   D   E   F

    for (int i = 0; i < content.length(); i++) {
        if (content.at(i).toAscii() == '=')
            ret.append((codeValue[content.at(++i).toAscii() - '0'] << 4) + (codeValue[content.at(++i).toAscii() - '0']));
        else
            ret.append(content.at(i).toAscii());
    }

    return ret;
}
