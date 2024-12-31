#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QFontDatabase>
#include <QModelIndex>
#include <QTextCodec>

#include "AppManager.hpp"
#include "Authorization.hpp"
#include "Chat.hpp"
#include "ChatManager.hpp"
#include "ChatPhotoProvider.hpp"
#include "ChatPosition.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "File.hpp"
#include "LanguagePackInfoModel.hpp"
#include "Localization.hpp"
#include "LottieAnimation.hpp"
#include "Message.hpp"
#include "MessageService.hpp"
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

    qRegisterMetaType<Chat::Type>("Chat::Type");

    qRegisterMetaType<QModelIndex>("QModelIndex");

    qmlRegisterType<LottieAnimation>("MyComponent", 1, 0, "LottieAnimation");
    qmlRegisterType<QrCodeItem>("MyComponent", 1, 0, "QrCode");

    qmlRegisterUncreatableType<Client>("MyComponent", 1, 0, "Client", "Client cannot be created from QML.");
    qmlRegisterUncreatableType<Authorization>("MyComponent", 1, 0, "Authorization", "Authorization cannot be created from QML.");
    qmlRegisterUncreatableType<Locale>("MyComponent", 1, 0, "Locale", "Locale cannot be created from QML.");
    qmlRegisterUncreatableType<Settings>("MyComponent", 1, 0, "Settings", "Settings cannot be created from QML.");
    qmlRegisterUncreatableType<ChatManager>("MyComponent", 1, 0, "ChatManager", "ChatManager cannot be created from QML.");
    qmlRegisterUncreatableType<StorageManager>("MyComponent", 1, 0, "StorageManager", "BasicGroup cannot be created from QML.");
    qmlRegisterUncreatableType<LanguagePackInfoModel>("MyComponent", 1, 0, "LanguagePackInfoModel", "LanguagePackInfoModel cannot be created from QML.");

    qmlRegisterUncreatableType<BasicGroup>("MyComponent", 1, 0, "BasicGroup", "BasicGroup cannot be created from QML.");
    qmlRegisterUncreatableType<Chat>("MyComponent", 1, 0, "Chat", "Chat cannot be created from QML.");
    qmlRegisterUncreatableType<ChatManager>("MyComponent", 1, 0, "ChatInfo", "ChatInfo cannot be created from QML.");
    qmlRegisterUncreatableType<ChatPosition>("MyComponent", 1, 0, "ChatPosition", "ChatPosition cannot be created from QML.");
    qmlRegisterUncreatableType<File>("MyComponent", 1, 0, "File", "File cannot be created from QML.");
    qmlRegisterUncreatableType<Message>("MyComponent", 1, 0, "Message", "Message cannot be created from QML.");
    qmlRegisterUncreatableType<Supergroup>("MyComponent", 1, 0, "Supergroup", "Supergroup cannot be created from QML.");
    qmlRegisterUncreatableType<SupergroupFullInfo>("MyComponent", 1, 0, "SupergroupFullInfo", "SupergroupFullInfo cannot be created from QML.");
    qmlRegisterUncreatableType<User>("MyComponent", 1, 0, "User", "User cannot be created from QML.");

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

    AppManager appManager;
    Utils utils;

    app.installTranslator(appManager.locale());

    viewer.rootContext()->setContextProperty("appManager", &appManager);
    viewer.rootContext()->setContextProperty("utils", &utils);

    viewer.rootContext()->setContextProperty("AppVersion", AppVersion);

    viewer.engine()->addImageProvider("chatPhoto", new ChatPhotoProvider);

    QObject::connect(viewer.engine(), SIGNAL(quit()), &viewer, SLOT(close()));

    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));
    viewer.showFullScreen();

    return app.exec();
}
