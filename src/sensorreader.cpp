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

void SensorReader::close(){
    serialPort->close();
}

void SensorReader::onReadyRead(){
    buffer += serialPort->readAll();

    float ax, ay, az, gx, gy, gz;
    bool hasData = false;

    int index = buffer.indexOf('\n');
    while(index != -1){
        QByteArray byteLine = buffer.left(index);
        buffer.remove(0, index + 1);
        QString line = QString::fromUtf8(byteLine).trimmed();

        int starIndex = line.indexOf('*');
        if(starIndex == -1){
            index = buffer.indexOf('\n');
            continue;
        }

        QString data = line.left(starIndex);
        QString receivedChecksum = line.mid(starIndex + 1);

        uint8_t checksum = 0;
        for(int i = 0; i < data.length(); i++){
            checksum ^= data.at(i).toLatin1();
        }
        QString calculatedChecksum = QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();

        if(calculatedChecksum != receivedChecksum.toUpper()){
            qDebug() << "Błędna suma kontrolna:" << calculatedChecksum << "!=" << receivedChecksum;
            index = buffer.indexOf('\n');
            continue;
        }

        QStringList list = data.split(',');
        if(list.size() == 6){
            ax = list[0].toFloat();
            ay = list[1].toFloat();
            az = list[2].toFloat();
            gx = list[3].toFloat();
            gy = list[4].toFloat();
            gz = list[5].toFloat();
            hasData = true;
        }

        index = buffer.indexOf('\n');
    }

    if(hasData){
        emit dataReceived(ax, ay, az, gx, gy, gz);
    }
}




