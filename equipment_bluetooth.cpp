#include "equipment_bluetooth.h"
#include <QDebug>
#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

EquipmentBluetooth::EquipmentBluetooth(QObject* parent)
    : QObject(parent), serial(new QSerialPort(this))
{}

EquipmentBluetooth::~EquipmentBluetooth() {
    disconnectBluetooth();
}

bool EquipmentBluetooth::connectToBluetooth(const QString& portName) {
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open" << portName << ":" << serial->errorString();
        return false;
    }
    qDebug() << "Bluetooth opened on" << portName;
    // forward incoming bytes
    connect(serial, &QSerialPort::readyRead, this, [this]() {
        QByteArray data = serial->readAll();
        emit dataReceived(data);
    });
    return true;
}

void EquipmentBluetooth::disconnectBluetooth() {
    if (serial->isOpen())
        serial->close();
}

void EquipmentBluetooth::writeData(const QByteArray &data) {
    if (serial->isOpen())
        serial->write(data);
}
