#ifndef TESTSERV_H
#define TESTSERV_H

#include <QObject>
#include <QtNetwork>
#include <QTcpSocket>
#include <QDebug>

class TestServ : public QObject
{
    Q_OBJECT
private:
    quint16 port = 7676;
    QString host;//имя сервера

    QTcpSocket *socket; //сокет
    QTcpServer *server; // указатель на сервер
    QTimer * timer_send;

    QByteArray compressData(const QVariant &var);
    QVariant uncompressData(const QByteArray &data);
public:
    explicit TestServ(QObject *parent = nullptr);

signals:
    void existMessage(QString m);

public slots:
    void incommingConnection(); // обработчик входящего подключения
    QAbstractSocket::SocketState stateChanged(QAbstractSocket::SocketState stat); // обработчик изменения состояния вещающего
    QAbstractSocket::SocketError onError(QAbstractSocket::SocketError socketError);
    void socketReady();
    void socketDisconnected();
    void sendMessage(QString m);
};

#endif // TESTSERV_H
