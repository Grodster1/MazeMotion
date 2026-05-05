#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#include <QVector>
#include <QRectF>
#include <random>

/**
 * @brief Struktura reprezentująca pojedynczą komórkę labiryntu.
 *
 * Przechowuje informacje o istnieniu ścian w czterech kierunkach
 * oraz o tym, czy komórka została odwiedzona podczas generowania.
 */
struct Cell {
    bool topW = true;    ///< Ściana górna
    bool botW = true;    ///< Ściana dolna
    bool leftW = true;   ///< Ściana lewa
    bool rightW = true;  ///< Ściana prawa
    bool visited = false; ///< Flaga odwiedzenia podczas generowania (DFS)
};

/**
 * @brief Klasa odpowiedzialna za generowanie labiryntu i wyznaczanie punktu docelowego.
 *
 * Generuje labirynt metodą Recursive Backtracking (DFS), zapewniającą
 * dokładnie jedną ścieżkę między dowolnymi dwoma komórkami.
 * Udostępnia listę ścian jako znormalizowane prostokąty (0.0 -- 1.0)
 * oraz wyznacza najdalszą komórkę od startu za pomocą BFS.
 */
class MazeGenerator
{
    QVector<QVector<Cell>> grid;  ///< Siatka komórek labiryntu
    int rows;    ///< Liczba wierszy siatki
    int cols;    ///< Liczba kolumn siatki
    std::mt19937 rng;       ///< Generator liczb pseudolosowych (Mersenne Twister)
    double thick = 0.015;   ///< Grubość ścian w znormalizowanych współrzędnych

public:
    /**
     * @brief Konstruktor inicjalizujący generator liczb losowych.
     */
    MazeGenerator();

    /**
     * @brief Generuje nowy labirynt o zadanym rozmiarze.
     *
     * Tworzy siatkę komórek i uruchamia algorytm Recursive Backtracking
     * począwszy od komórki (0, 0).
     *
     * @param rows Liczba wierszy labiryntu
     * @param cols Liczba kolumn labiryntu
     */
    void generate(int rows, int cols);

    /**
     * @brief Rekurencyjnie drąży ścieżki w labiryncie (algorytm DFS).
     *
     * Odwiedza komórkę, losowo miesza kolejność kierunków,
     * a następnie dla każdego nieodwiedzonego sąsiada usuwa ścianę
     * i rekurencyjnie kontynuuje drążenie.
     *
     * @param row Wiersz aktualnej komórki
     * @param col Kolumna aktualnej komórki
     */
    void makePath(int row, int col);

    /**
     * @brief Wyznacza komórkę najdalszą od punktu startowego (0, 0).
     *
     * Wykorzystuje algorytm BFS do obliczenia odległości
     * od komórki startowej do wszystkich pozostałych komórek.
     *
     * @return Para (wiersz, kolumna) najdalszej komórki
     */
    std::pair<int, int> findFarthestCell();

    /**
     * @brief Zwraca listę ścian labiryntu jako prostokąty w znormalizowanych współrzędnych.
     *
     * Każda ściana reprezentowana jest jako QRectF o współrzędnych z zakresu 0.0 -- 1.0.
     * Dla uniknięcia duplikatów, w głównej pętli uwzględniane są tylko ściany górne
     * i lewe każdej komórki, a krawędzie dolna i prawa siatki dodawane są osobno.
     *
     * @return Wektor prostokątów reprezentujących ściany
     */
    QVector<QRectF> getWalls();

    /**
     * @brief Zwraca grubość ścian w znormalizowanych współrzędnych.
     * @return Grubość ściany
     */
    double getThickness() { return thick; };
};

#endif // MAZEGENERATOR_H