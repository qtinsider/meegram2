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
#include "ChatPosition.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "CountryModel.hpp"
#include "File.hpp"
#include "LanguagePackInfoModel.hpp"
#include "Localization.hpp"
#include "LottieAnimation.hpp"
#include "Message.hpp"
#include "MessageModel.hpp"
#include "QrCodeItem.hpp"
#include "Settings.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

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

    qRegisterMetaType<QList<qlonglong>>("QList<qlonglong>");

    qRegisterMetaType<ChatList::Type>("ChatList::Type");

    qRegisterMetaType<QModelIndex>("QModelIndex");

    qmlRegisterType<Authorization>("MyComponent", 1, 0, "Authorization");
    qmlRegisterType<ChatList>("MyComponent", 1, 0, "ChatList");

    qmlRegisterType<ChatModel>("MyComponent", 1, 0, "ChatModel");
    qmlRegisterType<ChatFolderModel>("MyComponent", 1, 0, "ChatFolderModel");
    qmlRegisterType<CountryModel>("MyComponent", 1, 0, "CountryModel");
    qmlRegisterType<LanguagePackInfoModel>("MyComponent", 1, 0, "LanguagePackInfoModel");
    qmlRegisterType<MessageModel>("MyComponent", 1, 0, "MessageModel");

    qmlRegisterType<LottieAnimation>("MyComponent", 1, 0, "LottieAnimation");
    qmlRegisterType<QrCodeItem>("MyComponent", 1, 0, "QrCode");

    qmlRegisterUncreatableType<Chat>("MyComponent", 1, 0, "Chat", "Chat cannot be created from QML.");
    qmlRegisterUncreatableType<ChatInfo>("MyComponent", 1, 0, "ChatInfo", "ChatInfo cannot be created from QML.");
    qmlRegisterUncreatableType<ChatPosition>("MyComponent", 1, 0, "ChatPosition", "ChatPosition cannot be created from QML.");
    qmlRegisterUncreatableType<File>("MyComponent", 1, 0, "File", "File cannot be created from QML.");
    qmlRegisterUncreatableType<Message>("MyComponent", 1, 0, "Message", "Message cannot be created from QML.");

    qmlRegisterUncreatableType<MessageText>("MyComponent", 1, 0, "MessageText", "MessageText cannot be created from QML.");
    qmlRegisterUncreatableType<MessageAnimation>("MyComponent", 1, 0, "MessageAnimation", "MessageAnimation cannot be created from QML.");
    qmlRegisterUncreatableType<MessageAudio>("MyComponent", 1, 0, "MessageAudio", "MessageAudio cannot be created from QML.");
    qmlRegisterUncreatableType<MessageDocument>("MyComponent", 1, 0, "MessageDocument", "MessageDocument cannot be created from QML.");
    qmlRegisterUncreatableType<MessagePhoto>("MyComponent", 1, 0, "MessagePhoto", "MessagePhoto cannot be created from QML.");
    qmlRegisterUncreatableType<MessageSticker>("MyComponent", 1, 0, "MessageSticker", "MessageSticker cannot be created from QML.");
    qmlRegisterUncreatableType<MessageVideo>("MyComponent", 1, 0, "MessageVideo", "MessageVideo cannot be created from QML.");
    qmlRegisterUncreatableType<MessageVideoNote>("MyComponent", 1, 0, "MessageVideoNote", "MessageVideoNote cannot be created from QML.");
    qmlRegisterUncreatableType<MessageVoiceNote>("MyComponent", 1, 0, "MessageVoiceNote", "MessageVoiceNote cannot be created from QML.");
    qmlRegisterUncreatableType<MessageLocation>("MyComponent", 1, 0, "MessageLocation", "MessageLocation cannot be created from QML.");
    qmlRegisterUncreatableType<MessageVenue>("MyComponent", 1, 0, "MessageVenue", "MessageVenue cannot be created from QML.");
    qmlRegisterUncreatableType<MessageContact>("MyComponent", 1, 0, "MessageContact", "MessageContact cannot be created from QML.");
    qmlRegisterUncreatableType<MessageAnimatedEmoji>("MyComponent", 1, 0, "MessageAnimatedEmoji", "MessageAnimatedEmoji cannot be created from QML.");
    qmlRegisterUncreatableType<MessagePoll>("MyComponent", 1, 0, "MessagePoll", "MessagePoll cannot be created from QML.");
    qmlRegisterUncreatableType<MessageInvoice>("MyComponent", 1, 0, "MessageInvoice", "MessageInvoice cannot be created from QML.");
    qmlRegisterUncreatableType<MessageCall>("MyComponent", 1, 0, "MessageCall", "MessageCall cannot be created from QML.");
    qmlRegisterUncreatableType<MessageService>("MyComponent", 1, 0, "MessageService", "MessageService cannot be created from QML.");

    QDeclarativeView viewer;

    Application application;
    Utils utils;

    Translator translator;
    app.installTranslator(&translator);

    viewer.rootContext()->setContextProperty("app", &application);
    viewer.rootContext()->setContextProperty("settings", &Settings::instance());
    viewer.rootContext()->setContextProperty("utils", &utils);

    viewer.rootContext()->setContextProperty("AppVersion", AppVersion);

    viewer.engine()->addImageProvider("chatPhoto", new ChatPhotoProvider);

    QObject::connect(viewer.engine(), SIGNAL(quit()), &viewer, SLOT(close()));

    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showFullScreen();

    return app.exec();
}
