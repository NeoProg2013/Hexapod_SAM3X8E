#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QFuture>
#include "modbus.h"

class Core : public QObject {

	Q_OBJECT

public:
	explicit Core(QObject *parent = nullptr);
	virtual ~Core();
	Q_INVOKABLE bool findDevice();
	Q_INVOKABLE bool loadConfigurationToDevice();

signals: // QML
	void configurationFound(QString configurationName);
	void showActionMessage(QString message);
    void showActionResult(bool result);
	void showOperationCompletedMessage(QString message);

protected:
	void getMemoryDumpFromRawData(QString &rawData, QByteArray &memoryDump);
    bool waitThreadOperationComplete(QFuture<bool>& future);

protected:
    Modbus m_modbus;
};

#endif // CORE_H
