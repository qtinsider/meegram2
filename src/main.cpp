#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QFile>
#include <QFontDatabase>
#include <QTextCodec>

#include "ChatModel.hpp"
#include "Common.hpp"
#include "DBusAdaptor.hpp"
#include "ImageProviders.hpp"
#include "Localization.hpp"
#include "LottieAnimation.hpp"
#include "MessageModel.hpp"
#include "NotificationManager.hpp"
#include "SelectionModel.hpp"
#include "Settings.hpp"
#include "StorageManager.hpp"
#include "TdApi.hpp"
#include "Utils.hpp"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(new QApplication(argc, argv));
    QScopedPointer<QDeclarativeView> viewer(new QDeclarativeView);

    QCoreApplication::setApplicationName(AppName);
    QCoreApplication::setApplicationVersion(AppVersion);

    QFontDatabase::addApplicationFont(":/fonts/Icons.ttf");
    QFontDatabase::addApplicationFont(":/fonts/NotoEmoji-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/NotoSansSymbols-Regular.ttf");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    qmlRegisterType<ChatModel>("com.strawberry.meegram", 1, 0, "ChatModel");
    qmlRegisterType<ChatFilterModel>("com.strawberry.meegram", 1, 0, "ChatFilterModel");
    qmlRegisterType<CountryModel>("com.strawberry.meegram", 1, 0, "CountryModel");
    qmlRegisterType<MessageModel>("com.strawberry.meegram", 1, 0, "MessageModel");

    qmlRegisterType<LottieAnimation>("com.strawberry.meegram", 1, 0, "LottieAnimation");

    qmlRegisterUncreatableType<TdApi>("com.strawberry.meegram", 1, 0, "TdApi", "TdApi should not be created in QML");

    QScopedPointer<Utils> utils(new Utils);
    new DBusAdaptor(app.data(), viewer.data());

    viewer->rootContext()->setContextProperty("AppVersion", AppVersion);

    viewer->rootContext()->setContextProperty("Api", &TdApi::getInstance());
    viewer->rootContext()->setContextProperty("Localization", &Localization::getInstance());
    viewer->rootContext()->setContextProperty("Notification", &NotificationManager::getInstance());
    viewer->rootContext()->setContextProperty("Settings", &Settings::getInstance());
    viewer->rootContext()->setContextProperty("Store", &StorageManager::getInstance());
    viewer->rootContext()->setContextProperty("Utils", utils.data());

    viewer->engine()->addImageProvider("chatPhoto", new ChatPhotoProvider);

    QObject::connect(app.data(), SIGNAL(aboutToQuit()), &TdApi::getInstance(), SLOT(close()));
    QObject::connect(viewer->engine(), SIGNAL(quit()), viewer.data(), SLOT(close()));

    viewer->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer->setSource(QUrl("qrc:/qml/main.qml"));

    viewer->showFullScreen();

    return app->exec();
}
