#include <QCoreApplication>

#include <QUdpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QList>
#include <QMap>
#include <QDateTime>

#include <iostream>
#include "protocoludp.h"
//! перекодировка кода в мм
double covertCodeToMM(uint16_t d, double s)
{
    return (d * s)/0x4000;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QUdpSocket udpSocketRec;


    bool result = udpSocketRec.bind(603,QAbstractSocket::ReuseAddressHint);

    if(result == false)
    {
        std::cout<<"Socket can`t open"<<std::endl;
        return 0;
    }else {
        std::cout<<"size = "<<sizeof(RF60xUDPPACKET)<<std::endl;
    }
    //! подключение сокета
    QCoreApplication::connect(&udpSocketRec, &QUdpSocket::readyRead,[=, &udpSocketRec] () {
        QByteArray datagram;
        RF60xUDPPACKET packet;
        //! список буфера с данными
        static QMap<uint16_t,QTextStream*> buffer;
        static QMap<uint16_t,double*> timeStep;
        //! список файлов
        static QList<QFile* > files;
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
            QFile *tempFile = new QFile("Dev_" + QString::number(packet.wDeviceSerial) + "_"+QDateTime::currentDateTime().toString("dd_hh_mm")+ ".csv");
            tempFile->open(QIODevice::ReadWrite | QIODevice::Text);
            files.append(tempFile);
            QTextStream* out = new QTextStream(tempFile);
            double *time = new double (0.0);
            buffer.insert(packet.wDeviceSerial,out);
            timeStep.insert(packet.wDeviceSerial,time);
            std::cout<<"Find sensor N="<<packet.wDeviceSerial<<std::endl;
        }
        //! выбираем буфер который нужно заполнить
        QTextStream *out = buffer[packet.wDeviceSerial];
        double *time = timeStep[packet.wDeviceSerial];
        //! кол-во записей в полученном проекте
        //for(int i = 0; i < packet.bPackCount; i++)
        for(int i = 0; i < 168; i++)
        {
            (*time)+=0.005;
            (*out)<<QString::number(*time)
                  <<";"<<QString::number(covertCodeToMM(packet.rf60xValArray[i].wMeasure,packet.wDeviceMeasureRange))
                  <<";"<<"\n";
            double j;
            if(modf(*time,&j) == 0)
            {
                std::cout<<"Sensor #"<< packet.wDeviceSerial<<": d = "
                                     << covertCodeToMM(packet.rf60xValArray[i].wMeasure,packet.wDeviceMeasureRange)
                                     << std::endl;
            }

        }

        out->flush();
    });

    return a.exec();
}

