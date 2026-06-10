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

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Główne okno aplikacji MazeMotion.
 *
 * Zarządza układem interfejsu: panelem gry (2D/3D), panelem wykresów
 * oraz paskiem narzędzi. Obsługuje połączenie z Arduino przez port
 * szeregowy, przełączanie trybu wizualizacji i zmianę języka interfejsu.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

    GameLogic *gameLogic;           ///< Logika gry współdzielona przez widgety 2D i 3D
    GameWidget *gameWidget;         ///< Widget renderujący grę w trybie 2D
    SensorReader *sensorReader;     ///< Klasa odbierająca dane z Arduino przez port szeregowy
    ChartPanel *chartPanel;         ///< Panel z wykresami danych sensorycznych
    QComboBox *portCombo;           ///< Lista rozwijana dostępnych portów szeregowych
    QPushButton *connectButton;     ///< Przycisk nawiązania/rozłączenia połączenia z Arduino
    QPushButton *newMazeButton;     ///< Przycisk generowania nowego labiryntu
    QPushButton *langButton;        ///< Przycisk przełączania języka interfejsu (PL/EN)
    GameWidget3D *gameWidget3D;     ///< Widget renderujący grę w trybie 3D (OpenGL)
    QPushButton *switchViewButton;  ///< Przycisk przełączania między trybem 2D a 3D
    QSplitter *splitter;            ///< Splitter dzielący okno na panel gry i panel wykresów
    QStackedWidget *gameStack;      ///< Stos widgetów przełączający między GameWidget a GameWidget3D
    QTranslator *translator = nullptr; ///< Translator odpowiedzialny za tłumaczenie interfejsu na angielski
    bool isPolish = true;           ///< Flaga aktualnego języka interfejsu
    bool connected = false;         ///< Flaga stanu połączenia z Arduino

public:
    /**
     * @brief Konstruktor tworzący główne okno aplikacji.
     *
     * Inicjalizuje logikę gry, widgety 2D/3D, panel wykresów, pasek narzędzi,
     * podłącza sygnały komunikacji z Arduino oraz sterowania interfejsem.
     *
     * @param parent - Widget rodzica
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destruktor głównego okna.
     */
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

    /**
     * @brief Odświeża teksty interfejsu po zmianie języka.
     *
     * Aktualizuje napisy na przyciskach, etykietach oraz deleguje
     * odświeżenie tytułów wykresów do ChartPanel::retranslateUi().
     */
    void retranslateUI();

public slots:
    /**
     * @brief Slot obsługujący nawiązanie lub rozłączenie połączenia z Arduino.
     *
     * Otwiera port szeregowy wybrany w liście rozwijanej lub zamyka
     * aktywne połączenie. Aktualizuje tekst przycisku.
     */
    void onConnectClicked();

    /**
     * @brief Slot przełączający widok gry między trybem 2D a 3D.
     *
     * Zmienia aktywny widget w QStackedWidget i aktualizuje tekst przycisku.
     * Stan gry (pozycja kulki, labirynt) jest zachowany.
     */
    void onSwitchView();

    /**
     * @brief Slot przełączający język interfejsu między polskim a angielskim.
     *
     * Instaluje lub usuwa translator Qt i wywołuje retranslateUI()
     * w celu odświeżenia wszystkich widocznych tekstów.
     */
    void onSwitchLanguage();
};

#endif // MAINWINDOW_H