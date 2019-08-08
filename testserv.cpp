#include "testserv.h"

TestServ::TestServ(QObject *parent) : QObject(parent)
{
    server = new QTcpServer(this);
    QString log = " server listen port = " + QString::number(port);
    server->listen(QHostAddress::Any, port);
    qDebug() << log;
    connect(server, SIGNAL(newConnection()), this, SLOT(incommingConnection())); // подключаем сигнал "новое подключение" к нашему обработчику подключений
    connect(server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    qDebug()<<"start 2 conectted";

    //timer_send = new QTimer();
    //connect(this,SIGNAL(existMessage(QString)),this,SLOT(sendMessage(QString)));
    //connect(timer_send,SIGNAL(timeout()),this,SLOT(sendMessage()));
    //emit existMessage("Test message");
}

//////////////////////////////////////////////////////////////////////////////
//Для сжатие передаваемы данных(вряд ли пригодится)
QByteArray TestServ::compressData(const QVariant &var)
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    stream << var;
    return qCompress(data, 4);
}
QVariant TestServ::uncompressData(const QByteArray &data)
{
    QByteArray raw_data = qUncompress(data);
    if(raw_data.isEmpty())
        return QVariant();
    QDataStream stream(raw_data);
    stream.setVersion(QDataStream::Qt_DefaultCompiledVersion);
    QVariant var;
    stream >> var;
    return var;
}
////////////////////////////////////////////////////////////////////////////////
void TestServ::sendMessage(QString test)
{
    //QString test = "POS=x0,y:0,z:5;";

    /*QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

    QVariant tmp_data = test.toUtf8();

    stream << (tmp_data);
    //qDebug() << "SERVER SEND POS" << tmp_data.toString();
    if(true)//socket->isOpen() && socket->isWritable())
    {
        socket->write(data);
        socket->flush();
        qDebug() << "SERVER SEND POS" << data;
        //socket->flush();
    }*/
    QTextStream stream_str(socket);
    stream_str << test.toUtf8();

    socket->flush(); // !Чтоб разными пакетами отправлять
}
void TestServ::socketReady()
{
    /*qDebug() << "ready";
    QDataStream stream(socket);
    stream.setVersion(QDataStream::Qt_DefaultCompiledVersion);

    while(socket->bytesAvailable()) {
        stream.startTransaction();

        QByteArray data;
        stream >> data;


        // Если не докачалось, то ждём новую порцию данных.
        if(data.isEmpty() == true) {
            // QByteArray подвержен сериализации также,
            // как и другие контейнеры со вставкой размера
            // перед началом данных, поэтому если по факту
            // данных будет меньше, нежели чем было записано,
            // то буфер окажется пустой.
            stream.rollbackTransaction();
            return;
        }

        // Целостность (соответствие размеру) проверили,
        // теперь пытаемся проверить корректность.

        QVariant var = uncompressData(data);
        // Порядок.
        if(var.isValid() == true) {
            QString commandClient = var.toString();
            qDebug()<<"var: "<< commandClient;;
            commandClient = commandClient.split(";").at(0);
            int pos = 0;
            QRegExp rx("((\\d{1,3}\\.\\d{1,3})|(\\d{1,3}))");
            QStringList line;
            while ((pos = rx.indexIn(commandClient, pos)) != -1) {
                line.append(rx.cap(1));
                pos += rx.matchedLength();
            }
            //qDebug() << line;*/
    if(socket->waitForConnected(100000))
    {
            socket->waitForReadyRead(1000);
            QByteArray data = socket->readAll();
            qDebug() << data; //Gamepad command from Client
            socket->flush();
            QString commandClient = data;
            commandClient = commandClient.split(";").at(0);

            int pos = 0;
            QRegExp rx("((\\d{1,3}\\.\\d{1,3})|(\\d{1,3}))");
            QStringList line;
            while ((pos = rx.indexIn(commandClient, pos)) != -1) {
                line.append(rx.cap(1));
                pos += rx.matchedLength();
            }

            //Если данные о командах геймпада пришли то имитация датчиков о положении в пространстве и отправка их клиенту
            float x = !line.at(0).isNull()? line.at(0).toFloat() : 0;
            float y = !line.at(1).isNull()? line.at(0).toFloat() : 0;
            float z = !line.at(2).isNull()? line.at(0).toFloat() : 0;
            x = x > 0 ? static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/x)) : 0;
            y = y > 0 ? static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/y)) : 0;
            z = z > 0 ? static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/z)) : 0;
            QString sendData = "POS=x" + QString::number(x) +",y:" + QString::number(y) +",z:" + QString::number(z) +";";
            qDebug() << "SERVER SEND POS" << sendData ;
            emit existMessage(sendData);

            //имитация датчиков газа и отправка их клиенту
            int g1 = 1+ rand() % 100;
            int g2 = 1+ rand() % 100;
            int g3 = 1+ rand() % 100;
            int g4 = 1+ rand() % 100;
            QString sendData2 = "GAZ=g:" + QString::number(g1) +",g:" + QString::number(g2) +",g:" + QString::number(g3) +",g:" + QString::number(g4)+";";
            qDebug() << "SERVER SEND GAZ" << sendData2 ;
            emit existMessage(sendData2);

            //имитация датчиков батарей и отправка их клиенту
            int bs = 100;
            int bc1 = 100;
            int bc2 = 100;
            QString sendData3 = "BAT=bs:" + QString::number(bs) +",bc:" + QString::number(bc1) +",bc:" + QString::number(bc2)+";";
            qDebug() << "SERVER SEND BAT" << sendData3;
            emit existMessage(sendData3);
    }
}

void TestServ::socketDisconnected() {
    qDebug() << "disc";
    socket->deleteLater();
}
void TestServ::incommingConnection() // обработчик подключений
{
    while (server->hasPendingConnections()) {
        socket = server->nextPendingConnection();
        connect(socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
        connect(socket, SIGNAL(readyRead()), this, SLOT(socketReady()));

        //timer_send = new QTimer();
        connect(this,SIGNAL(existMessage(QString)),this,SLOT(sendMessage(QString)));
        //connect(timer_send,SIGNAL(timeout()),this,SLOT(sendMessage()));
        //timer_send->start(1000);
    }
}

QAbstractSocket::SocketState TestServ::stateChanged(QAbstractSocket::SocketState state) // обработчик статуса, нужен для контроля за "вещающим"
{
    if(socket == nullptr) {
        return QAbstractSocket::SocketState::ClosingState;
    }
    if(socket->state() == QTcpSocket::ConnectedState) {
    } else {
    }
    qDebug() << state;
    return state;
}

QAbstractSocket::SocketError TestServ::onError(QAbstractSocket::SocketError error)
{
    QString strErorr = "ERROR: " + error;
    qDebug() << strErorr;
    return error;
}
