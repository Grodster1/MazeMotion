#ifndef CHARTPANEL_H
#define CHARTPANEL_H

#include <QChart>
#include <QLineSeries>
#include <QWidget>

/**
 * @brief Panel z wykresami czasowymi danych sensorycznych.
 *
 * Wyświetla dwa wykresy aktualizowane w czasie rzeczywistym:
 * przyspieszenie liniowe (akcelerometr, 3 osie) oraz prędkość
 * kątową (żyroskop, 3 osie). Okno obserwacji obejmuje ostatnich
 * 200 próbek i przesuwa się automatycznie. Zrealizowany z wykorzystaniem
 * modułu Qt Charts.
 */
class ChartPanel : public QWidget
{
    Q_OBJECT

    QChart *accChart;    ///< Wykres danych z akcelerometru
    QChart *gyroChart;   ///< Wykres danych z żyroskopu

    QLineSeries *accX;   ///< Seria danych akcelerometru — oś X
    QLineSeries *accY;   ///< Seria danych akcelerometru — oś Y
    QLineSeries *accZ;   ///< Seria danych akcelerometru — oś Z

    QLineSeries *gyroX;  ///< Seria danych żyroskopu — oś X
    QLineSeries *gyroY;  ///< Seria danych żyroskopu — oś Y
    QLineSeries *gyroZ;  ///< Seria danych żyroskopu — oś Z

    int sampleCount = 0; ///< Licznik próbek (oś X wykresów)

public:
    /**
     * @brief Konstruktor tworzący panel wykresów.
     *
     * Inicjalizuje dwa wykresy (akcelerometr i żyroskop) z osiami,
     * seriami danych i widokami. Układ pionowy (QVBoxLayout).
     *
     * @param parent - Widget rodzica
     */
    explicit ChartPanel(QWidget *parent = nullptr);

    /**
     * @brief Odświeża teksty wykresów po zmianie języka interfejsu.
     *
     * Aktualizuje tytuły wykresów oraz opisy osi pionowych.
     */
    void retranslateUI();

public slots:
    /**
     * @brief Slot odbierający dane z czujnika IMU i aktualizujący wykresy.
     *
     * Dopisuje nowe punkty do serii, usuwa najstarsze przy przekroczeniu
     * 200 próbek i przesuwa okno obserwacji osi X.
     *
     * @param ax - Przyspieszenie liniowe w osi X [g]
     * @param ay - Przyspieszenie liniowe w osi Y [g]
     * @param az - Przyspieszenie liniowe w osi Z [g]
     * @param gx - Prędkość kątowa w osi X [°/s]
     * @param gy - Prędkość kątowa w osi Y [°/s]
     * @param gz - Prędkość kątowa w osi Z [°/s]
     */
    void onSensorData(float ax, float ay, float az, float gx, float gy, float gz);
};

#endif // CHARTPANEL_H