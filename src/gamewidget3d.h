#ifndef GAMEWIDGET3D_H
#define GAMEWIDGET3D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QKeyEvent>
#include <QMatrix4x4>
#include <QVector3D>
#include <QLabel>
#include "gamelogic.h"

/**
 * @brief Widget renderujący scenę gry w 3D z wykorzystaniem OpenGL 3.3 Core Profile.
 *
 * Wykorzystuje nowoczesny potok renderowania oparty na shaderach z oświetleniem
 * Phonga, bufory wierzchołków (VBO/VAO) oraz wielopróbkowanie (MSAA 4x).
 * Logika gry (fizyka, kolizje, labirynt) delegowana jest do klasy GameLogic,
 * dzięki czemu widget odpowiada wyłącznie za wizualizację.
 */
class GameWidget3D : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    /**
     * @brief Konstruktor tworzący widget gry 3D.
     *
     * Konfiguruje format powierzchni OpenGL (3.3 Core, MSAA 4x, 24-bit depth),
     * podłącza sygnał aktualizacji stanu gry i tworzy overlay wygranej.
     *
     * @param logic Wskaźnik do współdzielonej logiki gry
     * @param parent Widget rodzica
     */
    explicit GameWidget3D(GameLogic *logic, QWidget *parent = nullptr);

    /**
     * @brief Destruktor zwalniający zasoby OpenGL (VAO, VBO, shadery).
     */
    ~GameWidget3D();

    /**
     * @brief Wymusza odświeżenie kontekstu OpenGL i przerysowanie sceny.
     *
     * Wywoływana po przełączeniu widoku z 2D na 3D, aby zaktualizować
     * viewport i macierz projekcji do aktualnego rozmiaru widgetu.
     */
    void forceRefresh();

    /**
     * @brief Odpowiada za zmianę języka winLabel.
     *
     * Wywoływana po zmianie języka w mainWindow.
     */
    void retranslateUI();

