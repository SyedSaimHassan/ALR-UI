#pragma once

#include <iostream>
#include <cmath>

#include <QWidget>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QLabel>
#include <QApplication>
#include <QKeyEvent>
#include <QFont>
#include <QScreen>
#include <QPixmap>
#include <QtMath>

class GUI : public QWidget {
    Q_OBJECT
public:
    explicit GUI(QWidget *parent = nullptr);
    QByteArray processCommands();
    QByteArray cmds;
    float robotXPos = 960;
    float robotYPos = 1080;
    float robotAngle = 90;
public slots:
    void setConnected(bool connected);

private slots:
    void onExit();
    void onGearForward();
    void onGearBackward();
    void onGearStop();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void drawBackground(QPainter *painter);
    void drawRobotStatus(QPainter *painter);
    void drawRobotTopView(QPainter *painter);
    void drawRobotCameraView(QPainter *painter);
    void drawGraph(QPainter *painter);
    void drawRobotPosition(QPainter *painter);
    // ====== Button Click Input ======
    QPushButton *btnForward;
    QPushButton *btnReverse;
    QPushButton *btnLeft;
    QPushButton *btnRight;
    QPushButton *btnStop;
    QPushButton *btnExit;

    // ====== Labels for connection and label ======
    QLabel *lblConnection;
    QLabel *lblStatus;

    int widthPx = 1920;
    int heightPx = 1080;


    // ====== Key Held and pressed ======
    bool movingForward = false;
    bool movingBackward = false;
    bool movingLeft = false;
    bool movingRight = false;
    bool stopped = true;
    // track physical W/S key holds so releasing one while the other is held works correctly
    bool keyWHeld = false;
    bool keySHeld = false;

    // ====== Gear Controls ======
    QPushButton *gearForwardBtn;
    QPushButton *gearBackwardBtn;
    QPushButton *gearStopBtn;

    enum GearState { Neutral, ForwardGear, BackwardGear, StopGear } gear = StopGear;

    // ====== Background Texture ======
    QPixmap backgroundTexture;
};
