#include <QGuiApplication>
#include <QHostAddress>
#include <QDebug>
#include <QSerialPortInfo>
#include "modbus.h"

#define CRC16_POLYNOM						(0xA001)

#define MODBUS_CMD_WRITE_RAM				(0x41)	// Function Code: Write RAM
#define MODBUS_CMD_WRITE_EEPROM				(0x43)	// Function Code: Write EEPROM
#define MODBUS_CMD_READ_RAM					(0x44)	// Function Code: Read RAM
#define MODBUS_CMD_READ_EEPROM				(0x46)	// Function Code: Read EEPROM
#define MODBUS_EXCEPTION					(0x80)	// Function Code: Exception

#define MODBUS_MIN_RESPONSE_LENGTH			(4)



Modbus::Modbus(QObject* parent) : QObject(parent) {

}

bool Modbus::findDevice(void) {

	qDebug() << "Modbus: [findDevice] Start";

	// Find device
	auto serialPortList = QSerialPortInfo::availablePorts();
	for (int i = 0; i < serialPortList.size(); ++i) {

        m_portName = serialPortList[i].portName();

        if (this->checkDevice() == true) {
            qDebug() << "Modbus: [findDevice] Device found";
            return true;
        }

        m_portName.clear();
	}

    qDebug() << "Modbus: [findDevice] Device not found";
	qDebug() << "Modbus: [findDevice] Stop";
	return false;
}

bool Modbus::checkDevice(void) {

    qDebug() << "Modbus: [checkDevice] Start";

    QByteArray id;
    if (this->readRAM(0x0000, &id, 4) == false) {
        qDebug() << "Modbus: [checkDevice] Cannot read RAM";
        return false;
    }

    /*if (id.at(0) == static_cast<char>(0xAB) && id.at(1) == static_cast<char>(0xCD) &&
        id.at(2) == static_cast<char>(0xEF) && id.at(3) == static_cast<char>(0xFF)) {

        qDebug() << "Modbus: [checkDevice] Wrong ID";
        return true;
    }

    return false;*/

    return true;
}

bool Modbus::readRAM(uint16_t address, QByteArray* buffer, uint8_t bytesCount) {

	qDebug() << "Modbus: [readRAM] Start";

	// Make request
	QByteArray request;
	request.push_back(static_cast<char>(0xFE));
	request.push_back(static_cast<char>(MODBUS_CMD_READ_RAM));
	request.push_back(static_cast<char>((address & 0xFF00) >> 8));
	request.push_back(static_cast<char>((address & 0x00FF) >> 0));
	request.push_back(static_cast<char>(bytesCount));

	uint16_t crc = calculateCRC16(request);
	request.push_back(static_cast<char>((crc & 0x00FF) >> 0));
	request.push_back(static_cast<char>((crc & 0xFF00) >> 8));

	// Send request and receive response
	buffer->clear();
	bool operationResult = this->processModbusTransaction(request, buffer);

	qDebug() << "Modbus: [readRAM] Stop";
	return operationResult;
}

bool Modbus::writeRAM(uint16_t address, QByteArray data) {

	qDebug() << "Modbus: [writeRAM] Start";

	// Make request
	QByteArray request;
	request.push_back(static_cast<char>(0xFE));
	request.push_back(static_cast<char>(MODBUS_CMD_WRITE_RAM));
	request.push_back(static_cast<char>((address & 0xFF00) >> 8));
	request.push_back(static_cast<char>((address & 0x00FF) >> 0));
	request.push_back(static_cast<char>(data.size()));
	for (int i = 0; i < data.size(); ++i) {
		request.push_back(static_cast<char>(data[i]));
	}

	uint16_t crc = calculateCRC16(request);
	request.push_back(static_cast<char>((crc & 0x00FF) >> 0));
	request.push_back(static_cast<char>((crc & 0xFF00) >> 8));

	// Send request and receive response
	bool operationResult = processModbusTransaction(request, nullptr);

	qDebug() << "Modbus: [writeRAM] Stop";
	return operationResult;
}

