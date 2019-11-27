#ifndef CORE_H
#define CORE_H

#include <QObject>
#include <QNetworkAccessManager>
#include "modbus.h"

class Core : public QObject {

	Q_OBJECT

public:
	explicit Core(QObject *parent = nullptr);
	virtual ~Core();
	Q_INVOKABLE bool findDevice();
	Q_INVOKABLE bool requestConfigurationListFromServer();
	Q_INVOKABLE bool requestConfigurationFileFromServer(QString version);
	Q_INVOKABLE bool loadConfigurationToDevice();

signals:
	void configurationFound(QString configurationName);
	void showActionMessage(QString message);
	void showActionResult(QString result);
	void showOperationCompletedMessage(QString message);

protected:
	void getMemoryDumpFromRawData(QString &rawData, QByteArray &memoryDump);

protected:
	Modbus m_modbus;
	QNetworkAccessManager m_accessManager;
};

#endif // CORE_H
