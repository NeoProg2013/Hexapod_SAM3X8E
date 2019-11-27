#ifndef WIRELESSMODBUS_H
#define WIRELESSMODBUS_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QThread>

#define SCR_REGISTER_ADDRESS								(0x0060)
#define		SCR_CMD_SELECT_SEQUENCE_UP                      (0x01)
#define		SCR_CMD_SELECT_SEQUENCE_DOWN                    (0x02)
#define		SCR_CMD_SELECT_SEQUENCE_RUN                     (0x03)
#define		SCR_CMD_SELECT_SEQUENCE_DIRECT_MOVEMENT         (0x04)
#define		SCR_CMD_SELECT_SEQUENCE_REVERSE_MOVEMENT        (0x05)
#define		SCR_CMD_SELECT_SEQUENCE_ROTATE_LEFT             (0x06)
#define		SCR_CMD_SELECT_SEQUENCE_ROTATE_RIGHT            (0x07)
#define		SCR_CMD_SELECT_SEQUENCE_DIRECT_MOVEMENT_SLOW    (0x08)
#define		SCR_CMD_SELECT_SEQUENCE_REVERSE_MOVEMENT_SLOW   (0x09)
#define		SCR_CMD_SELECT_SEQUENCE_SHIFT_LEFT              (0x10)
#define		SCR_CMD_SELECT_SEQUENCE_SHIFT_RIGHT             (0x11)

#define		SCR_CMD_SELECT_SEQUENCE_ATTACK_LEFT             (0x20)
#define		SCR_CMD_SELECT_SEQUENCE_ATTACK_RIGHT            (0x21)
#define		SCR_CMD_SELECT_SEQUENCE_DANCE                   (0x30)
#define		SCR_CMD_SELECT_SEQUENCE_ROTATE_X				(0x31)
//#define	SCR_CMD_SELECT_SEQUENCE_ROTATE_Y				(0x32)
#define		SCR_CMD_SELECT_SEQUENCE_ROTATE_Z				(0x33)

#define		SCR_CMD_SELECT_SEQUENCE_NONE                    (0x90)

#define		SCR_CMD_CALCULATE_CHECKSUM                      (0xB0)
#define		SCR_CMD_ENABLE_FRONT_SENSOR                     (0xB1)
#define		SCR_CMD_DISABLE_FRONT_SENSOR                    (0xB2)

#define MAIN_BLOCK_ADDRESS									(0x0010)


class WirelessModbus : public QObject {

	Q_OBJECT

public:
	explicit WirelessModbus(QObject* parent = nullptr);
	bool isOperationInProgress() const;
	bool operationResult() const;

public slots:
	void connectToServer(void);
	void disconnectFromServer(void);
	void readRAM(uint16_t address, QByteArray* buffer, uint8_t bytesCount);
	void writeRAM(uint16_t address, QByteArray data);
	void readEEPROM(uint16_t address, QByteArray* buffer, uint8_t bytesCount);
	void writeEEPROM(uint16_t address, const QByteArray& data);

protected:
	void initialize();
	bool processModbusTransaction(const QByteArray& request, QByteArray* responseData);
	uint16_t calculateCRC16(const QByteArray &frameByteArray);

private:
	QTcpSocket* m_socket;
	QTimer* m_timeoutTimer;
	bool m_operationInProgress;
	bool m_operationResult;
};

#endif // WIRELESSMODBUS_H
