#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamelogic.h"
#include "gamewidget.h"
#include "gamewidget3d.h"
#include "sensorreader.h"
#include "chartpanel.h"
#include <QComboBox>
#include <QPushButton>
#include <QToolBar>
#include <QSplitter>
#include <QStackedWidget>
#include <QTranslator>
#include <QSlider>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Główne okno aplikacji MazeMotion.
 *
 * Zarządza układem interfejsu: panelem gry (2D/3D), panelem wykresów
 * oraz paskiem narzędzi. Obsługuje połączenie z Arduino, przełączanie
 * trybu wizualizacji, zmianę języka i sterowanie kamerą 3D.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

    GameLogic *gameLogic;           ///< Logika gry współdzielona przez widgety 2D i 3D
    GameWidget *gameWidget;         ///< Widget gry 2D
    SensorReader *sensorReader;     ///< Odbiór danych z Arduino
    ChartPanel *chartPanel;         ///< Panel wykresów
    QComboBox *portCombo;           ///< Lista portów szeregowych
    QPushButton *connectButton;     ///< Przycisk połączenia z Arduino
    QPushButton *newMazeButton;     ///< Przycisk generowania labiryntu
    QPushButton *langButton;        ///< Przycisk zmiany języka
    GameWidget3D *gameWidget3D;     ///< Widget gry 3D (OpenGL)
    QPushButton *switchViewButton;  ///< Przycisk przełączania 2D/3D
    QSplitter *splitter;            ///< Splitter gra/wykresy
    QStackedWidget *gameStack;      ///< Stos widgetów 2D/3D
    QTranslator *translator = nullptr; ///< Translator PL/EN
    QSlider *azimuthSlider;         ///< Slider obrotu kamery 3D
    QSlider *elevationSlider;       ///< Slider elewacji kamery 3D
    QLabel *azLabel;                ///< Etykieta slidera obrotu
    QLabel *elLabel;                ///< Etykieta slidera elewacji
    bool isPolish = true;           ///< Flaga aktualnego języka
    bool connected = false;         ///< Flaga połączenia z Arduino

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    /** @brief Odświeża teksty interfejsu po zmianie języka. */
    void retranslateUI();

public slots:
    /** @brief Obsługuje połączenie/rozłączenie z Arduino. */
    void onConnectClicked();
    /** @brief Przełącza widok gry między 2D a 3D. */
    void onSwitchView();
    /** @brief Przełącza język interfejsu PL/EN. */
    void onSwitchLanguage();
};

#endif // MAINWINDOW_H