#include "chartpanel.h"
#include <QChartView>
#include <QVBoxLayout>

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
    accChart->createDefaultAxes();

    gyroChart->addSeries(gyroX);
    gyroChart->addSeries(gyroY);
    gyroChart->addSeries(gyroZ);
    gyroChart->createDefaultAxes();

    QChartView *accView = new QChartView(accChart);
    QChartView *gyroView = new QChartView(gyroChart);

    accView->setRenderHint(QPainter::Antialiasing);
    gyroView->setRenderHint(QPainter::Antialiasing);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(accView);
    layout->addWidget(gyroView);
}


void ChartPanel::onSensorData(float ax, float ay, float az, float gx, float gy, float gz){
    accX->append(sampleCount, ax);
    accY->append(sampleCount, ay);
    accZ->append(sampleCount, az);

    gyroX->append(sampleCount, gx);
    gyroY->append(sampleCount, gy);
    gyroZ->append(sampleCount, gz);

    sampleCount++;
}
