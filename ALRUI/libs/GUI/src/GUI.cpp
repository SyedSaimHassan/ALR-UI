#include "GUI.h"


// ====== Constructor and Basic Setup
GUI::GUI(QWidget *parent) : QWidget(parent) {
    resize(QGuiApplication::primaryScreen()->availableSize() * 0.8);
    setMinimumSize(1900,1000);
    // setMaximumSize(1600,900);
    setStyleSheet("color: white; font: bold 18px Arial;");
    setFocusPolicy(Qt::StrongFocus);
    
    // Load the background texture
    backgroundTexture.load("../textures/carbon_fiber.jpg");
    if (backgroundTexture.isNull()) {
        std::cerr << "Failed to load background texture" << std::endl;
    }

    // ====== CONTROL BUTTONS ======
    btnForward = new QPushButton("Forward", this);
    // btnReverse = new QPushButton("Reverse", this);
    btnLeft    = new QPushButton("Left", this);
    btnRight   = new QPushButton("Right", this);
    btnStop    = new QPushButton("Stop", this);
    btnExit    = new QPushButton("Exit", this);

    QList<QPushButton*> buttons = {btnForward, btnLeft, btnStop, btnRight};
    for (auto *b : buttons) {
        b->setFixedSize(100, 100);
        b->setStyleSheet("QPushButton { background-color: #181818; border: 2px solid white; border-radius: 8px; color: white; font: bold 18px Arial; }"
                         "QPushButton:hover { background-color: #303030; }");
    }
    btnExit->setFixedSize(50, 25);
    btnExit->setStyleSheet("QPushButton { background-color: #181818; border: 2px solid white; border-radius: 8px; color: white; font: bold 18px Arial; }"
                        "QPushButton:hover { background-color: #303030; }");    
    connect(btnExit, &QPushButton::clicked, this, &GUI::onExit);

    // ====== GEAR CONTROLS ======
    gearForwardBtn = new QPushButton("Gear Forward", this);
    gearBackwardBtn = new QPushButton("Gear Backward", this);
    gearStopBtn = new QPushButton("Stop", this);

    QList<QPushButton*> gears = {gearForwardBtn, gearBackwardBtn, gearStopBtn};
    for (auto *g : gears) {
        g->setFixedSize(180, 80);
        g->setCheckable(true);
        g->setStyleSheet("QPushButton { background-color: #181818; border: 2px solid white; border-radius: 8px; color: white; font: bold 16px Arial; }"
                         "QPushButton:checked { background-color: #00AA00; }");
    }

    connect(gearForwardBtn, &QPushButton::clicked, this, &GUI::onGearForward);
    connect(gearBackwardBtn, &QPushButton::clicked, this, &GUI::onGearBackward);
    connect(gearStopBtn, &QPushButton::clicked, this, &GUI::onGearStop);
    int w = width();
    int h = height();
    // Ensure default gear is Stop when the app starts
    onGearStop();
    // ====== CONNECTION LABEL ======
    lblConnection = new QLabel("● Disconnected", this);
    lblConnection->setStyleSheet("color: #FF0000; font: bold 30px Arial;");
    lblConnection->move(w-325,h-130);

    lblStatus = new QLabel(" Not Ready", this);
    lblStatus->setStyleSheet("color: #FF0000; font: bold 50px Arial;");
    lblStatus->move(50,225);
    // ====== BUTTON POSITIONS ======
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        update();   // triggers paintEvent() again
    });
    timer->start(33); 

    // btnExit->move(widthPx - 160, heightPx - 80);
}


// ====== Paint Event ======
void GUI::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int w = width();
    int h = height();
    int cx = w / 2;
    int cy = h / 2;

    btnForward->move(cx + 0.09375 * w, cy + 0.2222 * h);
    btnLeft->move   (cx + 0.01875 * w, cy + 0.3555 * h);
    btnRight->move  (cx + 0.16875 * w, cy + 0.3555 * h);
    btnStop->move   (cx + 0.09375 * w, cy + 0.3555 * h);
    // position gear buttons left of the graph area
    int graphX = 150;
    int graphY = 400;
    int graphH = 290;
    int gearX = graphX + 100;
    int gearY = graphY + graphH + 40;
    gearForwardBtn->move(gearX, gearY);
    gearBackwardBtn->move(gearX, gearY + 90);
    gearStopBtn->move(gearX, gearY + 180);
    drawBackground(&painter);
    drawRobotStatus(&painter);
    drawRobotTopView(&painter);
    drawRobotCameraView(&painter);
    drawGraph(&painter);
}

