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
     * @param[in,out] logic - Wskaźnik do współdzielonej logiki gry
     * @param[in,out] parent - Widget rodzica
     */
    explicit GameWidget3D(GameLogic *logic, QWidget *parent = nullptr);

    /** @brief Destruktor zwalniający zasoby OpenGL (VAO, VBO, shadery). */
    ~GameWidget3D();

    /** @brief Wymusza odświeżenie kontekstu OpenGL i przerysowanie sceny. */
    void forceRefresh();

    /** @brief Odświeża teksty po zmianie języka. */
    void retranslateUI();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

public slots:
    /**
     * @brief Ustawia kąt obrotu kamery w poziomie.
     * @param[in] value - Kąt azymutu w stopniach (0--360)
     */
    void setAzimuth(int value);

    /**
     * @brief Ustawia kąt elewacji kamery.
     * @param[in] value - Kąt elewacji w stopniach (10--80)
     */
    void setElevation(int value);

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
    QOpenGLBuffer boardVBO;             ///< VBO podłogi

    QOpenGLVertexArrayObject wallsVAO;  ///< VAO ścian labiryntu
    QOpenGLBuffer wallsVBO;             ///< VBO ścian
    int wallsVertexCount = 0;           ///< Liczba wierzchołków ścian

    QOpenGLVertexArrayObject sphereVAO; ///< VAO sfery jednostkowej
    QOpenGLBuffer sphereVBO;            ///< VBO sfery
    int sphereVertexCount = 0;          ///< Liczba wierzchołków sfery

    QMatrix4x4 projectionMatrix;    ///< Macierz projekcji perspektywicznej
    QMatrix4x4 viewMatrix;          ///< Macierz widoku

    // Kamera orbitalna
    float cameraDistance = 1.5f;            ///< Odległość kamery od środka planszy
    float cameraAzimuth = -M_PI / 2.0f;     ///< Kąt obrotu kamery w poziomie [rad]
    float cameraElevation = 1.0f;           ///< Kąt elewacji kamery [rad]

    /** @brief Przelicza macierz widoku na podstawie parametrów kamery orbitalnej. */
    void updateViewMatrix();

    /** @brief Buduje geometrię podłogi (kwadrat 1x1). */
    void buildBoardGeometry();

    /** @brief Buduje geometrię ścian labiryntu jako prostopadłościany. */
    void buildWallsGeometry();

    /**
     * @brief Buduje geometrię sfery jednostkowej.
     * @param[in] stacks - Liczba podziałów wzdłuż osi Z
     * @param[in] slices - Liczba podziałów wokół osi Z
     */
    void buildSphereGeometry(int stacks, int slices);

    /** @brief Rysuje podłogę labiryntu. */
    void drawBoard();
    /** @brief Rysuje ściany labiryntu. */
    void drawWalls();
    /** @brief Rysuje kulkę jako sferę. */
    void drawBall();
    /** @brief Rysuje pulsujący cel jako zieloną sferę. */
    void drawGoal();

    int cachedWallCount = -1;   ///< Cache liczby ścian do detekcji zmian
    bool glInitialized = false; ///< Flaga inicjalizacji kontekstu OpenGL

    QLabel *winLabel = nullptr; ///< Overlay komunikatu wygranej

    bool goLeft = false;    ///< Flaga klawisza w lewo
    bool goRight = false;   ///< Flaga klawisza w prawo
    bool goUp = false;      ///< Flaga klawisza w górę
    bool goDown = false;    ///< Flaga klawisza w dół

private slots:
    /** @brief Slot reagujący na aktualizację stanu gry. */
    void onStateUpdated();
};

#endif // GAMEWIDGET3D_H