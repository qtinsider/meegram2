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
#include "ChatFolderModel.hpp"
#include "ChatModel.hpp"
#include "ChatPhotoProvider.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "CountryModel.hpp"
#include "DBusAdaptor.hpp"
#include "File.hpp"
#include "IconProvider.hpp"
#include "LanguagePackInfoModel.hpp"
#include "Localization.hpp"
#include "LottieAnimation.hpp"
#include "Message.hpp"
#include "MessageModel.hpp"
#include "NotificationManager.hpp"
#include "QrCodeItem.hpp"
#include "Settings.hpp"
#include "SortFilterProxyModel.hpp"
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

    qRegisterMetaType<qlonglong>("qlonglong");

    qRegisterMetaType<TdApi::AuthorizationState>("TdApi::AuthorizationState");
    qRegisterMetaType<TdApi::ChatList>("TdApi::ChatList");

    qRegisterMetaType<QModelIndex>("QModelIndex");

    qmlRegisterType<Authorization>("MyComponent", 1, 0, "Authorization");
    qmlRegisterType<Chat>("MyComponent", 1, 0, "Chat");
    qmlRegisterType<File>("MyComponent", 1, 0, "File");
    qmlRegisterType<Message>("MyComponent", 1, 0, "Message");

    qmlRegisterType<ChatModel>("MyComponent", 1, 0, "ChatModel");
    qmlRegisterType<ChatFolderModel>("MyComponent", 1, 0, "ChatFolderModel");
    qmlRegisterType<CountryModel>("MyComponent", 1, 0, "CountryModel");
    qmlRegisterType<LanguagePackInfoModel>("MyComponent", 1, 0, "LanguagePackInfoModel");
    qmlRegisterType<MessageModel>("MyComponent", 1, 0, "MessageModel");

    qmlRegisterType<LottieAnimation>("MyComponent", 1, 0, "LottieAnimation");
    qmlRegisterType<QrCodeItem>("MyComponent", 1, 0, "QrCode");
    qmlRegisterType<SortFilterProxyModel>("MyComponent", 1, 0, "SortFilterProxyModel");
    qmlRegisterType<TextFormatter>("MyComponent", 1, 0, "TextFormatter");

    qmlRegisterUncreatableType<TdApi>("MyComponent", 1, 0, "TdApi", "TdApi should not be created in QML");

    QDeclarativeView viewer;
    new DBusAdaptor(&app, &viewer);

    Application application;

    Translator translator;
    app.installTranslator(&translator);

    viewer.rootContext()->setContextProperty("app", &application);
    viewer.rootContext()->setContextProperty("client", StorageManager::instance().client());
    viewer.rootContext()->setContextProperty("settings", &Settings::instance());

    viewer.rootContext()->setContextProperty("AppVersion", AppVersion);

    viewer.engine()->addImageProvider("icon", new IconProvider);
    viewer.engine()->addImageProvider("chatPhoto", new ChatPhotoProvider);

    QObject::connect(viewer.engine(), SIGNAL(quit()), &viewer, SLOT(close()));

    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showFullScreen();

    return app.exec();
}
