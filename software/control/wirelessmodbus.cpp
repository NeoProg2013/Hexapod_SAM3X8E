#include <QGuiApplication>
#include <QHostAddress>
#include <QDebug>
#include "wirelessmodbus.h"
#define SERVER_IP_ADDRESS					("111.111.111.111")
#define SERVER_PORT							(3333)

#define CRC16_POLYNOM						(0xA001)

#define MODBUS_CMD_WRITE_RAM				(0x41)	// Function Code: Write RAM
#define MODBUS_CMD_WRITE_EEPROM				(0x43)	// Function Code: Write EEPROM
#define MODBUS_CMD_READ_RAM					(0x44)	// Function Code: Read RAM
#define MODBUS_CMD_READ_EEPROM				(0x46)	// Function Code: Read EEPROM
#define MODBUS_EXCEPTION					(0x80)	// Function Code: Exception

#define MODBUS_MIN_RESPONSE_LENGTH			(4)



WirelessModbus::WirelessModbus(QObject* parent) : QObject(parent) {

	m_socket = nullptr;
}

bool WirelessModbus::isOperationInProgress() const {

	return m_operationInProgress;
}

bool WirelessModbus::operationResult() const {

	return m_operationResult;
}

void WirelessModbus::connectToServer(void) {

	qDebug() << "WirelessModbus: [connectToServer] Start";
	m_operationInProgress = true;

	this->initialize();

	// Check socket state
	if (m_socket->state() == QTcpSocket::SocketState::ConnectedState) {
		qDebug() << "WirelessModbus: [connectToServer] Wrong socket state";
		return;
	}


	// Connect to server
	m_socket->abort();
	m_socket->connectToHost(QHostAddress(SERVER_IP_ADDRESS), SERVER_PORT);
	m_operationResult = m_socket->waitForConnected(5000);
	if (m_operationResult == false) {
		m_socket->abort();
	}

	qDebug() << "WirelessModbus: [connectToServer] Stop";
	m_operationInProgress = false;
}

void WirelessModbus::disconnectFromServer(void) {

	qDebug() << "WirelessModbus: [disconnectFromServer] Start";
	m_operationInProgress = true;

	this->initialize();
	m_socket->abort();
	m_socket->disconnectFromHost();

	qDebug() << "WirelessModbus: [disconnectFromServer] Stop";
	m_operationResult = true;
	m_operationInProgress = false;
}

void WirelessModbus::readRAM(uint16_t address, QByteArray* buffer, uint8_t bytesCount) {

	qDebug() << "WirelessModbus: [readRAM] Start";
	m_operationInProgress = true;

	this->initialize();

	do
	{
		// Check socket state
		if (m_socket->state() != QTcpSocket::SocketState::ConnectedState) {
			qDebug() << "WirelessModbus: [readRAM] Wrong socket state";
			m_operationResult = false;
			break;
		}

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
		m_operationResult = processModbusTransaction(request, buffer);
	}
	while (false);

	qDebug() << "WirelessModbus: [readRAM] Stop";
	m_operationInProgress = false;
}

void WirelessModbus::writeRAM(uint16_t address, QByteArray data) {

	qDebug() << "WirelessModbus: [writeRAM] Start";
	m_operationInProgress = true;

	this->initialize();

	do
	{
		// Check socket state
		if (m_socket->state() != QTcpSocket::SocketState::ConnectedState) {
			qDebug() << "WirelessModbus: [writeRAM] Wrong socket state";
			m_operationResult = false;
			break;
		}

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
		m_operationResult = processModbusTransaction(request, nullptr);
	}
	while (0);

	qDebug() << "WirelessModbus: [writeRAM] Stop";
	m_operationInProgress = false;
}

void WirelessModbus::readEEPROM(uint16_t address, QByteArray* buffer, uint8_t bytesCount) {

	qDebug() << "WirelessModbus: [readEEPROM] Start";
	m_operationInProgress = true;

	this->initialize();

	do
	{
		// Check socket state
		if (m_socket->state() != QTcpSocket::SocketState::ConnectedState) {
			qDebug() << "WirelessModbus: [readEEPROM] Wrong socket state";
			m_operationResult = false;
			break;
		}

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
		m_operationResult = processModbusTransaction(request, buffer);
	}
	while (false);

	qDebug() << "WirelessModbus: [readEEPROM] Stop";
	m_operationInProgress = false;
}

void WirelessModbus::writeEEPROM(uint16_t address, const QByteArray& data) {

	qDebug() << "WirelessModbus: [writeEEPROM] Start";
	m_operationInProgress = true;

	this->initialize();

	do
	{
		// Check socket state
		if (m_socket->state() != QTcpSocket::SocketState::ConnectedState) {
			qDebug() << "WirelessModbus: [writeEEPROM] Wrong socket state";
			m_operationResult = false;
			break;
		}

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
		m_operationResult = processModbusTransaction(request, nullptr);
	}
	while (0);

	qDebug() << "WirelessModbus: [writeEEPROM] Stop";
	m_operationInProgress = false;
}




void WirelessModbus::initialize() {

	if (m_socket == nullptr) {
		m_socket = new QTcpSocket(this);
		m_timeoutTimer = new QTimer;
	}
}

bool WirelessModbus::processModbusTransaction(const QByteArray& request, QByteArray* responseData) {

	// Clear socket
	m_socket->readAll();

	// Send request
	m_socket->write(request);
	m_socket->flush();

	// Wait response
	m_timeoutTimer->stop();
	m_timeoutTimer->setInterval(100);
	m_timeoutTimer->setSingleShot(true);
	m_timeoutTimer->start();
	while (m_socket->bytesAvailable() < MODBUS_MIN_RESPONSE_LENGTH) {

		QGuiApplication::processEvents();

		if (m_timeoutTimer->isActive() == false) {
			return false;
		}
	}

	// Read response
	QByteArray response = m_socket->readAll();

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

uint16_t WirelessModbus::calculateCRC16(const QByteArray& frameByteArray) {

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
