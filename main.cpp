#include <QCoreApplication>

#include <QUdpSocket>
#include <QByteArray>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QUdpSocket udpSocketRec;

    bool result = udpSocketRec.bind(6080,QAbstractSocket::ReuseAddressHint);

//    connect(&udpSocketRec, SIGNAL(readyRead()),this, SLOT(processPendingDatagrams()));

    return a.exec();
}
