#ifndef IMAGEDESCRIPTIONSETTINGS_H
#define IMAGEDESCRIPTIONSETTINGS_H

#include <QObject>
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
    void readImageDescSettings(QList<ImageDescription> &);
    void saveImageDescription(QList<ImageDescription> &);

private:
    QSettings *settings;
};

#endif // IMAGEDESCRIPTIONSETTINGS_H
