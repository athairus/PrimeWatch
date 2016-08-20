#include "client.h"

#include <QtGlobal>
#include <QDebug>
#include <QtEndian>

#if defined( Q_OS_WIN )
#include <Winsock2.h>
#else
#include <netinet/in.h>
#endif

Client::Client( QObject *parent ) : QObject( parent ) {
    typedef void ( QAbstractSocket::*QAbstractSocketErrorSignal )( QAbstractSocket::SocketError );

    connect( &socket, static_cast<QAbstractSocketErrorSignal>( &QAbstractSocket::error ),
    this, [ & ]( QAbstractSocket::SocketError socketError ) {
        qWarning() << socketError;
        status = socket.errorString();
        emit statusChanged();
    } );

    emit statusChanged();

    connect( &socket, &QAbstractSocket::disconnected, this, [ = ]() {
        qDebug() << "Got disconnect signal";
        emit disconnected();
    } );

    connect( &socket, &QAbstractSocket::stateChanged, this, [ & ]( QAbstractSocket::SocketState newState ) {
        if( newState == QAbstractSocket::UnconnectedState ) {
            emit disconnected();
        }

        if( newState == QAbstractSocket::ConnectedState ) {
            emit connected();
        }

        qDebug() << newState;
    } );

    connect( &socket, &QIODevice::readyRead, this, &Client::readData );
}

void Client::connectToServer( QString ip ) {
    socket.connectToHost( ip, 43673 );
}

void Client::disconnectFromServer() {
    socket.disconnectFromHost();
}

void Client::abortConnection() {
    socket.abort();
}

void Client::readData() {
    // Check packet size
    if( socket.bytesAvailable() % PACKET_SIZE != 0 ) {
        qWarning().nospace() << "Buffer misalignment! Bytes available: " << socket.bytesAvailable();
        qWarning() << "sizeof packet:" << sizeof( PrimeMemoryDump );
    }

    // Read packets until buffer is empty
    while( socket.readLine( reinterpret_cast<char *>( &data ), PACKET_SIZE ) > 0 );

    // Make sure the data is valid
    // FIXME: Apparently all the packets are of type 0x0
    //    if( data.type != PACKET_TYPE_GAME_DATA ) {
    //        qWarning().nospace() << "Invalid packet, type = 0x" << hex << data.type;
    //        abortConnection();
    //        return;
    //    }

    // Correct the byte order
    data.gameid = qFromBigEndian( data.gameid );
    data.makerid = qFromBigEndian( data.makerid );

    for( int i = 0; i < 3; i++ ) {
        data.speed[ i ] = ntohf( *reinterpret_cast<quint32 *>( &data.speed[ i ] ) );
        data.pos[ i ] = ntohf( *reinterpret_cast<quint32 *>( &data.pos[ i ] ) );
    }

    data.room = qFromBigEndian( data.room );
    data.health = ntohf( *reinterpret_cast<quint32 *>( &data.health ) );

    for( int i = 0; i < INVENTORY_SIZE; i++ ) {
        data.inventory[ i ] = ntohf( *reinterpret_cast<quint32 *>( &data.inventory[ i ] ) );
        data.inventory[ i + INVENTORY_SIZE ] = ntohf( *reinterpret_cast<quint32 *>( &data.inventory[ i + INVENTORY_SIZE ] ) );
    }

    static bool bigEndianHost = ntohl( 0xAABBCCDD ) == 0xAABBCCDD;
    data.timer = ntohd( *reinterpret_cast<quint64 *>( &data.timer ) );
    emit dataChanged();
}

QString Client::getStatus() {
    return status;
}
