#include <QCoreApplication>

#include <QUdpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QList>
#include <QMap>

#include <iostream>
#include "protocoludp.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QUdpSocket udpSocketRec;

    bool result = udpSocketRec.bind(6080,QAbstractSocket::ReuseAddressHint);

    if(result == false)
    {
        std::cout<<"Socket can`t open"<<std::endl;
        return 0;
    }
    //! подключение сокета
    QObject::connect(&udpSocketRec, &QUdpSocket::readyRead,[=, &udpSocketRec] () {
        QByteArray datagram;
        RF60xUDPPACKET packet;
        //! список буфера с данными
        QMap<uint16_t,QTextStream*> buffer;
        //! список файлов
        QList<QFile* > files;
        do
        {
            datagram.resize(udpSocketRec.pendingDatagramSize());
            udpSocketRec.readDatagram(datagram.data(), datagram.size());
        }while (udpSocketRec.hasPendingDatagrams());

        //! выдача данных
        QDataStream outHead(&datagram,QIODevice::ReadOnly);
        outHead.setVersion(QDataStream::Qt_4_2);
        outHead.readRawData(reinterpret_cast<char *> (&packet),sizeof(RF60xUDPPACKET));
        if(buffer.count(packet.wDeviceSerial) == 0)
        {
            QFile *tempFile = new QFile("Dev_" + QString::number(packet.wDeviceSerial) + ".csv");
            files.append(tempFile);
            QTextStream out(tempFile);
            buffer.insert(packet.wDeviceSerial,&out);
            std::cout<<"Add files"<<std::endl;
        }
        //! выбираем буфер который нужно заполнить
        QTextStream *out = buffer[packet.wDeviceSerial];
        //! кол-во записей в полученном проекте
        for(int i = 0; i < packet.bPackCount; i++)
            (*out)<<packet.rf60xValArray[i].wMeasure<<";"<<"\n";

    });

    return a.exec();
}

