#ifndef WIRELESSMODBUS_H
#define WIRELESSMODBUS_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QThread>


class Modbus : public QObject {

	Q_OBJECT

public:
	explicit Modbus(QObject* parent = nullptr);
	bool findDevice(void);
	bool readRAM(uint16_t address, QByteArray* buffer, uint8_t bytesCount);
	bool writeRAM(uint16_t address, QByteArray data);
	bool readEEPROM(uint16_t address, QByteArray* buffer, uint8_t bytesCount);
	bool writeEEPROM(uint16_t address, const QByteArray& data);

protected:
	bool processModbusTransaction(const QByteArray& request, QByteArray* responseData);
	uint16_t calculateCRC16(const QByteArray &frameByteArray);

private:
	QSerialPort m_port;
	QTimer m_timeoutTimer;
};

#endif // WIRELESSMODBUS_H
