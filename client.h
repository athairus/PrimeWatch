#pragma once

#include <cmath>

#include <QDateTime>
#include <QObject>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QtEndian>

#define INVENTORY_SIZE (0x29)
#define PACKET_TYPE_GAME_DATA_SIZE sizeof( PrimeMemoryDump )
#define PACKET_TYPE_RAW_DISC_READ_SIZE 17

#define PACKET_TYPE_GAME_DATA (1)
#define PACKET_TYPE_RAW_DISC_READ (2)
#define PACKET_TYPE_RAW_DISC_INVALID (-1)

#define PRIME_1_GAMEID (0x474D3845)
#define PRIME_1_MAKERID (0x3031)

#pragma pack( push, 1 )
struct PrimeMemoryDump {
    // PACKET_TYPE_*
    quint8 type { 0 };

    quint32 gameid { 0 };
    quint16 makerid { 0 };
    float speed[ 3 ] { 0 };
    float pos[ 3 ] { 0 };
    quint32 worldID { 0 };
    quint32 worldStatus { 0 };
    quint32 room { 0 };
    float health { 0 };

    // (current, max) pairs
    // Types: https://github.com/Pwootage/prime-patcher/blob/master/src/main/resources/templates/mp1/Enums/Item.xml
    float inventory[ INVENTORY_SIZE * 2 ] { 0 };

    // Time in seconds
    double timer { 0 };
};
#pragma pack( pop )

class Client : public QObject {
        Q_OBJECT
        Q_PROPERTY( QString statusText READ getStatus NOTIFY statusChanged )
        Q_PROPERTY( quint8 type READ getType NOTIFY dataChanged )
        Q_PROPERTY( QString gameid READ getGameid NOTIFY dataChanged )
        Q_PROPERTY( QString makerid READ getMakerid NOTIFY dataChanged )
        Q_PROPERTY( float speedX READ getSpeedX NOTIFY dataChanged )
        Q_PROPERTY( float speedY READ getSpeedY NOTIFY dataChanged )
        Q_PROPERTY( float speedZ READ getSpeedZ NOTIFY dataChanged )
        Q_PROPERTY( float posX READ getPosX NOTIFY dataChanged )
        Q_PROPERTY( float posY READ getPosY NOTIFY dataChanged )
        Q_PROPERTY( float posZ READ getPosZ NOTIFY dataChanged )
        Q_PROPERTY( QString worldID READ getWorldID NOTIFY dataChanged )
        Q_PROPERTY( quint32 worldStatus READ getWorldStatus NOTIFY dataChanged )
        Q_PROPERTY( quint32 room READ getRoom NOTIFY dataChanged )
        Q_PROPERTY( float health READ getHealth NOTIFY dataChanged )
        // TODO: Q_PROPERTY( int inventory MEMBER data.type NOTIFY dataChanged )
        Q_PROPERTY( QString timer READ getTimer NOTIFY dataChanged )

    public:
        explicit Client( QObject *parent = 0 );

    signals:
        void statusChanged();
        void connected();
        void disconnected();
        void dataChanged();

    public slots:
        void connectToServer( QString ip );
        void disconnectFromServer();
        void abortConnection();
        void readData();

    private:
        QString getStatus();

        QString ip;
        QTcpSocket socket;
        QString status { "" };

        PrimeMemoryDump data;

        int getType() {
            return data.type;
        }
        QString getGameid() {
            quint32 gameIDLong = qToBigEndian( data.gameid );
            union {
                quint32 asLong;
                char asChar;
            } gameID = { gameIDLong };
            QByteArray str( &gameID.asChar, 4 );
            return QString::fromLatin1( str );
        }
        QString getMakerid() {
            quint32 makerIDLong = qToBigEndian( data.makerid );
            union {
                quint32 asLong;
                char asChar;
            } makerID = { makerIDLong };
            QByteArray str( &makerID.asChar, 2 );
            return QString::fromLatin1( str );
        }
        float getSpeedX() {
            return data.speed[ 0 ];
        }
        float getSpeedY() {
            return data.speed[ 1 ];
        }
        float getSpeedZ() {
            return data.speed[ 2 ];
        }
        float getPosX() {
            return data.pos[ 0 ];
        }
        float getPosY() {
            return data.pos[ 1 ];
        }
        float getPosZ() {
            return data.pos[ 2 ];
        }
        quint32 getRoom() {
            return data.room;
        }
        QString getWorldID() {
            return QString( "%1" ).arg( data.worldID, 8, 16, QChar( '0' ) ) + ".MLVL";
        }
        quint32 getWorldStatus() {
            return data.worldStatus;
        }
        float getHealth() {
            return data.health;
        }
        QString getTimer() {
            double integer, frac;
            frac = modf( data.timer, &integer );
            QString fracString = QString::number( frac, 'f', 2 );
            fracString = fracString.replace( "0.", "." );
            return QDateTime::fromTime_t( static_cast<uint>( data.timer ) ).toUTC().toString( "hh:mm:ss" ) + fracString;
        }
};
