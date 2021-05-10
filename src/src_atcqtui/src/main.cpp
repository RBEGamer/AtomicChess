#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <stdio.h>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include "menumanager.h"
#include "SHARED/guicommunicator/guicommunicator.h"
int main(int argc, char *argv[])
{



  //  qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);
    qInfo() << argc;
    QCommandLineParser parser;
    parser.setApplicationDescription("use -platform webgl:port=1337 to start application in webgl mode with different control server interface");
    parser.addHelpOption();
    parser.addOption(QCommandLineOption("platform"));
    parser.process(app);
    const QStringList args = parser.positionalArguments();
    qInfo()<< args;
    QString iswebgl = parser.value("platform");
    if(iswebgl.contains("webgl")){
        qInfo()<< "WEBGL";
        guicommunicator::IS_PLATTFORM_WEB = true;
    }else{
        guicommunicator::IS_PLATTFORM_WEB = false;
    }



    QWindow window;
    window.setBaseSize(QSize(800,480));

    qmlRegisterType<MenuManager>("MenuManager",1,0,"MenuManager");

    QQuickView view;
    view.setHeight(480);
    view.setWidth(800);
    view.engine()->addImportPath("qrc:/qml/imports");
    view.setSource(QUrl("qrc:/qml/WINDOW.qml"));
    view.engine()->rootContext()->setContextProperty("app", &app);
    view.setResizeMode(QQuickView::SizeRootObjectToView); //SIZE WINDOW TO VIEW SIZE !!
    if(!view.errors().empty()){
           return -1;
    }





    view.show();
    //The following step is important, to make the c++ backend (menumanager.h) working
    //The instance of the mainmenu manager has no parent, but to access/search object in the qmlgraph
    //the menu manager instance need a parent. So this code finds the menumanager instance and assign the parent with the setparent funktion
    //the new parent is the window root object
    QObject *object = view.rootObject();
    qInfo() <<object->objectName();
    QObject *rect = object->findChild<QObject*>("mainmenu");
    if (rect){
        qInfo()<< rect->objectName();
           rect->setParent(object);
    }


    return app.exec();
}

