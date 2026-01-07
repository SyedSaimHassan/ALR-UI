#pragma once

#include <QObject>

class QTcpSocket;

class SendCommands : public QObject {
    Q_OBJECT

public:
    explicit SendCommands(QTcpSocket *socket, QObject *parent = nullptr);
    void sendMoveCommand(int speed, QByteArray cmds);

private:
    QTcpSocket *m_socket;
};
