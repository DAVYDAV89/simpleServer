#include <QCoreApplication>
#include <QDebug>
#include "Server.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
    if (!server.start(3333)) {
        qDebug() << "Failed to start server.";
        return 1;
    }

    qDebug() << "Server started on port 3333";

    return a.exec();
}
