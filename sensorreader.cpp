#include "sensorreader.h"

SensorReader::SensorReader(QObject *parent) : QObject{parent}
{
    serialPort = new QSerialPort(this);
    serialPort->setBaudRate(QSerialPort::Baud115200);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);

    connect(serialPort, &QSerialPort::readyRead, this, &SensorReader::onReadyRead);
}

void SensorReader::open(const QString &portName){

    serialPort->setPortName(portName);
    serialPort->open(QIODevice::ReadOnly);
    if(serialPort->isOpen()){
        qDebug() << "Serial Port Is Connected";
        qDebug() << serialPort->error();
    }
    else{
        qDebug() << "Serial Port Is Not Connected";
        qDebug() << serialPort->error();
    }
}

void SensorReader::onReadyRead(){
    buffer += serialPort->readAll();
    int index = buffer.indexOf('\n');

    while(index != -1){
        QByteArray byteLine = buffer.left(index);
        buffer.remove(0, index+1);
        QString line = QString::fromUtf8(byteLine).trimmed();
        QStringList list = line.split(',');

        if(list.size() == 6){
            float ax = list[0].toFloat();
            float ay = list[1].toFloat();
            float az = list[2].toFloat();

            float gx = list[3].toFloat();
            float gy = list[4].toFloat();
            float gz = list[5].toFloat();

            emit dataReceived(ax, ay, az, gx, gy, gz);
        }

        index = buffer.indexOf('\n');
    }
}


