#include <QCoreApplication>
#include "testserv.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    TestServ * tcpControl = new TestServ();


    return a.exec();
}
