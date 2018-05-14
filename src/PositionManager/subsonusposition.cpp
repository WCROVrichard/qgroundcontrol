
#include <QtCore>
#include <QDateTime>
#include <QDate>
#include "subsonusposition.h"
#include "subsonusmanager.h"
#include "QGCApplication.h"
#include "QGC.h"



// SeaView test tank location: 42.341440, -83.875043
// default location 47.3977420*1e7, 8.5455941*1e7 outside of Zurich

SubsonusPosition::SubsonusPosition()
    : QGeoPositionInfoSource(NULL),
      lat_int(42.341440),
      lon_int(-83.875043),
      subLat(0),
      subLon(0),
      subCourse(0),
      subHeading(0),
      subTrueVel(0),
      _subsonusManager(qgcApp()->toolbox()->subsonusManager())
{
    connect(&update_timer, &QTimer::timeout, this, &SubsonusPosition::updatePosition);

    connect(_subsonusManager, &SubSonusManager::sysDataChanged, this, &SubsonusPosition::updatePosition);
}

QGeoPositionInfo SubsonusPosition::lastKnownPosition(bool /*fromSatellitePositioningMethodsOnly*/) const
{
    return lastPosition;
}

SubsonusPosition::PositioningMethods SubsonusPosition::supportedPositioningMethods() const
{
    return AllPositioningMethods;
}

int SubsonusPosition::minimumUpdateInterval() const
{
    return 5000;
}

void SubsonusPosition::startUpdates()
{
    int interval = updateInterval();
    if (interval < minimumUpdateInterval())
        interval = minimumUpdateInterval();

    update_timer.setSingleShot(false);
    update_timer.start(interval);
}

void SubsonusPosition::stopUpdates()
{
    update_timer.stop();
}

void SubsonusPosition::requestUpdate(int /*timeout*/)
{
    emit updateTimeout();
}

void SubsonusPosition::updatePosition()
{

    double latitude = (double) (lat_int);
    double longitude = (double) (lon_int);

    // we can get these values from the Subsonus system:
    // lat, lon, heading, COG and SOG (in meters/sec)

    subLat = _subsonusManager->latitude();
    if (subLat != 0)  {  //Subsonus is getting data, so we're good to use it
        latitude = (_subsonusManager->latitude());
        longitude = (_subsonusManager->longitude());
    }
    // otherwise proceed with default lat/lon positions
    // even if these are zero that's OK
    subHeading = _subsonusManager->heading();
    subCourse = _subsonusManager->course();
    subTrueVel = _subsonusManager->trueVel();


    QDateTime timestamp = QDateTime::currentDateTime();

    QGeoCoordinate position(latitude, longitude);
    // qDebug() << "position from subsonposition: " << position;
    QGeoPositionInfo info(position, timestamp);

    info.setAttribute(QGeoPositionInfo::Attribute::Direction, subCourse);
    info.setAttribute(QGeoPositionInfo::Attribute::GroundSpeed, subTrueVel);

    lastPosition = info;
    // this is inherited from QGeoPositionSource
    emit positionUpdated(info);
}

QGeoPositionInfoSource::Error SubsonusPosition::error() const
{
    return QGeoPositionInfoSource::NoError;
}
