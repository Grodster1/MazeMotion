#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamelogic.h"
#include "gamewidget.h"
#include "sensorreader.h"
#include "chartpanel.h"
#include <QComboBox>
#include <QPushButton>
#include <QToolBar>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    GameLogic *gameLogic;
    GameWidget *gameWidget;
    SensorReader *sensorReader;
    ChartPanel *chartPanel;
    QComboBox *portCombo;
    QPushButton *connectButton;
    QPushButton *newMazeButton;
    bool connected = false;


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow *ui;

public slots:
    void onConnectClicked();
};
#endif // MAINWINDOW_H