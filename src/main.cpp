#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeComponent>
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

#if defined(MEEGO_EDITION_HARMATTAN)
#    include <MDeclarativeCache>
#endif

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#ifdef MEEGO_EDITION_HARMATTAN
    QScopedPointer<QApplication> app(MDeclarativeCache::qApplication(argc, argv));

    QScopedPointer<QDeclarativeView> viewer(MDeclarativeCache::qDeclarativeView());
#else
    QScopedPointer<QApplication> app(new QApplication(argc, argv));

    QScopedPointer<QDeclarativeView> viewer(new QDeclarativeView);
#endif

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

    QScopedPointer<ChatModel> chatModel(new ChatModel);
    QScopedPointer<ChatFilterModel> chatFilterModel(new ChatFilterModel);
    QScopedPointer<CountryModel> countryModel(new CountryModel);
    QScopedPointer<MessageModel> messageModel(new MessageModel);

    QScopedPointer<Utils> utils(new Utils);

    qRegisterMetaType<TdApi::AuthorizationState>("TdApi::AuthorizationState");
    qRegisterMetaType<TdApi::ChatList>("TdApi::ChatList");

    qmlRegisterUncreatableType<TdApi>("com.strawberry.meegram", 0, 1, "TdApi", "TdApi should not be created in QML");

    viewer->rootContext()->setContextProperty("tdapi", &TdApi::getInstance());
    viewer->rootContext()->setContextProperty("myChatModel", chatModel.data());
    viewer->rootContext()->setContextProperty("myChatFilterModel", chatFilterModel.data());
    viewer->rootContext()->setContextProperty("myCountryModel", countryModel.data());
    viewer->rootContext()->setContextProperty("myMessageModel", messageModel.data());

    viewer->engine()->addImageProvider("telegram", new TdImageProvider);

    viewer->rootContext()->setContextProperty("Utils", utils.data());

    viewer->rootContext()->setContextProperty("AppVersion", AppVersion);

    QObject::connect(app.data(), SIGNAL(aboutToQuit()), &TdApi::getInstance(), SLOT(close()));
    QObject::connect(viewer->engine(), SIGNAL(quit()), viewer.data(), SLOT(close()));

    viewer->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer->setSource(QUrl("qrc:/qml/main.qml"));

    TdApi::getInstance().initialize();
    viewer->showFullScreen();

    return app->exec();
}
