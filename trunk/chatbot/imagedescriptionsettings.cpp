#include "imagedescriptionsettings.h"

#include <QtDebug>

const QString IMAGE_DESC_NAME = "imageDescription";

ImageDescriptionSettings::ImageDescriptionSettings()
{
    settings = QSharedPointer<QSettings>(new QSettings("imagedescription.ini", QSettings::IniFormat));

    if(!settings->isWritable() || settings->status() != QSettings::NoError)
        qDebug() << "Error in reading Image description file!";
}

ImageDescriptionSettings::~ImageDescriptionSettings()
{}

void ImageDescriptionSettings::readImageDescSettings(QList<ImageDescription> &idescList)
{
    qDebug() << "Read image description file.";

    ImageDescription idesc;
    int size = settings->beginReadArray(IMAGE_DESC_NAME);

    for(int i=0; i<size; ++i)
    {
        settings->setArrayIndex(i);
        idesc.userbarId = settings->value("UserbarId").toString();
        idesc.beginTime = settings->value("BeginTime").toDateTime();
        idesc.expireTime = settings->value("ExpireTime").toDateTime();

        // sprawdzic czy expire uplynal

        idescList.push_back(idesc);
    }
}

void ImageDescriptionSettings::saveImageDescription(QList<ImageDescription> &idescList)
{
    qDebug() << "Save image description file.";

    settings->beginWriteArray(IMAGE_DESC_NAME);

    for(int i=0; i<idescList.size(); ++i)
    {
        settings->setArrayIndex(i);
        settings->setValue("UserbarId", idescList.at(i).userbarId);
        settings->setValue("BeginTime", idescList.at(i).beginTime);
        settings->setValue("ExpireTime", idescList.at(i).expireTime);
    }

    settings->endArray();

    settings->sync();
}
