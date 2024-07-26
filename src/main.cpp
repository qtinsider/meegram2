#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QFontDatabase>
#include <QTextCodec>

#include "Application.hpp"
#include "Authorization.hpp"
#include "Chat.hpp"
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
#include "TextFormatter.hpp"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setApplicationName(AppName);
    QCoreApplication::setApplicationVersion(AppVersion);
    QCoreApplication::setOrganizationName("insider");

    QFontDatabase::addApplicationFont(":/fonts/Icons.ttf");
    QFontDatabase::addApplicationFont(":/fonts/NotoEmoji-Regular.ttf");
    QFontDatabase::addApplicationFont(":/fonts/NotoSansSymbols-Regular.ttf");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    qRegisterMetaType<TdApi::AuthorizationState>("TdApi::AuthorizationState");
    qRegisterMetaType<TdApi::ChatList>("TdApi::ChatList");
    qRegisterMetaType<QModelIndex>("QModelIndex");

    qmlRegisterType<Authorization>("MyComponent", 1, 0, "Authorization");
    qmlRegisterType<Chat>("MyComponent", 1, 0, "Chat");

    qmlRegisterType<ChatModel>("MyComponent", 1, 0, "ChatModel");
    qmlRegisterType<ChatFolderModel>("MyComponent", 1, 0, "ChatFolderModel");
    qmlRegisterType<CountryModel>("MyComponent", 1, 0, "CountryModel");
    qmlRegisterType<FlexibleListModel>("MyComponent", 1, 0, "FlexibleListModel");
    qmlRegisterType<MessageModel>("MyComponent", 1, 0, "MessageModel");

    qmlRegisterType<LottieAnimation>("MyComponent", 1, 0, "LottieAnimation");
    qmlRegisterType<TextFormatter>("MyComponent", 1, 0, "TextFormatter");

    qmlRegisterUncreatableType<TdApi>("MyComponent", 1, 0, "TdApi", "TdApi should not be created in QML");

    QDeclarativeView viewer;
    new DBusAdaptor(&app, &viewer);

    Client client;
    Locale locale;
    Settings settings;
    StorageManager storageManager(&client, &locale);

    Application application(&settings, &storageManager);

    viewer.rootContext()->setContextProperty("app", &application);
    viewer.rootContext()->setContextProperty("tdclient", &client);
    viewer.rootContext()->setContextProperty("settings", &settings);
    viewer.rootContext()->setContextProperty("storageManager", &storageManager);

    viewer.rootContext()->setContextProperty("AppVersion", AppVersion);
    viewer.engine()->addImageProvider("chatPhoto", new ChatPhotoProvider);

    QObject::connect(viewer.engine(), SIGNAL(quit()), &viewer, SLOT(close()));

    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showFullScreen();

    return app.exec();
}
