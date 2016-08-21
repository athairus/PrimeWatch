#include "client.h"

#include <QtGlobal>
#include <QDebug>

float ntohf( float val ) {
    Q_ASSERT( sizeof( float ) == sizeof( quint32 ) );

    union {
        float asFloat;
        quint32 asLong;
    } data = { val };

    data.asLong = qFromBigEndian<quint32>( data.asLong );
    return data.asFloat;
}

double ntohd( double val ) {
    Q_ASSERT( sizeof( double ) == sizeof( quint64 ) );

    union {
        double asDouble;
        struct {
            quint32 first;
            quint32 second;
        } asLongPair;
    } data = { val };

    quint32 firstBE = data.asLongPair.first;
    quint32 secondBE = data.asLongPair.second;
    data.asLongPair.first = qFromBigEndian<quint32>( secondBE );
    data.asLongPair.second = qFromBigEndian<quint32>( firstBE );
    return data.asDouble;
}

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

        if( newState == QAbstractSocket::BoundState ) {
            emit connected();
        }

        qDebug() << newState;
    } );

    connect( &socket, &QIODevice::readyRead, this, &Client::readData );

    //qDebug() << "sizeof( PACKET_TYPE_GAME_DATA ) =" << PACKET_TYPE_GAME_DATA_SIZE;
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
    // Read packets until buffer is empty
    while( socket.bytesAvailable() ) {
        // Grab the type byte
        char type;
        socket.peek( &type, 1 );

        // Parse packet according to its type
        switch( type ) {
            case PACKET_TYPE_GAME_DATA: {
                // Incomplete packet in the buffer, wait until more data comes in before parsing it
                if( socket.bytesAvailable() < PACKET_TYPE_GAME_DATA_SIZE ) {
                    return;
                }

                // Read the packet
                socket.read( reinterpret_cast<char *>( &data ), PACKET_TYPE_GAME_DATA_SIZE );

                // Correct the byte order
                {
                    data.gameid = qFromBigEndian( data.gameid );
                    data.makerid = qFromBigEndian( data.makerid );

                    for( int i = 0; i < 3; i++ ) {
                        data.speed[ i ] = ntohf( data.speed[ i ] );
                        data.pos[ i ] = ntohf( data.pos[ i ] );
                    }

                    data.worldID = qFromBigEndian( data.worldID );
                    data.worldStatus = qFromBigEndian( data.worldStatus );

                    data.room = qFromBigEndian( data.room );
                    data.health = ntohf( data.health );

                    for( int i = 0; i < INVENTORY_SIZE; i++ ) {
                        data.inventory[ i ] = ntohf( data.inventory[ i ] );
                        data.inventory[ i + INVENTORY_SIZE ] = ntohf( data.inventory[ i + INVENTORY_SIZE ] );
                    }

                    data.timer = ntohd( data.timer );
                }

                emit dataChanged();
                break;
            }

            // case PACKET_TYPE_RAW_DISC_READ: {
            //     break;
            // }

            // case PACKET_TYPE_RAW_DISC_INVALID: {
            //     break;
            // }

            default: {
                qDebug().nospace() << "Unknown packet with id 0x" << hex << static_cast<int>( type );
                qDebug() << socket.bytesAvailable() << "bytes available";
                //qDebug() << socket.readAll();
                return;
            }
        }
    }
}

QString Client::getStatus() {
    return status;
}
