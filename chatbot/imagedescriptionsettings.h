#ifndef IMAGEDESCRIPTIONSETTINGS_H
#define IMAGEDESCRIPTIONSETTINGS_H

#include <QObject>
#include <QSharedPointer>
#include <QSettings>
#include <QDateTime>

class ImageDescription
{
public:
    QString userbarId;
    QDateTime beginTime;
    QDateTime expireTime;
};

class ImageDescriptionSettings : public QObject
{
    Q_OBJECT
public:
    ImageDescriptionSettings();
    ~ImageDescriptionSettings();
    QList<ImageDescription> readImageDescSettings();
    void saveImageDescription(QList<ImageDescription> &);

private:
    QSharedPointer<QSettings> settings;
};

#endif // IMAGEDESCRIPTIONSETTINGS_H