void GUI::drawGraph(QPainter *painter) {
    painter->setRenderHint(QPainter::Antialiasing, true);


    int W = width();
    int H = height();

    const qreal left = 0.05 * W;
    const qreal top  = 0.20 * H;
    const qreal widthR = 0.40 * W;
    const qreal heightR = 0.35 * H;
    // QRectF arena(left, top, widthR, heightR);
    QRectF arena(150, 400, 480, 270);

    painter->setBrush(QBrush(QColor("#001533")));
    painter->setPen(QPen(Qt::white, std::max<qreal>(2.0, widthR * 0.003)));
    painter->drawRect(arena);

    const int LANES = 6;
    qreal laneW = arena.width() / LANES;
    for (int i = 1; i < LANES; i++) {
        qreal x = arena.left() + i * laneW;
        painter->drawLine(QPointF(x, arena.top()), QPointF(x, arena.bottom()));
    }

    qreal qrSize = laneW * 0.32;
    qreal qrY = arena.top() + arena.height() * 0.72;

    qreal obsW = laneW * 0.55;
    qreal obsH = heightR * 0.08;
    qreal obsY = arena.top() + arena.height() * 0.25;

    int qrFont = std::max(10, int(qrSize * 0.30));
    int labelFont = std::max(10, int(widthR * 0.028));

    auto laneCenterX = [&](int idx) {
        return arena.left() + idx*laneW + laneW/2;
    };

    auto drawQR = [&](int idx, QString t) {
        qreal cx = laneCenterX(idx);
        QRectF r(cx-qrSize/2-40, qrY-qrSize/2-60, qrSize+2, qrSize+2);
        painter->setBrush(Qt::white);
        painter->setPen(Qt::black);
        painter->drawRect(r);
        painter->setFont(QFont("Arial", qrFont, QFont::Bold));
        painter->drawText(r, Qt::AlignCenter, t);
    };

    auto drawObstacle = [&](int idx) {
        qreal cx = laneCenterX(idx);
        QRectF r(cx-obsW/2-30, obsY-obsH/2+10, obsW-20, obsH-20);
        painter->setBrush(Qt::gray);
        painter->setPen(Qt::NoPen);
        painter->drawRect(r);
    };
    drawQR(1, "A\nQR");
    drawQR(2, "B\nQR");


    drawQR(5, "D\nQR");
    drawQR(4, "C\nQR");


    drawObstacle(1); 
    drawObstacle(5);

    painter->setPen(Qt::white);
    painter->setFont(QFont("Arial", labelFont, QFont::Bold));

    // QRectF pickupRect(
    //     arena.left() + laneW*0.2,
    //     arena.top() + arena.height()*0.50,
    //     laneW*3.0,
    //     22
    // );
    // painter->drawText(pickupRect, Qt::AlignCenter, "Pick up points");

    // QRectF dropRect(
    //     arena.left() + arena.width() - laneW*3.2,
    //     arena.top() + arena.height()*0.50,
    //     laneW*3.0,
    //     22
    // );
    // painter->drawText(dropRect, Qt::AlignCenter, "Drop off points");

    qreal seW = arena.width()*0.12;
    qreal seH = arena.height()*0.12;
    QRectF seRect(
        arena.left() + (arena.width()-seW)/2,
        arena.bottom()-seH+15,
        seW, seH
    );
    painter->setBrush(Qt::white);
    painter->setPen(Qt::black);
    painter->drawRect(seRect);
    painter->setFont(QFont("Arial", std::max(10, int(seH*0.35)), QFont::Bold));
    painter->drawText(seRect, Qt::AlignCenter, "S   /   E");

    //Draw Robot

    painter->setBrush(Qt::blue);
    painter->setPen(Qt::black);
    float posX = robotXPos/4+150-10;
    float posY = robotYPos/4+400-10;
    QRectF circleRect(posX,posY,20,20);
    painter->drawEllipse(circleRect);
    
    float robAngle = qDegreesToRadians(robotAngle);

    float centerX = posX + 10;
    float centerY = posY + 10;
    float lineLength = 10;

    painter->setPen(QPen(Qt::red, 2));

    float endX = centerX + lineLength * std::cos(robAngle);
    float endY = centerY - lineLength * std::sin(robAngle);

    painter->drawLine(QPointF(centerX, centerY),
                    QPointF(endX, endY));

}

void GUI::drawRobotPosition(QPainter *painter){

}

QByteArray GUI::processCommands(){
    // Build commands for this frame. Clear previous contents first to avoid accumulation.
    cmds.clear();
    if (movingForward) {
        cmds.append("W\n");
    }
    if (movingLeft) {
        cmds.append("A\n");
    }
    if (movingRight) {
        cmds.append("D\n");
    }
    if (movingBackward) {
        cmds.append("S\n");
    }
    if (stopped){
        cmds.clear();
        cmds.append("Break\n");
    }
    return cmds;
}

