#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "SongModel.h"
#include "SongList.h"
#include "SongPlayer.h"
#include "ImageItem.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    SongList * songList = new SongList;
    SongPlayer * songPlayer = new SongPlayer;

    songPlayer->setList(songList);
    songPlayer->loadFolder("C:/Users/Admin/Music");

    qmlRegisterUncreatableType<SongList>("MediaPlayer", 1, 0, "SongList",
                                         "SongList cannot be created inside QML");
    qmlRegisterType<SongModel>("MediaPlayer", 1, 0, "SongModel");
    qmlRegisterType<ImageItem>("MediaPlayer", 1, 0, "ImageItem");

    engine.rootContext()->setContextProperty("songList", songList);
    engine.rootContext()->setContextProperty("songPlayer", songPlayer);

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
