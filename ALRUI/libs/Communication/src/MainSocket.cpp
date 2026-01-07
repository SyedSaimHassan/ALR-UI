#include "MainSocket.h"
#include "ReceiveCommands.h"
#include "SendCommands.h"
#include <QDebug>

MainSocket::MainSocket(QWidget *parent)
    : QMainWindow(parent),
      socket(new QTcpSocket(this)),
      sender(new SendCommands(socket, this)),
      receiver(new ReceiveCommands(socket, this))
{
    socket->connectToHost("192.168.4.1", 8888);

    connect(socket, &QTcpSocket::connected, this, [this](){
        qDebug() << "Connected to ESP32 successfully!";
        emit connectionChanged(true);
    });

    connect(socket, &QTcpSocket::disconnected, this, [this](){
        qDebug() << "Socket disconnected";
        emit connectionChanged(false);
    });

    connect(socket, &QTcpSocket::errorOccurred,
            this, [this](QAbstractSocket::SocketError error){
        qWarning() << "Socket Error:" << error;
        emit connectionChanged(false);
    });

    connect(socket, &QTcpSocket::readyRead,
            receiver, &ReceiveCommands::processIncomingData);
}

MainSocket::~MainSocket() {
    if (socket->isOpen())
        socket->disconnectFromHost();
}