void GUI::drawBackground(QPainter *painter) {
    if (!backgroundTexture.isNull()) {
        // Scale the texture to fit the window while maintaining aspect ratio
        QRect backgroundRect = rect();
        QPixmap scaled = backgroundTexture.scaled(backgroundRect.size(),
                                                Qt::KeepAspectRatioByExpanding,
                                                Qt::SmoothTransformation);
        
        // If the scaled image is larger than the window, center it
        if (scaled.width() > width() || scaled.height() > height()) {
            int x = (scaled.width() - width()) / 2;
            int y = (scaled.height() - height()) / 2;
            painter->drawPixmap(0, 0, scaled.copy(x, y, width(), height()));
        } else {
            painter->drawPixmap(backgroundRect, scaled);
        }
    } else {
        // Fallback to solid color if texture loading failed
        painter->fillRect(rect(), QColor("#101010"));
    }}
void GUI::drawRobotStatus(QPainter *painter) {
    int w = width();
    int h = height();

    // 15px margin from bottom-right (relative to screen size)
    int margin = 0.01 * w;  // e.g. ~15px on 1365px wide window
    int rectW = 0.22 * w;
    int rectH = 0.25 * h;

    QRect statusRect(w - rectW - margin, h - rectH - margin, rectW, rectH);
    painter->setPen(QPen(Qt::gray, 2));
    painter->setBrush(QBrush(QColor("#1E1E1E")));
    painter->drawRoundedRect(statusRect, 15, 15);

    QFont titleFont("Arial", 28, QFont::Bold);
    painter->setFont(titleFont);
    painter->drawText(statusRect.adjusted(30, 20, 0, 0), "Robot Status");
    

    // painter->setBrush(QBrush(QColor("#00FF66")));
    // painter->setPen(Qt::NoPen);
    // painter->drawEllipse(QPoint(300, 350), 50, 50);
}

void GUI::drawRobotCameraView(QPainter *painter){
    int w = width();
    int h = height();

    QRect camRect(0.4 * w, 0.03 * h, 0.55 * w, 0.65 * h);
    painter->setPen(QPen(Qt::gray, 2));
    painter->setBrush(QBrush(QColor("#1E1E1E")));
    painter->drawRect(camRect);
}

void GUI::drawRobotTopView(QPainter *painter) {
    int w = width();
    int h = height();

    // === Robot body ===
    QRect body(400, 150, 0.125 * w, 0.22 * h);
    painter->setPen(QPen(Qt::gray, 2));
    painter->setBrush(QBrush(QColor("#24333F"))); 
    painter->drawRoundedRect(body, 15, 15);

    int wheelW = 0.025 * w;  
    int wheelH = 0.075 * h; 
    int gap = 0.008 * w;     

    painter->setBrush(QBrush(QColor("#505050")));  

    int wheelTopY = body.top() + 0.02 * h;
    int wheelBottomY = body.bottom() - wheelH - 0.02 * h;

    painter->drawRoundedRect(body.left() - gap - wheelW, wheelTopY, wheelW, wheelH, 5, 5);
    painter->drawRoundedRect(body.left() - gap - wheelW, wheelBottomY, wheelW, wheelH, 5, 5);

    painter->drawRoundedRect(body.right() + gap, wheelTopY, wheelW, wheelH, 5, 5);
    painter->drawRoundedRect(body.right() + gap, wheelBottomY, wheelW, wheelH, 5, 5);

    painter->setBrush(QBrush(QColor("#00BFFF"))); 
    int indicatorW = 0.04 * w;
    int indicatorH = 0.015 * h;
    painter->drawRect(body.center().x() - indicatorW / 2, body.top() - indicatorH - 3, indicatorW, indicatorH);
}

// ====== Keyboard Input ======
void GUI::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    
    const QString activeStyle = "QPushButton { background-color: #303030; border: 2px solid white; border-radius: 8px; color: white; font: bold 18px Arial; }";
    
    switch (event->key()) {
        case Qt::Key_W:
            keyWHeld = true;
            if (gear == BackwardGear) {
                movingBackward = true;
                btnStop->setStyleSheet(activeStyle);
            } else if (gear == ForwardGear) {
                movingForward = true;
                btnForward->setStyleSheet(activeStyle);
            }
            break;
        case Qt::Key_A:
            movingLeft = true;
            btnLeft->setStyleSheet(activeStyle);
            break;
        case Qt::Key_S:
            // mark physical key held
            keySHeld = true;
            // Allow backward only in Backward gear. In Forward or Stop gear S does nothing.
            if (gear == BackwardGear) {
                movingBackward = true;
                btnStop->setStyleSheet(activeStyle);
            }
            break;
        case Qt::Key_D:
            movingRight = true;
            btnRight->setStyleSheet(activeStyle);
            break;
        case Qt::Key_Down:
            // Cycle gears upward: Stop -> Forward -> Backward -> Stop
            if (gear == StopGear) {
                onGearForward();
            } else if (gear == ForwardGear) {
                onGearBackward();
            } else if (gear == BackwardGear) {
                onGearStop();
            }
            break;
        case Qt::Key_Up:
            // Cycle gears downward: Stop -> Backward -> Forward -> Stop
            if (gear == StopGear) {
                onGearBackward();
            } else if (gear == BackwardGear) {
                onGearForward();
            } else if (gear == ForwardGear) {
                onGearStop();
            }
            break;
    }
}

