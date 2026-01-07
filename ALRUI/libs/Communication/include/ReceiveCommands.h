#pragma once
#include <QObject>
#include <QTcpSocket>

class ReceiveCommands : public QObject {
    Q_OBJECT

public:
    explicit ReceiveCommands(QTcpSocket *socket, QObject *parent = nullptr);

public slots:     
    void processIncomingData();

private:
    QTcpSocket *m_socket;
};

