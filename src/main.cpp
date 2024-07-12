#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QFontDatabase>
#include <QTextCodec>

#include "Application.hpp"
#include "Authorization.hpp"
#include "ChatModel.hpp"
#include "Client.hpp"
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

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(AppName);
    QCoreApplication::setApplicationVersion(AppVersion);

    QFontDatabase::addApplicationFont(":/fonts/Icons.ttf");
    QFontDatabase::addApplicationFont(":/fonts/NotoEmoji-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/NotoSansSymbols-Regular.ttf");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    qRegisterMetaType<TdApi::AuthorizationState>("TdApi::AuthorizationState");
    qRegisterMetaType<TdApi::ChatList>("TdApi::ChatList");
    qRegisterMetaType<QModelIndex>("QModelIndex");

    qmlRegisterType<Authorization>("com.strawberry.meegram", 1, 0, "Authorization");
    qmlRegisterType<ChatModel>("com.strawberry.meegram", 1, 0, "ChatModel");
    qmlRegisterType<ChatFolderModel>("com.strawberry.meegram", 1, 0, "ChatFolderModel");
    qmlRegisterType<CountryModel>("com.strawberry.meegram", 1, 0, "CountryModel");
    qmlRegisterType<MessageModel>("com.strawberry.meegram", 1, 0, "MessageModel");
    qmlRegisterType<LottieAnimation>("com.strawberry.meegram", 1, 0, "LottieAnimation");
    qmlRegisterUncreatableType<TdApi>("com.strawberry.meegram", 1, 0, "TdApi", "TdApi should not be created in QML");

    QDeclarativeView viewer;
    Application application;

    new DBusAdaptor(&app, &viewer);

    viewer.rootContext()->setContextProperty("app", &application);
    viewer.rootContext()->setContextProperty("AppVersion", AppVersion);
    viewer.engine()->addImageProvider("chatPhoto", new ChatPhotoProvider);

    QObject::connect(viewer.engine(), SIGNAL(quit()), &viewer, SLOT(close()));

    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showFullScreen();

    return app.exec();
}
