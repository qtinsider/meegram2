#include <QApplication>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QFile>
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>

#include "ChatModel.hpp"
#include "Common.hpp"
#include "ImageProviders.hpp"
#include "Lottie.hpp"
#include "MessageModel.hpp"
#include "Stores.hpp"
#include "TdApi.hpp"
#include "Utils.hpp"

#if defined(MEEGO_EDITION_HARMATTAN)
#    include <MDeclarativeCache>
#endif

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#ifdef MEEGO_EDITION_HARMATTAN
    QApplication *app = MDeclarativeCache::qApplication(argc, argv);

    QDeclarativeView *viewer = MDeclarativeCache::qDeclarativeView();
#else
    QApplication *app = new QApplication(argc, argv);

    QDeclarativeView *viewer = new QDeclarativeView();
#endif

    QCoreApplication::setApplicationName(AppName);
    QCoreApplication::setApplicationVersion(AppVersion);

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

    viewer->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    QScopedPointer<ChatModel> myChatModel(new ChatModel);
    QScopedPointer<MessageModel> myMessageModel(new MessageModel);

    QScopedPointer<Utils> utils(new Utils);

    qRegisterMetaType<TdApi::AuthorizationState>("TdApi::AuthorizationState");
    qRegisterMetaType<TdApi::ChatList>("TdApi::ChatList");

    qmlRegisterUncreatableType<TdApi>("com.strawberry.meegram", 1, 0, "TdApi", "TdApi should not be created in QML");

    viewer->rootContext()->setContextProperty("tdapi", &TdApi::getInstance());
    viewer->rootContext()->setContextProperty("myChatModel", myChatModel.data());
    viewer->rootContext()->setContextProperty("myMessageModel", myMessageModel.data());

    viewer->engine()->addImageProvider("telegram", new TdImageProvider);

    viewer->rootContext()->setContextProperty("BasicGroupStore", TdApi::getInstance().basicGroupStore);
    viewer->rootContext()->setContextProperty("ChatStore", TdApi::getInstance().chatStore);
    viewer->rootContext()->setContextProperty("FileStore", TdApi::getInstance().fileStore);
    viewer->rootContext()->setContextProperty("OptionStore", TdApi::getInstance().optionStore);
    viewer->rootContext()->setContextProperty("SupergroupStore", TdApi::getInstance().supergroupStore);
    viewer->rootContext()->setContextProperty("UserStore", TdApi::getInstance().userStore);

    viewer->rootContext()->setContextProperty("Utils", utils.data());

    viewer->rootContext()->setContextProperty("AppVersion", AppVersion);

    QObject::connect(viewer->engine(), SIGNAL(quit()), viewer, SLOT(close()));
    QObject::connect(app, SIGNAL(aboutToQuit()), &TdApi::getInstance(), SLOT(close()));

    viewer->setSource(QUrl("qrc:/qml/main.qml"));

    TdApi::getInstance().initialize();
    viewer->showFullScreen();

    return app->exec();
}