bool Modbus::readEEPROM(uint16_t address, QByteArray* buffer, uint8_t bytesCount) {

	qDebug() << "Modbus: [readEEPROM] Start";

	// Make request
	QByteArray request;
	request.push_back(static_cast<char>(0xFE));
	request.push_back(static_cast<char>(MODBUS_CMD_READ_EEPROM));
	request.push_back(static_cast<char>((address & 0xFF00) >> 8));
	request.push_back(static_cast<char>((address & 0x00FF) >> 0));
	request.push_back(static_cast<char>(bytesCount));

	uint16_t crc = calculateCRC16(request);
	request.push_back(static_cast<char>((crc & 0x00FF) >> 0));
	request.push_back(static_cast<char>((crc & 0xFF00) >> 8));

	// Send request and receive response
	buffer->clear();
	bool operationResult = this->processModbusTransaction(request, buffer);

	qDebug() << "Modbus: [readEEPROM] Stop";
	return operationResult;
}

bool Modbus::writeEEPROM(uint16_t address, const QByteArray& data) {

	qDebug() << "Modbus: [writeRAM] Start";

	// Make request
	QByteArray request;
	request.push_back(static_cast<char>(0xFE));
	request.push_back(static_cast<char>(MODBUS_CMD_WRITE_EEPROM));
	request.push_back(static_cast<char>((address & 0xFF00) >> 8));
	request.push_back(static_cast<char>((address & 0x00FF) >> 0));
	request.push_back(static_cast<char>(data.size()));
	for (int i = 0; i < data.size(); ++i) {
		request.push_back(static_cast<char>(data[i]));
	}

	uint16_t crc = calculateCRC16(request);
	request.push_back(static_cast<char>((crc & 0x00FF) >> 0));
	request.push_back(static_cast<char>((crc & 0xFF00) >> 8));

	// Send request and receive response
	bool operationResult = processModbusTransaction(request, nullptr);

	qDebug() << "Modbus: [writeRAM] Stop";
	return operationResult;
}

QString Modbus::devicePortName() const {

    return m_portName;
}





bool Modbus::processModbusTransaction(const QByteArray& request, QByteArray* responseData) {

    // Configure port
    QSerialPort port;
    port.setPortName(m_portName);
    port.setBaudRate(QSerialPort::BaudRate::Baud115200);
    port.setParity(QSerialPort::Parity::NoParity);
    port.setStopBits(QSerialPort::StopBits::TwoStop);
    port.setDataBits(QSerialPort::DataBits::Data8);

	// Open and clear serial port
    if (port.open(QSerialPort::ReadWrite) == false) {
		return false;
	}
    port.readAll();

	// Send request
    port.write(request);
    port.flush();

	// Wait response
    QTimer timeoutTimer;
    timeoutTimer.setInterval(500);
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start();
    while (port.bytesAvailable() < MODBUS_MIN_RESPONSE_LENGTH) {

        QGuiApplication::processEvents();

        if (timeoutTimer.isActive() == false) {
            port.close();
			return false;
        }
	}

	// Read response
    QByteArray response = port.readAll();
    port.close();

	// Verify response
	if (this->calculateCRC16(response) != 0) {
		return false;
	}

	// Check function code or exception
	if (response[1] & MODBUS_EXCEPTION) {
		return false;
	}

	// Copy data from response
	if (request[1] == MODBUS_CMD_READ_RAM || request[1] == MODBUS_CMD_READ_EEPROM) {
		*responseData = response.mid(3, request[4]);
	}

	return true;
}

uint16_t Modbus::calculateCRC16(const QByteArray& frameByteArray) {

	uint8_t size = static_cast<uint8_t>(frameByteArray.size());
	const uint8_t* frame = reinterpret_cast<const uint8_t*>(frameByteArray.data());


	uint16_t crc16 = 0xFFFF;
	uint16_t data = 0;
	uint16_t k = 0;

	while (size--) {
		crc16 ^= *frame++;
		k = 8;
		while (k--) {
			data = crc16;
			crc16 >>= 1;
			if (data & 0x0001) {
				crc16 ^= CRC16_POLYNOM;
			}
		}
	}
	return crc16;
}
