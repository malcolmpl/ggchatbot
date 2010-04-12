#include "imagedescriptionsettings.h"

#include <QtDebug>

ImageDescriptionSettings::ImageDescriptionSettings()
{
    settings = new QSettings("imagedescription.ini", QSettings::IniFormat);

    if(!settings->isWritable() || settings->status() != QSettings::NoError)
        qDebug() << "Error in reading Image description file!";
}

ImageDescriptionSettings::~ImageDescriptionSettings()
{
    delete settings;
}

void ImageDescriptionSettings::readImageDescSettings(QList<ImageDescription> &idescList)
{
    qDebug() << "Read image description file.";

    int size = settings->beginReadArray("imageDescription");

    for(int i=0; i<size; ++i)
    {
        settings->setArrayIndex(i);

        ImageDescription idesc;
        idesc.userbarId = settings->value("UserbarId").toString();
        idesc.beginTime = settings->value("BeginTime").toDateTime();
        idesc.expireTime = settings->value("ExpireTime").toDateTime();

        qDebug() << idesc.userbarId << idesc.beginTime << idesc.expireTime;

        // sprawdzic czy expire uplynal
        if(QDateTime::currentDateTime() < idesc.expireTime)
            idescList.push_back(idesc);
    }

    settings->endArray();
}

void ImageDescriptionSettings::saveImageDescription(QList<ImageDescription> &idescList)
{
    qDebug() << "Save image description file.";
    settings->clear();

    settings->beginWriteArray("imageDescription");

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
