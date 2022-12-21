#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QPainterPath>
#include <QDebug>
#include <QTcpSocket>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QNetworkSession>
#include <QMainWindow>
#include <track.h>
#include <network.h>
#include <QTableWidget>
#include <QDockWidget>
#include <QTableWidgetItem>
class ServerAttributes;
class PlayerAttributes;
class Track;

class MainWindow: public QMainWindow{
    Q_OBJECT
    friend class ServerAttributes;
    friend class PlayerAttributes;
    friend class Track;
public:
    MainWindow();
    ~MainWindow();
private slots:
    void openPlayerAttributes();
    void openServerAttributes();
    void connectToServer();
    void serverDisconnect();
    void sendCoordsToServer();
    void sendToPainter();
    void sendMapData();
    void sendPicturesToGame();
    void fillTable();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    Network *network;
    ServerAttributes *attrWindow;
    PlayerAttributes *playerWindow;
    Track *gameWindow;
    void readSettings();
    QDockWidget* dockWidget;
    QTableWidget* tableWidget;
};

class ServerAttributes:public  QWidget{
    Q_OBJECT
    friend class MainWindow;
protected:
    void closeEvent(QCloseEvent *event) override;
public:
    explicit ServerAttributes(QWidget *parent = 0);
private:
    QString serverIp;
    QString serverPort;
    QLineEdit *serverLineEdit = nullptr;
    QLineEdit *portLineEdit = nullptr;
    QPushButton *confirmButton = nullptr;
    QString getServetIp(){
        return serverIp;
    }
    QString getPort(){
        return serverPort;
    }
private slots:
    void dataSaved();
};

class PlayerAttributes: public QWidget{
    Q_OBJECT
    friend class MainWindow;
protected:
    void closeEvent(QCloseEvent *event) override;
public:
    explicit PlayerAttributes(QWidget *parent = 0);
private:
    QString nickname;
    QLineEdit *nicknameEdit = nullptr;
    QPushButton *confirmButton = nullptr;
    QString getNickname(){
        return nickname;
    }

private slots:
    void saveNickname();
};

#endif // CLIENT_H
