#ifndef SENSORREADER_H
#define SENSORREADER_H

#include <QObject>
#include <QSerialPort>
#include <QDebug>

/**
 * @brief Klasa odpowiedzialna za komunikację z płytką Arduino Nano 33 IoT przez port szeregowy.
 * 
 * Odbiera dane z IMU (akcelerometr i żyroskop) przesyłane w formacie CSV: ax, ay, az, gx, gy, gz.
 * Realizuje buforowanie przychodzących bajtów, arsowanie ramek danych oraz emisję sygnału dataReceived() 
 * z odczytanymi wiadomościami. 
 */

class SensorReader : public QObject
{
    Q_OBJECT
    QSerialPort *serialPort; ///<Obiekt portu szeregowego
    QByteArray buffer; ///<Bufor ramki danych
public:
    explicit SensorReader(QObject *parent = nullptr);
/**
 * @brief Otwiera port szeregowy o danej nazwie.
 * @param portName nazwa portu (np. /dev/ttyACM0)
 */
    void open(const QString &portName);

signals:
/**
 * @brief Sygnał emitowany po odebraniu i sparsowaniu pełnej ramki danych.
 * @param ax Przyspieszenie liniowe w osi X [g]
 * @param ay Przyspieszenie liniowe w osi Y [g]
 * @param az Przyspieszenie liniowe w osi Z [g]
 * @param gx Prędkość kątowa w osi X [°/s]
 * @param gy Prędkość kątowa w osi Y [°/s]
 * @param gz Prędkość kątowa w osi Z [°/s]
 */
    void dataReceived(float ax, float ay, float az, float gx, float gy, float gz);

private slots:
    void onReadyRead();
};

#endif // SENSORREADER_H
