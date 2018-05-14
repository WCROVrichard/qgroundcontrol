#ifndef SUBSONUSMANAGER_H
#define SUBSONUSMANAGER_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QDateTime>
#include <QString>
#include "QGCToolbox.h"



class SubSonusManager : public QGCTool
{


    Q_OBJECT
    // SubSonus system state values
    Q_PROPERTY(QString sysStatus MEMBER _status NOTIFY sysDataChanged)
    Q_PROPERTY(double latitude MEMBER _lat NOTIFY sysDataChanged)
    Q_PROPERTY(double longitude MEMBER _lon NOTIFY sysDataChanged)
    Q_PROPERTY(float heading MEMBER _heading NOTIFY sysDataChanged)
    Q_PROPERTY(float course MEMBER _course NOTIFY sysDataChanged)
    Q_PROPERTY(float trueVel MEMBER _trueVel NOTIFY sysDataChanged)

    // SubSonus remote vehicle tracking values
    Q_PROPERTY(float range MEMBER _range NOTIFY trackDataChanged)
    Q_PROPERTY(float azimuth MEMBER _azimuth NOTIFY trackDataChanged)

signals:
    void sysDataChanged();
    void trackDataChanged();

public:
    SubSonusManager(QGCApplication* app, QGCToolbox* toolbox);
    ~SubSonusManager();

    // Override from QGCTool
    virtual void setToolbox(QGCToolbox *toolbox);

    // values for position manager to access
    double latitude()         { return _lat; }
    double longitude()        { return _lon; }
    float heading()           { return _heading; }
    float course()            { return _course; }
    float trueVel()           { return _trueVel; }
    float range()             { return _range; }
    float azimuth()           { return _azimuth; }

private:
    QTcpSocket *tcpSocket;
    QDataStream in;
    QString _status;
    double _lat;
    double _lon;
    float _heading;
    float _velN;
    float _velE;
    float _course;
    float _trueVelSqaured;
    float _trueVel;
    float _range;
    float _azimuth;
    QByteArray _holdingBuffer;

private slots:
    void connectToServer();
    void updateData();
    void closeConnection();
    void displayError(QAbstractSocket::SocketError socketError);

};

#endif // SUBSONUSMANAGER_H
