#include <QApplication>
#include <QDeclarativeComponent>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QFile>
#include <QFontDatabase>
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>

#include "ChatModel.hpp"
#include "Common.hpp"
#include "ImageProviders.hpp"
#include "MessageModel.hpp"
#include "SelectionModel.hpp"
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

    QString lang = QLocale::system().name();
    lang.truncate(2);  // ignore the country code

    QTranslator translator;
    if (QFile::exists(":/i18n/meegram_" + lang + ".qm"))
    {
        translator.load("meegram_" + lang, ":/i18n");
    }
    else
    {
        translator.load("meegram_en", ":/i18n");
    }
    app->installTranslator(&translator);

    qmlRegisterType<ChatModel>("com.strawberry.meegram", 1, 0, "ChatModel");
    qmlRegisterType<ChatFilterModel>("com.strawberry.meegram", 1, 0, "ChatFilterModel");
    qmlRegisterType<CountryModel>("com.strawberry.meegram", 1, 0, "CountryModel");
    qmlRegisterType<MessageModel>("com.strawberry.meegram", 1, 0, "MessageModel");

    qRegisterMetaType<TdApi::ChatList>("TdApi::ChatList");

    qmlRegisterUncreatableType<TdApi>("com.strawberry.meegram", 1, 0, "TdApi", "TdApi should not be created in QML");

    QScopedPointer<Utils> utils(new Utils);

    viewer->rootContext()->setContextProperty("AppVersion", AppVersion);
    viewer->rootContext()->setContextProperty("tdapi", &TdApi::getInstance());
    viewer->rootContext()->setContextProperty("Utils", utils.data());

    viewer->engine()->addImageProvider("telegram", new TdImageProvider);

    QObject::connect(app.data(), SIGNAL(aboutToQuit()), &TdApi::getInstance(), SLOT(close()));
    QObject::connect(viewer->engine(), SIGNAL(quit()), viewer.data(), SLOT(close()));

    viewer->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer->setSource(QUrl("qrc:/qml/main.qml"));

    TdApi::getInstance().initialize();
    viewer->showFullScreen();

    return app->exec();
}
