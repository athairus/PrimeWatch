#pragma once

#include <QObject>
#include <QTcpSocket>

#define INVENTORY_SIZE (0x29)
#define PACKET_SIZE 375

#define PACKET_TYPE_GAME_DATA (0x1)
#define PACKET_TYPE_RAW_DISC_READ (0x2)
#define PACKET_TYPE_RAW_DISC_INVALID (0xFF)

#pragma pack( push, 1 )
struct PrimeMemoryDump {
    // PACKET_TYPE_*
    quint8 type { 0 };

    quint32 gameid { 0 };
    quint16 makerid { 0 };
    float speed[ 3 ] { 0 };
    float pos[ 3 ] { 0 };
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
        Q_PROPERTY( int type READ getType NOTIFY dataChanged )
        Q_PROPERTY( QString gameid READ getGameid NOTIFY dataChanged )
        Q_PROPERTY( QString makerid READ getMakerid NOTIFY dataChanged )
        Q_PROPERTY( float speedX READ getSpeedX NOTIFY dataChanged )
        Q_PROPERTY( float speedY READ getSpeedY NOTIFY dataChanged )
        Q_PROPERTY( float speedZ READ getSpeedZ NOTIFY dataChanged )
        Q_PROPERTY( float posX READ getPosX NOTIFY dataChanged )
        Q_PROPERTY( float posY READ getPosY NOTIFY dataChanged )
        Q_PROPERTY( float posZ READ getPosZ NOTIFY dataChanged )
        Q_PROPERTY( int room READ getRoom NOTIFY dataChanged )
        Q_PROPERTY( float health READ getHealth NOTIFY dataChanged )
        // TODO: Q_PROPERTY( int inventory MEMBER data.type NOTIFY dataChanged )
        Q_PROPERTY( float timer READ getTimer NOTIFY dataChanged )

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

        int getType() { return data.type; }
        QString getGameid() { return "0x" + QString::number( data.gameid, 16 ); }
        QString getMakerid() { return "0x" + QString::number( data.makerid, 16 ); }
        float getSpeedX() { return data.speed[ 0 ]; }
        float getSpeedY() { return data.speed[ 1 ]; }
        float getSpeedZ() { return data.speed[ 2 ]; }
        float getPosX() { return data.pos[ 0 ]; }
        float getPosY() { return data.pos[ 1 ]; }
        float getPosZ() { return data.pos[ 2 ]; }
        int getRoom() { return data.room; }
        float getHealth() { return data.health; }
        float getTimer() { return data.timer; }
};
