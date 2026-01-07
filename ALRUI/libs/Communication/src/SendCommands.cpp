#include "SendCommands.h"
// #include "GUI.h"
#include <QTcpSocket>
#include <QDebug>
#include <iostream>

SendCommands::SendCommands(QTcpSocket *socket, QObject *parent)
    : QObject(parent), m_socket(socket)
{
}

void SendCommands::sendMoveCommand(int speed, QByteArray cmds)
{
    if (!m_socket || !m_socket->isOpen()) {
        qWarning() << "Socket not connected!";
        return;
    }
    
    m_socket->write(cmds);
    m_socket->flush();
    std::cout << "Sent Command" << std::endl;
    // qDebug() << "Sent command:" << data;
}
