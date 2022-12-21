#ifndef SERVER_H
#define SERVER_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QTableWidget>
#include <QWidget>
#include <QSslSocket>
#include <QMap>
#include <QTime>

QT_BEGIN_NAMESPACE
class QSessionManager;
QT_END_NAMESPACE

class Server : public QTcpServer {
    Q_OBJECT

public:
    explicit Server(QObject *parent = 0);
    //~Server();
    QTcpSocket *socket;
    //QSslSocket socket;
    QWidget *win;

private:
    void SendToClient(QTcpSocket*, int, int);
    void window();
    void getRequest(QTcpSocket*, int);
    void changeJournal();
    void carCollision();
    void parseXml();
    QVector<QTcpSocket*> sockets;
    QVector<bool> connections;
    QByteArray data;
    QByteArray dataMap;
    QTableWidget *tableWidget;
    //int user_id;
    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;
    QMap<QTcpSocket*, QPair<QString, QVector<float> > > users;
    QVector<QVector<float>> controlPoints;
    QVector<int> times;
    QTime start;
    bool startGame;
    bool drawStartCars = true;
    void sendDataAboutPlayersStart();
    QByteArray dataTime;


public slots:
    void incomingConnection(qintptr dsescriptor);
    void tcpReady();
    void sendDataAboutPlayers();
//    void encrypted();
//    void sslError(QList<QSslError> errors);
//    bool start_listen(int port);


};

#endif // SERVER_H
