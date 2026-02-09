#ifndef EQUIPMENT_BLUETOOTH_H
#define EQUIPMENT_BLUETOOTH_H

#include <QObject>
#include <QSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
class EquipmentBluetooth : public QObject {
    Q_OBJECT

public:
    explicit EquipmentBluetooth(QObject* parent = nullptr);
    ~EquipmentBluetooth();

    // Opens the BT module in ReadWrite mode
    bool connectToBluetooth(const QString& portName);
    void disconnectBluetooth();

    // send arbitrary data to Arduino
    void writeData(const QByteArray &data);

signals:
    // emitted when Arduino sends something
    void dataReceived(const QByteArray &data);

private:
    QSerialPort* serial;
};

#endif // EQUIPMENT_BLUETOOTH_H
