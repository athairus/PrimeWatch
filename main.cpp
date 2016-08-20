#include <QtGlobal>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "debughelper.inc"

#include "client.h"

int main( int argc, char *argv[] ) {

    // Use a custom message printing setup
    qSetMessagePattern( DebugHandler::messageFormat() );
    DebugHandler::install( DebugHandler::messageHandler );

    // Runs the main thread's event loop and handles messages from the windowing system
    QGuiApplication app( argc, argv );

    QThread::currentThread()->setObjectName( "Main/QML thread " );

    // The engine that runs our QML-based UI
    QQmlApplicationEngine engine;

    engine.addImportPath( app.applicationDirPath() + "/QML" );

    // Necessary to quit properly from QML
    QObject::connect( &engine, &QQmlApplicationEngine::quit, &app, &QGuiApplication::quit );

    // Set application metadata
    QGuiApplication::setApplicationDisplayName( "PrimeWatch" );
    QGuiApplication::setApplicationName( "PrimeWatch" );
    QGuiApplication::setApplicationVersion( "0.0.0" );
    QGuiApplication::setOrganizationName( "athairus" );
    QGuiApplication::setOrganizationDomain( "athair.us" );

    // Instantiate the client
    Client client;
    engine.rootContext()->setContextProperty( "client", &client );

    // Load the root QML object and everything under it
    engine.load( QDir::toNativeSeparators( app.applicationDirPath() + "/QML/main.qml" ) );

    return app.exec();
}
