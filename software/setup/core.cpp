#include "core.h"
#include <QApplication>
#include <QDebug>
#include <QtConcurrent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFileDialog>
#include <QFile>

const QString reposityroBaseUrl = "https://raw.githubusercontent.com/NeoProg2013/Skynet_configurations/master/";
const QString versionFileName = "VERSION";


Core::Core(QObject *parent) : QObject(parent) {

}

Core::~Core() {

}

bool Core::findDevice() {

	auto future = QtConcurrent::run(&m_modbus, &Modbus::findDevice);
	while (future.isFinished() == false) {
		QApplication::processEvents();
	}

	return future.result();
}

bool Core::requestConfigurationListFromServer() {

	//
	// Check SSL
	//
	emit showActionMessage("Check SSL library...");
	if (QSslSocket::supportsSsl() == false) {
		emit showActionResult("FAIL");
		return false;
	}
	emit showActionResult("OK");


	//
	// Download file with version list
	//
	emit showActionMessage("Getting available versions...");
	QNetworkRequest request(QUrl(reposityroBaseUrl + versionFileName));
	QNetworkReply* reply = m_accessManager.get(request);

	QTimer timeoutTimer;
	timeoutTimer.setInterval(5000);
	timeoutTimer.setSingleShot(true);
	timeoutTimer.start();
	while (!reply->isFinished()) {

		if (timeoutTimer.isActive() == false) {
			emit showActionResult("FAIL");
			return false;
		}
		QApplication::processEvents();
	}

	// Check errors
	if (reply->error() != QNetworkReply::NetworkError::NoError) {
		emit showActionResult("FAIL");
		return false;
	}
	emit showActionResult("OK");

	// Read data
	QString rawFileData = QString::fromUtf8(reply->readAll());
	delete reply;


	//
	// Get configuration list
	//
	emit showActionMessage("Getting configuration list...");
	QList<QString> versionList;
	while (true) {

		int index = rawFileData.indexOf("\r\n");
		if (index == -1) {
			break;
		}

		QString version = rawFileData.mid(0, index);
		rawFileData.remove(0, index + 2);

		emit configurationFound(version);
		QApplication::processEvents();
	}
	emit configurationFound(rawFileData);	// Last version number
	emit showActionResult("OK");


	emit showOperationCompletedMessage("Operation completed");
	return true;
}

bool Core::requestConfigurationFileFromServer(QString version) {

	//
	// Check SSL
	//
	emit showActionMessage("Check SSL library...");
	if (QSslSocket::supportsSsl() == false) {
		emit showActionResult("FAIL");
		return false;
	}
	emit showActionResult("OK");


	//
	// Download configuration file
	//
	emit showActionMessage("Download configuration...");
	QNetworkRequest request(QUrl(reposityroBaseUrl + version));
	QNetworkReply* reply = m_accessManager.get(request);

	QTimer timeoutTimer;
	timeoutTimer.setInterval(5000);
	timeoutTimer.setSingleShot(true);
	timeoutTimer.start();
	while (!reply->isFinished()) {

		if (timeoutTimer.isActive() == false) {
			emit showActionResult("FAIL");
			return false;
		}
		QApplication::processEvents();
	}

	// Check errors
	if (reply->error() != QNetworkReply::NetworkError::NoError) {
		emit showActionResult("FAIL");
		return false;
	}
	emit showActionResult("OK");


	//
	// Save configuration to file
	//
	QString pathToSaveFile = QFileDialog::getSaveFileName(nullptr, "", version + ".eehex", "*.eehex");
	if (pathToSaveFile.size() == 0) {
		return true;
	}

	// Create file
	emit showActionMessage("Create file...");
	QFile saveFile(pathToSaveFile);
	if (saveFile.open(QFile::ReadWrite) == false) {
		emit showActionResult("FAIL");
		return false;
	}
	emit showActionResult("OK");

	// Write data to file
	saveFile.write(reply->readAll());
	delete reply;
	saveFile.close();

	emit showOperationCompletedMessage("Operation completed");
	return true;
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
		emit showActionResult("FAIL");
		return false;
	}
	emit showActionResult("OK");

	// Read data
	QString rawFileData = QString::fromUtf8(saveFile.readAll());
	saveFile.close();

	// Get memory dump
	emit showActionMessage("Getting memory dump...");
	QByteArray memoryDump;
	this->getMemoryDumpFromRawData(rawFileData, memoryDump);
	emit showActionResult("OK");

	// Search device
	emit showActionMessage("Searching device...");
	if (m_modbus.findDevice() == false) {
		emit showActionResult("FAIL");
		return false;
	}
	emit showActionResult("OK");

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
		emit showActionMessage("Writting data to address " + address + "...");
		if (m_modbus.writeEEPROM(static_cast<uint16_t>(i), dataBlock) == false) {
			emit showActionResult("FAIL");
			return false;
		}
		emit showActionResult("OK");
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
