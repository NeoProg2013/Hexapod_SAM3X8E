#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QDebug>
#include "core.h"

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);
	Core core;

	QQmlApplicationEngine engine;
	engine.rootContext()->setContextProperty("CppCore", &core);
	engine.load(QUrl(QStringLiteral("qrc:/QML/main.qml")));
	if (engine.rootObjects().isEmpty()) {
		return -1;
	}

	return app.exec();
}
