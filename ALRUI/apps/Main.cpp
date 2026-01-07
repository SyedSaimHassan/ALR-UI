#include "GUI.h"
#include "MainSocket.h"
#include "SendCommands.h"
#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    GUI Window;
    MainSocket socket; 
    // Connect MainSocket connection signal to GUI so it can update status labels
    QObject::connect(&socket, &MainSocket::connectionChanged, &Window, &GUI::setConnected);

    QTimer *sendTimer = new QTimer(&app);
    sendTimer->setInterval(333);
    QObject::connect(sendTimer, &QTimer::timeout, [&socket, &Window]() {
        QByteArray cmd = Window.processCommands();
        if (!cmd.isEmpty() && socket.getSender()) {
            socket.getSender()->sendMoveCommand(0, cmd);
        }
    });
    sendTimer->start();
    
    Window.show();
    return app.exec();
}
  