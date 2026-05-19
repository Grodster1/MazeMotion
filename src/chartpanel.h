#ifndef CHARTPANEL_H
#define CHARTPANEL_H
#include <QChart>
#include <QLineSeries>
#include <QWidget>

class ChartPanel : public QWidget
{
    Q_OBJECT
    QChart *accChart;
    QChart *gyroChart;
    QLineSeries *accX, *accY, *accZ;
    QLineSeries *gyroX, *gyroY, *gyroZ;
    int sampleCount = 0;

public:
    explicit ChartPanel(QWidget *parent = nullptr);
    void retranslateUI();

public slots:
    void onSensorData(float ax, float ay, float az, float gx, float gy, float gz);

};

#endif // CHARTPANEL_H
