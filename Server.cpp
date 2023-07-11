#include "Server.h"


Server::Server(QObject *parent) : QObject(parent) {
    tcpServer = new QTcpServer(this);

    connect(tcpServer, &QTcpServer::newConnection, this, &Server::onNewConnection);
}

bool Server::start(int port) {
    return tcpServer->listen(QHostAddress::Any, port);
}

void Server::onNewConnection() {
//    qDebug() << __PRETTY_FUNCTION__;

    QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &Server::onReadyRead);
}

void Server::onReadyRead() {
//    qDebug() << __PRETTY_FUNCTION__;

    QTcpSocket *clientSocket = qobject_cast<QTcpSocket *>(sender());
    if (clientSocket) {
        QByteArray requestData = clientSocket->readAll();
        QList<QByteArray> hashes = requestData.split('\n');
        hashes.removeLast();

        QByteArray responseData;
        QDataStream responseStream(&responseData, QIODevice::WriteOnly);

        for (const QByteArray &hash : hashes) {
            size_t blockNum = get_block_number(hash.toStdString());
            size_t blockSize = get_block_size(hash.toStdString());

            // Создаем буфер нужного размера
            QByteArray buffer(blockSize, '\0');

            // Читаем данные блока из блочного устройства
            int bytesRead = get_block_data(blockNum, buffer.data(), blockSize);

            if (bytesRead > 0) {
                // Отправляем хэш и размер блока
                responseStream << hash << static_cast<quint32>(bytesRead);

                // Отправляем данные блока
                responseStream.writeRawData(buffer.constData(), bytesRead);
            }
        }

        // Отправляем ответ клиенту
        clientSocket->write(responseData);
        clientSocket->disconnectFromHost();
    }
}

size_t Server::get_block_number(const std::string &hash) {

    auto it = block_map.find(hash);

    if (it != block_map.end()) {
        return it->second.first;
    } else {
        return static_cast<block_number>(-1);
    }
}

size_t Server::get_block_size(const std::string &hash) {
    auto it = block_map.find(hash);

    if (it != block_map.end()) {
        return it->second.second;
    } else {
        return 0; // или другое значение, указывающее на ошибку/несуществующий блок
    }
}

int Server::get_block_data(size_t block_num, char *buffer, size_t buffer_size) {
    std::string data = "Data of block " + std::to_string(block_num);
    if (buffer_size >= data.size()) {
        std::copy(data.begin(), data.end(), buffer);
        return 0; // Успешное чтение
    } else {
        return -1; // Ошибка чтения
    }
}
