#ifndef SERVER_H
#define SERVER_H

#include <QtNetwork>
#include <QObject>


class Server : public QObject {
    Q_OBJECT

    QTcpServer *tcpServer;

    typedef size_t block_number;
    typedef std::unordered_map<std::string, std::pair<block_number, size_t>> BlockMap;

    BlockMap block_map;

public:
    explicit Server(QObject *parent = nullptr);
    bool start(int port);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    size_t get_block_number(const std::string& hash);
    size_t get_block_size(const std::string& hash);
    int get_block_data(size_t block_num, char *buffer, size_t buffer_size);
};

#endif // SERVER_H
