#include <QApplication>
#include <QDebug>
#include <QDeclarativeComponent>
#include <QDeclarativeView>
#include <QFontDatabase>
#include <QTextCodec>

#include "Application.hpp"
#include "Common.hpp"
#include "DBusAdaptor.hpp"
#include "LottieAnimation.hpp"
#include "TdApi.hpp"
#include "TextFormatter.hpp"

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

    qmlRegisterType<LottieAnimation>("MyComponent", 1, 0, "LottieAnimation");
    qmlRegisterType<TextFormatter>("MyComponent", 1, 0, "TextFormatter");

    qmlRegisterUncreatableType<TdApi>("MyComponent", 1, 0, "TdApi", "TdApi should not be created in QML");

    QDeclarativeView viewer;

    Application application(&viewer);

    new DBusAdaptor(&app, &viewer);

    return app.exec();
}
