#include <QApplication>
#include <QStyleFactory>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStringList>
#include <QQuickView>
#include <QQmlContext>
#include <QStyle>

#include "palette.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QList<qreal> points;
    points.append(0.0f);
    points.append(20.0f);
    points.append(40.0f);
    points.append(60.0f);
    points.append(80.0f);
    points.append(100.0f);

    QList<qreal> red;
    red.append(0.0f);
    red.append(88.0f);
    red.append(32.0f);
    red.append(64.0f);
    red.append(180.0f);
    red.append(255.0f);

    QList<qreal> green;
    green.append(0.0f);
    green.append(20.0f);
    green.append(30.0f);
    green.append(40.0f);
    green.append(100.0f);
    green.append(255.0f);

    QList<qreal> blue;
    blue.append(0.0f);
    blue.append(10.0f);
    blue.append(50.0f);
    blue.append(10.0f);
    blue.append(50.0f);
    blue.append(10.0f);

    // QList<QObject*> red;
    // red.append(new DataObject(0.0, 0.0));
    // red.append(new DataObject(20.0, 88.0));
    // red.append(new DataObject(40.0, 32.0));
    // red.append(new DataObject(60.0, 64.0));
    // red.append(new DataObject(80.0, 180.0));
    // red.append(new DataObject(100.0, 255.0));

    // QList<QObject*> green;
    // red.append(new DataObject(0.0, 0.0));
    // red.append(new DataObject(20.0, 88.0));
    // red.append(new DataObject(40.0, 32.0));
    // red.append(new DataObject(60.0, 64.0));
    // red.append(new DataObject(80.0, 180.0));
    // red.append(new DataObject(100.0, 255.0));

    // QList<QObject*> blue;
    // red.append(new DataObject(0.0, 0.0));
    // red.append(new DataObject(20.0, 88.0));
    // red.append(new DataObject(40.0, 32.0));
    // red.append(new DataObject(60.0, 64.0));
    // red.append(new DataObject(80.0, 180.0));
    // red.append(new DataObject(100.0, 255.0));

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("curve_points", QVariant::fromValue(points));
    engine.rootContext()->setContextProperty("red", QVariant::fromValue(red));
    engine.rootContext()->setContextProperty("green", QVariant::fromValue(green));
    engine.rootContext()->setContextProperty("blue", QVariant::fromValue(blue));

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
