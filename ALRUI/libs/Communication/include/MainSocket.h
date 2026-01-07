#pragma once

#include <QTcpSocket>
#include <QMainWindow>
#include <QObject>
#include <QTcpSocket>
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>

class SendCommands;
class ReceiveCommands;

class MainSocket : public QMainWindow {
    Q_OBJECT
public:
    explicit MainSocket(QWidget *parent = nullptr);
    ~MainSocket();
    SendCommands* getSender() { return sender; }
    
signals:
    void connectionChanged(bool connected);

private:
    QTcpSocket *socket;
    SendCommands *sender;
    ReceiveCommands *receiver;
};



