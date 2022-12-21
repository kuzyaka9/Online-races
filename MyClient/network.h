#ifndef NETWORK_H
#define NETWORK_H

#include <QTcpServer>
#include <QTcpSocket>
class Network: public QObject
{
    Q_OBJECT
public:
    Network();
    void connectTo(QString, int, QString);
    void disconnect();
    void condition();
    void move(float, float, float, float);
    void sendToServer(int);
    void getAnswer();
    QMap<int, QPair<QString, QVector<float>>> getUsers(){
        return users;
    };
    QVector<QPair<QPair<int, int>, QVector<float> > > mapCoords;
    QMap<QString, QString> pictures;
    QString picturePath;
    int min;
    int sec;
signals:
    void needToPaint();
    void sendMapCoords();
    void sendPictures();
public slots:
    void tcpReady();
private:
    QTcpSocket *socket;
    QByteArray data;
    QString ip;
    int port;
    QString nickname;
    float x, y, speed_x, speed_y;
    QHash<QTcpSocket*, QByteArray*> buffers;
    QHash<QTcpSocket*, qint32*> sizes;
    QMap<int, QPair<QString, QVector<float>>> users;


};

#endif // NETWORK_H
