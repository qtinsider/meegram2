#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QFile>
#include <QFontDatabase>
#include <QTextCodec>

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
    QScopedPointer<QApplication> app(new QApplication(argc, argv));
    QScopedPointer<QDeclarativeView> viewer(new QDeclarativeView);

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
    qmlRegisterType<Client>("com.strawberry.meegram", 1, 0, "Client");
    qmlRegisterType<Locale>("com.strawberry.meegram", 1, 0, "Locale");
    qmlRegisterType<Settings>("com.strawberry.meegram", 1, 0, "Settings");

    qmlRegisterType<TdManager>("com.strawberry.meegram", 1, 0, "TdManager");
    qmlRegisterType<NotificationManager>("com.strawberry.meegram", 1, 0, "NotificationManager");
    qmlRegisterType<StorageManager>("com.strawberry.meegram", 1, 0, "StorageManager");

    qmlRegisterType<ChatModel>("com.strawberry.meegram", 1, 0, "ChatModel");
    qmlRegisterType<ChatFilterModel>("com.strawberry.meegram", 1, 0, "ChatFilterModel");
    qmlRegisterType<CountryModel>("com.strawberry.meegram", 1, 0, "CountryModel");
    qmlRegisterType<MessageModel>("com.strawberry.meegram", 1, 0, "MessageModel");

    qmlRegisterType<LottieAnimation>("com.strawberry.meegram", 1, 0, "LottieAnimation");

    qmlRegisterUncreatableType<TdApi>("com.strawberry.meegram", 1, 0, "TdApi", "TdApi should not be created in QML");

    new DBusAdaptor(app.data(), viewer.data());

    viewer->rootContext()->setContextProperty("AppVersion", AppVersion);

    viewer->engine()->addImageProvider("chatPhoto", new ChatPhotoProvider);

    QObject::connect(viewer->engine(), SIGNAL(quit()), viewer.data(), SLOT(close()));

    viewer->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer->setSource(QUrl("qrc:/qml/main.qml"));

    viewer->showFullScreen();

    return app->exec();
}