void GUI::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    
    const QString normalStyle = "QPushButton { background-color: #181818; border: 2px solid white; border-radius: 8px; color: white; font: bold 18px Arial; }"
                              "QPushButton:hover { background-color: #303030; }";
    
    switch (event->key()) {
        case Qt::Key_W:
            // physical key released
            keyWHeld = false;
            if (gear == BackwardGear) {
                // in backward gear, if neither W nor S are held, stop movingBackward
                if (!keyWHeld && !keySHeld) {
                    movingBackward = false;
                    btnStop->setStyleSheet(normalStyle);
                }
            } else if (gear == ForwardGear) {
                movingForward = false;
                btnForward->setStyleSheet(normalStyle);
            }
            break;
        case Qt::Key_A:
            movingLeft = false;
            btnLeft->setStyleSheet(normalStyle);
            break;
        case Qt::Key_S:
            // physical key released
            keySHeld = false;
            if (gear == BackwardGear) {
                if (!keyWHeld && !keySHeld) {
                    movingBackward = false;
                    btnStop->setStyleSheet(normalStyle);
                }
            }
            // In Forward/Stop gear S had no effect
            break;
        case Qt::Key_D:
            movingRight = false;
            btnRight->setStyleSheet(normalStyle);
            break;
    }
}


// Exit ====== 
void GUI::onExit() {
    qApp->quit();
}

void GUI::setConnected(bool connected) {
    if (connected) {
        lblConnection->setText("● Connected");
        lblConnection->setStyleSheet("color: #00FF00; font: bold 30px Arial;");
        lblStatus->setText("      Ready");
        lblStatus->setStyleSheet("color: #00FF00; font: bold 50px Arial;");
    } else {
        lblConnection->setText("● Disconnected");
        lblConnection->setStyleSheet("color: #FF0000; font: bold 30px Arial;");
        lblStatus->setText(" Not Ready");
        lblStatus->setStyleSheet("color: #FF0000; font: bold 50px Arial;");
    }
}

// ====== Gear button handlers ======
void GUI::onGearForward() {
    gear = ForwardGear;
    gearForwardBtn->setChecked(true);
    gearBackwardBtn->setChecked(false);
    gearStopBtn->setChecked(false);

    // In Forward gear we only place restrictions: disallow backward.
    stopped = false;
    movingBackward = false;
    // If W is physically held, enable forward movement now; otherwise leave movement off.
    if (keyWHeld) {
        movingForward = true;
        // reflect button pressed visual
        btnForward->setStyleSheet("QPushButton { background-color: #303030; border: 2px solid white; border-radius: 8px; color: white; font: bold 18px Arial; }");
    } else {
        movingForward = false;
        btnForward->setStyleSheet("QPushButton { background-color: #181818; border: 2px solid white; border-radius: 8px; color: white; font: bold 18px Arial; }QPushButton:hover { background-color: #303030; }");
    }
}

void GUI::onGearBackward() {
    gear = BackwardGear;
    gearForwardBtn->setChecked(false);
    gearBackwardBtn->setChecked(true);
    gearStopBtn->setChecked(false);

    // In Backward gear only restrictions: disallow forward.
    stopped = false;
    movingForward = false;
    // If either W or S are currently held, treat them as backward inputs.
    if (keyWHeld || keySHeld) {
        movingBackward = true;
        btnStop->setStyleSheet("QPushButton { background-color: #303030; border: 2px solid white; border-radius: 8px; color: white; font: bold 18px Arial; }");
    } else {
        movingBackward = false;
        btnStop->setStyleSheet("QPushButton { background-color: #181818; border: 2px solid white; border-radius: 8px; color: white; font: bold 18px Arial; }QPushButton:hover { background-color: #303030; }");
    }
}

void GUI::onGearStop() {
    gear = StopGear;
    gearForwardBtn->setChecked(false);
    gearBackwardBtn->setChecked(false);
    gearStopBtn->setChecked(true);

    // stop all movement immediately. Do NOT clear keyWHeld/keySHeld so held keys are still
    // recognized if user later selects a gear (they'll then take effect).
    movingForward = movingBackward = movingLeft = movingRight = false;
    stopped = true;
}
