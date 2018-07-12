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

public:
    SubSonusManager(QGCApplication* app, QGCToolbox* toolbox);
    ~SubSonusManager();

    // Override from QGCTool
    virtual void setToolbox(QGCToolbox *toolbox);

    // SubSonus system state values
    Q_PROPERTY(QString sysStatus READ status NOTIFY sysDataChanged)
    Q_PROPERTY(double latitude READ latitude NOTIFY sysDataChanged)
    Q_PROPERTY(double longitude READ longitude NOTIFY sysDataChanged)
    Q_PROPERTY(float heading READ heading NOTIFY sysDataChanged)
    Q_PROPERTY(float course READ course NOTIFY sysDataChanged)
    Q_PROPERTY(float trueVel READ trueVel NOTIFY sysDataChanged)

    // SubSonus remote vehicle tracking values
    Q_PROPERTY(float range READ range NOTIFY trackDataChanged)
    Q_PROPERTY(float azimuth READ azimuth NOTIFY trackDataChanged)
    Q_PROPERTY(float ROVcourse READ ROVcourse NOTIFY trackDataChanged)
    Q_PROPERTY(float ROVvelocity READ ROVvelocity NOTIFY trackDataChanged)


    // values for position manager to access
    QString status()          { return _status; }
    double latitude()         { return _lat; }
    double longitude()        { return _lon; }
    float heading()           { return _heading; }
    float course()            { return _course; }
    float trueVel()           { return _trueVel; }
    float range()             { return _range; }
    float azimuth()           { return _azimuth; }
    float ROVcourse()         { return _ROVcourse; }
    float ROVvelocity()       { return _ROVvelocity; }

signals:
    void sysDataChanged();
    void trackDataChanged();

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
    float _trueVelSquared;
    float _trueVel;
    float _range;
    float _azimuth;
    float _ROVvelN;
    float _ROVvelE;
    float _ROVcourse;
    float _ROVvelocity;
    float _ROVvelSquared;
    QByteArray _holdingBuffer;

private slots:
    void connectToServer();
    void updateData();
    void closeConnection();
    void displayError(QAbstractSocket::SocketError socketError);

};

#endif // SUBSONUSMANAGER_H
