#include "ReceiveCommands.h"
#include <QTcpSocket>
#include <QDebug>

ReceiveCommands::ReceiveCommands(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
}

void ReceiveCommands::processIncomingData() {
    if (!m_socket)
        return;

    while (m_socket->bytesAvailable()) {
        QByteArray data = m_socket->readAll();
        qDebug() << "Received data:" << data;
    }
}
