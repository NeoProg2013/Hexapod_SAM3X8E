#include "core.h"
#include <QApplication>
#include <QDebug>
#include <QtConcurrent>
#include <QFileDialog>
#include <QFile>


Core::Core(QObject *parent) : QObject(parent) {

}

Core::~Core() {

}

bool Core::findDevice() {

    emit showActionMessage("Search device...");

    auto future = QtConcurrent::run(&m_modbus, &Modbus::findDevice);
    bool result = waitThreadOperationComplete(future);

    emit showActionResult(result);
    if (result == true) {
        emit showActionMessage("Device founded on " + m_modbus.devicePortName());
        emit showActionResult(result);
    }
    return result;
}

bool Core::loadConfigurationToDevice() {

    QString pathToFile = QFileDialog::getOpenFileName(nullptr, "", "", "*.eehex");
	if (pathToFile.size() == 0) {
		return true;
	}

	// Open file
	emit showActionMessage("Reading file...");
	QFile saveFile(pathToFile);
	if (saveFile.open(QFile::ReadWrite) == false) {
        emit showActionResult(false);
		return false;
	}
    emit showActionResult(true);

	// Read data
	QString rawFileData = QString::fromUtf8(saveFile.readAll());
	saveFile.close();

	// Get memory dump
	emit showActionMessage("Getting memory dump...");
	QByteArray memoryDump;
	this->getMemoryDumpFromRawData(rawFileData, memoryDump);
    emit showActionResult(true);

    // Check device
    emit showActionMessage("Check device...");
    if (m_modbus.findDevice() == false) {
        emit showActionResult(false);
        return false;
    }
    emit showActionResult(true);

	// Write data to device
	for (int i = 0; i < memoryDump.size(); i += 16) {

		// Make data block
		QByteArray dataBlock = memoryDump.mid(i, 16);

		// Make address for log message
		QString address = QString::number(i, 16).toUpper();
		while (address.size() != 4) {
			address.insert(0, '0');
		}

		// Send modbus request for write data
        emit showActionMessage("Writting data to address 0x" + address + "...");
		if (m_modbus.writeEEPROM(static_cast<uint16_t>(i), dataBlock) == false) {
            emit showActionResult(false);
			return false;
		}
        emit showActionResult(true);
	}

    emit showOperationCompletedMessage("Operation completed");
	return true;
}




void Core::getMemoryDumpFromRawData(QString& rawData, QByteArray& memoryDump) {

    // Remove spaces and \r \n symbols
	while (true) {

		int index = rawData.indexOf(QRegExp("\\ |\\\n|\\\r"));
		if (index == -1) {
			break;
		}
		rawData.remove(index, 1);
	}

	// Parse data
	while (true) {

		// Search and remove address
		int index = rawData.indexOf(QRegExp("....\\:"));
		if (index == -1) {
			break;
		}
		rawData.remove(index, 5);

		// Read data line
		for (int i = 0; i < 16; ++i) {

			QString numberStr = rawData.mid(0, 2);
			memoryDump.push_back(static_cast<char>(numberStr.toInt(nullptr, 16)));

			rawData.remove(0, 2);
		}
	}
}

bool Core::waitThreadOperationComplete(QFuture<bool>& future) {

    while (future.isFinished() == false) {
        QApplication::processEvents();
    }
    return future.result();
}