protected:
    /**
     * @brief Inicjalizuje kontekst OpenGL — shadery, geometrię i macierz widoku.
     *
     * Kompiluje i linkuje vertex/fragment shader z oświetleniem Phonga,
     * buduje geometrię podłogi, ścian i sfery jednostkowej,
     * oraz konfiguruje macierz widoku (pozycja i orientacja kamery).
     */
    void initializeGL() override;

    /**
     * @brief Aktualizuje viewport i macierz projekcji przy zmianie rozmiaru widgetu.
     * @param w Nowa szerokość widgetu w pikselach
     * @param h Nowa wysokość widgetu w pikselach
     */
    void resizeGL(int w, int h) override;

    /**
     * @brief Renderuje pełną scenę 3D — podłogę, ściany, cel i kulkę.
     *
     * Ustawia uniformy shaderów (macierze, pozycja światła i kamery),
     * rysuje wszystkie elementy sceny, a następnie zwalnia shader.
     */
    void paintGL() override;

    /**
     * @brief Obsługuje naciśnięcie klawisza strzałki.
     *
     * Ustawia przyspieszenie kulki w odpowiednim kierunku.
     * Ignoruje zdarzenia auto-repeat systemu operacyjnego.
     *
     * @param event Zdarzenie klawiatury
     */
    void keyPressEvent(QKeyEvent *event) override;

    /**
     * @brief Obsługuje zwolnienie klawisza strzałki.
     *
     * Zeruje przyspieszenie w danym kierunku.
     * Ignoruje zdarzenia auto-repeat systemu operacyjnego.
     *
     * @param event Zdarzenie klawiatury
     */
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    GameLogic *logic;   ///< Wskaźnik do współdzielonej logiki gry

    QOpenGLShaderProgram *shaderProgram = nullptr;  ///< Program shaderów (Phong lighting)

    int uModelLoc;      ///< Lokalizacja uniformu macierzy modelu
    int uViewLoc;       ///< Lokalizacja uniformu macierzy widoku
    int uProjLoc;       ///< Lokalizacja uniformu macierzy projekcji
    int uColorLoc;      ///< Lokalizacja uniformu koloru obiektu
    int uLightPosLoc;   ///< Lokalizacja uniformu pozycji światła
    int uViewPosLoc;    ///< Lokalizacja uniformu pozycji kamery

    QOpenGLVertexArrayObject boardVAO;  ///< VAO podłogi
    QOpenGLBuffer boardVBO;             ///< VBO podłogi (2 trójkąty)

    QOpenGLVertexArrayObject wallsVAO;  ///< VAO ścian labiryntu
    QOpenGLBuffer wallsVBO;             ///< VBO ścian (przebudowywane przy resetMaze)
    int wallsVertexCount = 0;           ///< Liczba wierzchołków ścian

    QOpenGLVertexArrayObject sphereVAO; ///< VAO sfery jednostkowej
    QOpenGLBuffer sphereVBO;            ///< VBO sfery (współdzielone przez kulkę i cel)
    int sphereVertexCount = 0;          ///< Liczba wierzchołków sfery

    QMatrix4x4 projectionMatrix;    ///< Macierz projekcji perspektywicznej
    QMatrix4x4 viewMatrix;          ///< Macierz widoku (pozycja i orientacja kamery)

    /**
     * @brief Buduje geometrię podłogi (kwadrat 1x1 jako dwa trójkąty).
     *
     * Wierzchołki zawierają pozycję (3 floaty) i normalną (3 floaty).
     * Normalna skierowana w górę (0, 0, 1).
     */
    void buildBoardGeometry();

    /**
     * @brief Buduje geometrię ścian labiryntu jako prostopadłościany.
     *
     * Każda ściana z listy logic->getWalls() jest renderowana jako
     * prostopadłościan o 6 ścianach, każda z poprawną normalną
     * do oświetlenia Phonga. Wywoływana ponownie przy zmianie labiryntu.
     */
    void buildWallsGeometry();

    /**
     * @brief Buduje geometrię sfery jednostkowej (promień 1.0).
     *
     * Sfera jest skalowana macierzą modelu do rozmiaru kulki lub celu.
     * Normalne wierzchołków równe znormalizowanym pozycjom (dla poprawnego oświetlenia).
     *
     * @param stacks Liczba podziałów wzdłuż osi Z (równoleżników)
     * @param slices Liczba podziałów wokół osi Z (południków)
     */
    void buildSphereGeometry(int stacks, int slices);

    /**
     * @brief Rysuje podłogę labiryntu z odwróceniem osi Y.
     */
    void drawBoard();

    /**
     * @brief Rysuje ściany labiryntu jako prostopadłościany.
     */
    void drawWalls();

    /**
     * @brief Rysuje kulkę jako sferę w pozycji odczytanej z logiki gry.
     */
    void drawBall();

    /**
     * @brief Rysuje pulsujący cel jako zieloną sferę.
     */
    void drawGoal();

    int cachedWallCount = -1;   ///< Cache liczby ścian do detekcji zmian labiryntu
    bool glInitialized = false; ///< Flaga inicjalizacji kontekstu OpenGL

    QLabel *winLabel = nullptr; ///< Overlay wyświetlający komunikat o wygranej

    bool goLeft = false;    ///< Flaga naciśnięcia klawisza w lewo
    bool goRight = false;   ///< Flaga naciśnięcia klawisza w prawo
    bool goUp = false;      ///< Flaga naciśnięcia klawisza w górę
    bool goDown = false;    ///< Flaga naciśnięcia klawisza w dół

private slots:
    /**
     * @brief Slot reagujący na aktualizację stanu gry.
     *
     * Przebudowuje geometrię ścian jeśli labirynt się zmienił,
     * aktualizuje widoczność overlay wygranej i wywołuje przerysowanie.
     */
    void onStateUpdated();
};

#endif // GAMEWIDGET3D_H