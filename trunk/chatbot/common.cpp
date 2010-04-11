#include "common.h"

namespace GGChatBot
{
    QTextCodec *codec_cp1250 = QTextCodec::codecForName("CP1250");
    QTextCodec *codec_latin2 = QTextCodec::codecForName("ISOi8859-2");

    QString cp2unicode(const QByteArray &buf)
    {
        return codec_cp1250->toUnicode(buf);
    }

    QByteArray unicode2cp(const QString &buf)
    {
        return codec_cp1250->fromUnicode(buf);
    }

    QString makeInternalMessage(QString buf)
    {
        buf.replace(QLatin1String("\r\n"), QString(QChar::LineSeparator));
        buf.replace(QLatin1String("\n"), QString(QChar::LineSeparator));
        buf.replace(QLatin1String("\r"), QString(QChar::LineSeparator));
        return buf;
    }

    QString makeMessage(QString buf)
    {
        buf.replace("\r\n", "\n");
        buf.replace("\r", "\n");
        buf.replace(QChar::LineSeparator, "\n");
        return buf;
    }

    QDateTime getDateTime()
    {
        return QDateTime::currentDateTime().toUTC().addSecs(7200);
    }
}
