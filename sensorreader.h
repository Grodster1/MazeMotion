#ifndef SENSORREADER_H
#define SENSORREADER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>

class SensorReader : public QObject
{
    Q_OBJECT
    QSerialPort *serialPort;
    QByteArray buffer;
public:
    explicit SensorReader(QObject *parent = nullptr);
    void open(const QString &portName);

signals:
    void dataReceived(float ax, float ay, float az, float gx, float gy, float gz);

private slots:
    void onReadyRead();
};

#endif // SENSORREADER_H
