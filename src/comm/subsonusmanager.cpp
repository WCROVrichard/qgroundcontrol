#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <math.h>
#include "subsonusmanager.h"
#include "QGCApplication.h"

#include <QQmlEngine>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif


SubSonusManager::SubSonusManager(QGCApplication* app, QGCToolbox* toolbox)
    : QGCTool(app, toolbox)
    , tcpSocket(new QTcpSocket(this))
    , _status("initializing")
    , _lat(0)
    , _lon(0)
    , _heading(0)
    , _velN(0)
    , _velE(0)
    , _course(0)
    , _trueVelSqaured(0)
    , _trueVel(0)
    , _range(0)
    , _azimuth(0)

{


}

// mabye don't need destructor?
SubSonusManager::~SubSonusManager() {

    closeConnection();

}

void SubSonusManager::setToolbox(QGCToolbox *toolbox)
{
    QGCTool::setToolbox(toolbox);

    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_5_6);
    in.setByteOrder(QDataStream::LittleEndian);

    connect(tcpSocket, &QIODevice::readyRead, this, &SubSonusManager::updateData);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &SubSonusManager::displayError);

    connectToServer();
    qDebug() << "subsonus manager connecting to server";

}


void SubSonusManager::connectToServer() {

    tcpSocket->abort();

    tcpSocket->connectToHost("192.168.2.101", 16718);
    _status = "connecting";
}

void SubSonusManager::updateData() {

    while(tcpSocket->bytesAvailable() > 0) {

        _holdingBuffer = tcpSocket->readAll(); // get all avail bytes into holding buffer Qbytearray
        // qDebug() << "holdingbuffer size: " << _holdingBuffer.size();
        // calc LRC
        // uint8_t lrcCheck = ((_holdingBuffer[1] + _holdingBuffer[2] + _holdingBuffer[3] + _holdingBuffer[4]) ^ 0xFF) + 1;
        // qDebug() << "LRC from packet: " << _holdingBuffer[0] << " vs. check val: " << lrcCheck;

        // can judge packet types by size: sysState are 121, tracks are 216.
        if(_holdingBuffer.size() == 121) {
            // it's a sysState packet, process it
            // trim off first 5 bytes
            // lat, lon, heading data are in radians
            QByteArray packetData = _holdingBuffer.right(116);
                    // qDebug() << "buffer dump: " << _holdingBuffer;
            QByteArray latData = packetData.mid(16,8);
                     _lat = *reinterpret_cast<double*>(latData.data());
                     _lat *= 180/M_PI;
                    // qDebug() << "lat: " << _lat;

            QByteArray lonData = packetData.mid(24,8);
                    _lon = *reinterpret_cast<double*>(lonData.data());
                    _lon *= 180/M_PI;

            QByteArray headingData = packetData.mid(76,4);
                    _heading = *reinterpret_cast<float*>(headingData.data());
                    _heading *= 180/M_PI;

            QByteArray velNData = packetData.mid(40,4);
            QByteArray velEData = packetData.mid(44,4);
                    _velN = *reinterpret_cast<float*>(velNData.data());
                    _velE = *reinterpret_cast<float*>(velEData.data());
                    _course = atan2(_velE, _velN); // This function returns the angle, in radians, between -pi and pi.
                    _course *= 180.0 / M_PI; // Convert from radians to degrees. Now heading is in the range [-180, 180]
                    if (_course < 0.0) { _course += 360.0 ;} // get rid of negative values
                    _trueVelSqaured = ( pow(_velN, 2) + pow(_velE, 2));
                    _trueVel = sqrt(_trueVelSqaured);
             emit sysDataChanged();
        }
        else if(_holdingBuffer.size() == 216) {
            // it's a track packet, process it
            // trim off first 5 bytes
            QByteArray packetData = _holdingBuffer.right(211);

            QByteArray rangeData = packetData.mid(103,4);
                    _range = *reinterpret_cast<float*>(rangeData.data());
                    // qDebug() << "range: " << _range;
            QByteArray azimuthData = packetData.mid(107,4);
                    _azimuth = *reinterpret_cast<float*>(azimuthData.data());
                    _azimuth *= 180/M_PI;
                    if (_azimuth < 0.0) { _azimuth += 360.0 ;}
            emit trackDataChanged();
        }
        else {
            //something's wrong, bail
            _status = "comm error";
        }
        _status = "good";
        _holdingBuffer = "";  // empty out holding buffer for next go-round
    }
}

void SubSonusManager::displayError(QAbstractSocket::SocketError socketError) {

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        _status = "SubSonus not found";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        _status = "SubSonus refused connection";
        break;
    default:
        _status = "connection error: " + tcpSocket->errorString();
    }

    QTimer::singleShot(5000, this, &SubSonusManager::updateData);
}

void SubSonusManager::closeConnection() {

    tcpSocket->close();
}

