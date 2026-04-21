#include "chartpanel.h"
#include <QChartView>
#include <QVBoxLayout>
#include <QValueAxis>

ChartPanel::ChartPanel(QWidget *parent) : QWidget(parent){
    accChart = new QChart();
    gyroChart = new QChart();

    accX = new QLineSeries();
    accY = new QLineSeries();
    accZ = new QLineSeries();

    gyroX = new QLineSeries();
    gyroY = new QLineSeries();
    gyroZ = new QLineSeries();


    accChart->addSeries(accX);
    accChart->addSeries(accY);
    accChart->addSeries(accZ);

    gyroChart->addSeries(gyroX);
    gyroChart->addSeries(gyroY);
    gyroChart->addSeries(gyroZ);

    QValueAxis *accAxisY = new QValueAxis();
    accAxisY->setRange(-2.0, 2.0);
    accAxisY->setTitleText("Przyśpieszenie [g]");

    QValueAxis *accAxisX = new QValueAxis();
    accAxisX->setRange(0, 200);

    accChart->addAxis(accAxisX, Qt::AlignBottom);
    accChart->addAxis(accAxisY, Qt::AlignLeft);

    QValueAxis *gyroAxisY = new QValueAxis();
    gyroAxisY->setRange(-500.0, 500.0);
    gyroAxisY->setTitleText("Prędkość kątowa [°/s]");

    QValueAxis *gyroAxisX = new QValueAxis();
    gyroAxisX->setRange(0, 200);

    gyroChart->addAxis(gyroAxisX, Qt::AlignBottom);
    gyroChart->addAxis(gyroAxisY, Qt::AlignLeft);

    accX->attachAxis(accAxisX);
    accX->attachAxis(accAxisY);
    accY->attachAxis(accAxisX);
    accY->attachAxis(accAxisY);
    accZ->attachAxis(accAxisX);
    accZ->attachAxis(accAxisY);

    gyroX->attachAxis(gyroAxisX);
    gyroX->attachAxis(gyroAxisY);
    gyroY->attachAxis(gyroAxisX);
    gyroY->attachAxis(gyroAxisY);
    gyroZ->attachAxis(gyroAxisX);
    gyroZ->attachAxis(gyroAxisY);

    QChartView *accView = new QChartView(accChart);
    QChartView *gyroView = new QChartView(gyroChart);

    accView->setRenderHint(QPainter::Antialiasing);
    gyroView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(accView);
    layout->addWidget(gyroView);

    accX->setName("X");
    accY->setName("Y");
    accZ->setName("Z");

    gyroX->setName("X");
    gyroY->setName("Y");
    gyroZ->setName("Z");

    accChart->setTitle("Akcelerometr");
    gyroChart->setTitle("Żyroskop");
}


void ChartPanel::onSensorData(float ax, float ay, float az, float gx, float gy, float gz){
    if(sampleCount > 200){
        accX->remove(0);
        accY->remove(0);
        accZ->remove(0);
        gyroX->remove(0);
        gyroY->remove(0);
        gyroZ->remove(0);
    }

    accX->append(sampleCount, ax);
    accY->append(sampleCount, ay);
    accZ->append(sampleCount, az);

    gyroX->append(sampleCount, gx);
    gyroY->append(sampleCount, gy);
    gyroZ->append(sampleCount, gz);

    sampleCount++;
    if(sampleCount > 200){
        accChart->axes(Qt::Horizontal).first()->setRange(sampleCount-200, sampleCount);
        gyroChart->axes(Qt::Horizontal).first()->setRange(sampleCount-200, sampleCount);
    }
}
