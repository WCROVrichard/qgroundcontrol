#ifndef SUBSONUSPOSITION_H
#define SUBSONUSPOSITION_H

#pragma once

#include <QtPositioning/qgeopositioninfosource.h>
#include "subsonusmanager.h"
#include <QTimer>
#include <QObject>


class SubsonusPosition : public QGeoPositionInfoSource
{

    Q_OBJECT

 public:
     SubsonusPosition();

     QGeoPositionInfo lastKnownPosition(bool fromSatellitePositioningMethodsOnly = false) const;

     PositioningMethods supportedPositioningMethods() const;
     int minimumUpdateInterval() const;
     Error error() const;

public slots:
    virtual void startUpdates();
    virtual void stopUpdates();

    virtual void requestUpdate(int timeout = 5000);

private slots:
    void updatePosition();

private:

    int32_t lat_int;
    int32_t lon_int;

    double subLat;
    double subLon;
    float subCourse;
    float subHeading;
    float subTrueVel;
    QTimer update_timer;

    QGeoPositionInfo lastPosition;

    SubSonusManager*    _subsonusManager;

};

#endif // SUBSONUSPOSITION_H

