#include "server.h"
#include <QDataStream>
#include <QWidget>
#include <QTableWidget>
#include <QFile>
#include <QSslKey>
#include <QSslConfiguration>
#include <QMessageBox>
#include <QtNetwork>
#include <QAbstractSocket>
#include <string>
#include <sstream>
#include <QMessageBox>

Server::Server(QObject *parent): QTcpServer(parent) {
    parseXml();
    startGame = false;
    start = QTime::currentTime();
    win = new QWidget();
    tableWidget = new QTableWidget(win);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels(QString("Client Status").split(" "));
    tableWidget->setFixedHeight(900);
    tableWidget->setFixedWidth(1200);
    QTimer* timer = new QTimer;
    connect(timer, &QTimer::timeout, this, &Server::sendDataAboutPlayers);
    timer->start(50);
    if (this->listen(QHostAddress::Any, 2323)) {
        qDebug() << "start";
    } else {
        qDebug() << "error";
    }
    window();
}

QByteArray type_cast2(qint32 number){
    QByteArray tmp;
    QDataStream data(&tmp, QIODevice::ReadWrite);
    data << number;
    return tmp;
}



void Server::carCollision(){
    for(int i = 0; i < sockets.size();i++)
    {
        for(int j=0; j < sockets.size();j++)
        {
            int dx=0, dy=0;
            while (dx*dx + dy*dy < 4* 22*22)
             {
               users[sockets[i]].second[0] += dx/10.0;
               users[sockets[i]].second[1] += dy/10.0;
               users[sockets[j]].second[0] -= dx/10.0;
               users[sockets[j]].second[1] -= dy/10.0;
               dx = users[sockets[i]].second[0] - users[sockets[j]].second[0];
               dy = users[sockets[i]].second[1] - users[sockets[j]].second[1];
               if (!dx && !dy)
               {
                   break;
               }
             }
        }
    }
}




void Server::sendDataAboutPlayers(){
    int command = 210;
    QByteArray dataPlayers;
    dataPlayers.clear();
    //qDebug() << users;
    int countPlayers = sockets.size();
    dataPlayers.append(reinterpret_cast<char*> (&command), 2);
    dataPlayers.append(reinterpret_cast<char*> (&countPlayers), 2);
    carCollision();
    for (int i = 0; i < sockets.size(); i++){
        short len = users[sockets[i]].first.size();
        dataPlayers.append(reinterpret_cast<char*> (&len), 2);
        dataPlayers.append(users[sockets[i]].first.toStdString().c_str(), len);
        users[sockets[i]].second[0] += users[sockets[i]].second[2];
        users[sockets[i]].second[1] += users[sockets[i]].second[3];
        //qDebug () << users[sockets[i]].second[0] << users[sockets[i]].second[1] << users[sockets[i]].second[2] << users[sockets[i]].second[3];
        float x = users[sockets[i]].second[0];
        float y = users[sockets[i]].second[1];
        float vX = users[sockets[i]].second[2];
        float vY = users[sockets[i]].second[3];

        dataPlayers.append(reinterpret_cast<char*> (&x), 4);
        dataPlayers.append(reinterpret_cast<char*> (&y), 4);
        dataPlayers.append(reinterpret_cast<char*> (&vX), 4);
        dataPlayers.append(reinterpret_cast<char*> (&vY), 4);
    }
    for (int i = 0; i < sockets.size(); i++){
        if (connections[i]){
            sockets[i]->write(type_cast2(dataPlayers.size()));
            sockets[i]->write(dataPlayers);
            sockets[i]->flush();
        }
    }
}

void Server::sendDataAboutPlayersStart(){
    int command = 210;
    QByteArray dataPlayers;
    dataPlayers.clear();
    //qDebug() << users;
    int countPlayers = sockets.size();
    dataPlayers.append(reinterpret_cast<char*> (&command), 2);
    dataPlayers.append(reinterpret_cast<char*> (&countPlayers), 2);
    //carCollision();
    for (int i = 0; i < sockets.size(); i++){
        short len = users[sockets[i]].first.size();
        dataPlayers.append(reinterpret_cast<char*> (&len), 2);
        dataPlayers.append(users[sockets[i]].first.toStdString().c_str(), len);
        users[sockets[i]].second[0] += users[sockets[i]].second[2];
        users[sockets[i]].second[1] += users[sockets[i]].second[3];
        //qDebug () << users[sockets[i]].second[0] << users[sockets[i]].second[1] << users[sockets[i]].second[2] << users[sockets[i]].second[3];
        float x = users[sockets[i]].second[0];
        float y = users[sockets[i]].second[1];
        float vX = users[sockets[i]].second[2];
        float vY = users[sockets[i]].second[3];

        dataPlayers.append(reinterpret_cast<char*> (&x), 4);
        dataPlayers.append(reinterpret_cast<char*> (&y), 4);
        dataPlayers.append(reinterpret_cast<char*> (&vX), 4);
        dataPlayers.append(reinterpret_cast<char*> (&vY), 4);
    }
    for (int i = 0; i < sockets.size(); i++){
        sockets[i]->write(type_cast2(dataPlayers.size()));
        sockets[i]->write(dataPlayers);
        //sockets[i]->waitForBytesWritten(3000);
        sockets[i]->flush();
    }
}

void Server::changeJournal(){
    tableWidget->setRowCount(sockets.size());
    for (int i = 0; i < sockets.size(); i++){
        tableWidget->setItem(i, 0, new QTableWidgetItem(users[sockets[i]].first));
        tableWidget->setItem(i, 1, new QTableWidgetItem("connected"));
    }
    //qDebug () << users;
}

void Server::incomingConnection(qintptr descriptor) {
    socket = new QTcpSocket;
    if (this->isListening()) {
        socket->setSocketDescriptor(descriptor);
        connect(socket, &QTcpSocket::readyRead, this, &Server::tcpReady);
        sockets.push_back(socket);
        times.push_back(0);
        connections.push_back(false);
        QByteArray *buffer = new QByteArray;
        qint32 *s = new qint32(0);
        buffers.insert(socket, buffer);
        sizes.insert(socket, s);
        qDebug() << "client connected" << descriptor;

    } else {
        qDebug() << "Connection error";
    }
}

qint32 type_cast(QByteArray buf){
    qint32 tmp;
    QDataStream data(&buf, QIODevice::ReadWrite);
    data >> tmp;
    return tmp;
}


void Server::getRequest(QTcpSocket* tmpSocket, int user_id){

    QString hexStr = data.toHex();
    bool ok;
    //qDebug() << hexStr;
    QString id = QString(hexStr[2]) + QString(hexStr[3]) + QString(hexStr[0]) + QString(hexStr[1]);

    int command = QString::number(id.toInt(&ok, 16)).toInt();
    //qDebug () << "Request: " << tmpSocket << user_id << command;
    if (command == 333){
        //qDebug () << user_id << sockets.size();
        //qDebug () << users;
        sendDataAboutPlayersStart();
        if (sockets.size() == 1){
            startGame = true;

            drawStartCars = false;
            //qDebug () << start;
        }
        if (startGame)
            for (int i = 0; i < sockets.size(); i++){
                connections[i] = true;
        }

    } else if (command == 100){
        //qDebug () << "Connected";
        QString byte_len = QString(hexStr[6]) + QString(hexStr[7]) + QString(hexStr[4]) + QString(hexStr[5]);
        int len_nickname = QString::number(byte_len.toInt(&ok, 16)).toInt();

        QByteArray dataNick;
        for (int i = 4; i < len_nickname*4; i++){
            dataNick.append(QChar(data[i]));
        }
        QString nickname = QString(dataNick);
        users[tmpSocket] = {nickname, {(float)(60.0*sockets.size()), 30.0, 0.0, 0.0}};
        changeJournal();
        SendToClient(tmpSocket, 101, user_id);
    }
    else if (command == 150){
        QString byte_len = QString(hexStr[6]) + QString(hexStr[7]) + QString(hexStr[4]) + QString(hexStr[5]);
        int len_nickname = QString::number(byte_len.toInt(&ok, 16)).toInt();
        QByteArray dataNick;
        for (int i = 4; i < len_nickname*4; i++){
            dataNick.append(QChar(data[i]));
        }
        QString nickname = QString(dataNick);
        //qDebug () << "Disconnected id" << user_id;
        sockets.erase(sockets.begin()+user_id);
        connections.erase(connections.begin()+user_id);
        buffers.erase(buffers.begin()+user_id);
        times.erase(times.begin()+user_id);
        sizes.erase(sizes.begin()+user_id);
        users.remove(tmpSocket);
        //qDebug() << nickname;
        //qDebug() << connections;
        changeJournal();
        SendToClient(tmpSocket, 151, user_id);
    }
    else if (command == 300){
        QString res = data.toHex();
        QString x;
        QString y;
        QString speed_x;
        QString speed_y;

        int tmp = 11;
        while (tmp > 3){
            x.append(res[tmp-1]);
            x.append(res[tmp]);
            tmp -= 2;
        }
        tmp = 19;
        while (tmp > 11){
            y.append(res[tmp-1]);
            y.append(res[tmp]);
            tmp -= 2;
        }
        tmp = 27;
        while (tmp > 19){
            speed_x.append(res[tmp-1]);
            speed_x.append(res[tmp]);
            tmp -= 2;
        }
        tmp = 35;
        while (tmp > 27){
            speed_y.append(res[tmp-1]);
            speed_y.append(res[tmp]);
            tmp -= 2;
        }
        union ulf{
            unsigned long ul;
            float f;
        };

        ulf u;
        std::string x_std = x.toStdString();
        std::string y_std = y.toStdString();
        std::string speed_x_std = speed_x.toStdString();
        std::string speed_y_std = speed_y.toStdString();
        std::stringstream ss1(x_std);
        ss1 >> std::hex >> u.ul;
        float coordX = u.f;

        std::stringstream ss2(y_std);
        ss2 >> std::hex >> u.ul;
        float coordY = u.f;

        std::stringstream ss3(speed_x_std);
        ss3 >> std::hex >> u.ul;
        float speedX = u.f;
        std::stringstream ss4(speed_y_std);
        ss4 >> std::hex >> u.ul;
        float speedY = u.f;
        if (startGame)
            users[tmpSocket].second = {coordX, coordY, speedX, speedY};
        //qDebug() << "data from client" <<coordX << coordY << speedX << speedY;
        SendToClient(tmpSocket, 301, user_id);
    } else if (command == 200){
        //qDebug () << 200;
        SendToClient(tmpSocket, 220, user_id);
    }
    else if (command == 350){
        QByteArray dataPlayers;
        dataPlayers.clear();
        int cmd = 351;
        //qDebug () << 350;
        dataPlayers.append(reinterpret_cast<char*>(&cmd), 2);
        int countPlayers = sockets.size();
        dataPlayers.append(reinterpret_cast<char*> (&countPlayers), 2);
        //qDebug () << sockets;
        for (int i = 0; i < sockets.size(); i++){
            QString pngName = "car"+QString::number(i%5)+".png";
            QString nickname = users[sockets[i]].first;
            //qDebug() << "Send png" << pngName;
            short lenFileName = pngName.size();
            short nicknameLen = nickname.size();
            dataPlayers.append(reinterpret_cast<char*>(&nicknameLen), 2);
            dataPlayers.append(nickname.toStdString().c_str(),nicknameLen);
            dataPlayers.append(reinterpret_cast<char*>(&lenFileName), 2);
            dataPlayers.append(pngName.toStdString().c_str(),lenFileName);
        }
        qDebug () << dataPlayers;
        //qDebug () << user_id << sockets.size();

        for (int i = 0; i < sockets.size(); i++){
            //qDebug () << users[sockets[i]];
            sockets[i]->write(type_cast2(dataPlayers.size()));
            sockets[i]->write(dataPlayers);
            //sockets[i]->waitForBytesWritten(60000);
            sockets[i]->flush();
            //qDebug () << "SEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEND";
        }
//        QString pngName = "car0.png";
//        short lenFileName = pngName.size();
//        dataPlayers.append(reinterpret_cast<char*>(&lenFileName), 2);
//        dataPlayers.append(pngName.toStdString().c_str(),lenFileName);
//        tmpSocket->write(type_cast2(dataPlayers.size()));
//        tmpSocket->write(dataPlayers);
//        connections[user_id] = true;
    }
    //qDebug () << connections;
}

void Server::parseXml(){
    //qDebug () << "xml";
    int command = 220;
    dataMap.append(reinterpret_cast<char*> (&command), 2);
    QString fileName = "/mnt/c/Users/vovag/Documents/GameSOund/server/map/map.xml";
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)){
        qDebug () << "Not opened";
    } else {
        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&file);
        xmlReader.readNext();
        while (!xmlReader.atEnd()){
            if (xmlReader.isStartElement()){
                if (xmlReader.name() == "Length"){
                    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()){
                        if (attr.name().toString() == "n") {
                            int num = attr.value().toString().toInt();
                            //qDebug () << num;
                            dataMap.append(reinterpret_cast<char*> (&num), 2);
                        }
                    }
                }
                if (xmlReader.name() == "Path"){
                    int type;
                    float x,y,w,h,ang;
                    foreach(const QXmlStreamAttribute &attr, xmlReader.attributes()) {
                        if (attr.name().toString() == "num") {
                            int num = attr.value().toString().toInt();
                            dataMap.append(reinterpret_cast<char*> (&num), 2);
                        }
                        if (attr.name().toString() == "type") {
                            type = attr.value().toString().toInt();
                            dataMap.append(reinterpret_cast<char*> (&type), 2);

                        }
                        if (attr.name().toString() == "x"){
                            x = attr.value().toString().toFloat();
                            dataMap.append(reinterpret_cast<char*> (&x), 4);

                        }
                        if (attr.name().toString() == "y"){
                            y = attr.value().toString().toFloat();
                            dataMap.append(reinterpret_cast<char*> (&y), 4);
                        }
                        if (attr.name().toString() == "d1"){
                            float d1 = attr.value().toString().toFloat();
                            dataMap.append(reinterpret_cast<char*> (&d1), 4);

                        }
                        if (attr.name().toString() == "d2"){
                            float d2 = attr.value().toString().toFloat();
                            dataMap.append(reinterpret_cast<char*> (&d2), 4);
                        }

                        if (attr.name().toString() == "w"){
                            w = attr.value().toString().toFloat();
                            dataMap.append(reinterpret_cast<char*> (&w), 4);
                        }
                        if (attr.name().toString() == "h"){
                            h = attr.value().toString().toFloat();
                            dataMap.append(reinterpret_cast<char*> (&h), 4);

                        }
                        if (attr.name().toString() == "ang"){
                            ang = attr.value().toString().toFloat();
                            dataMap.append(reinterpret_cast<char*> (&ang), 4);
                        }
                    }
                    if (type == 5){
                        controlPoints.push_back({x,y,w,h,ang});

                    }
                }
            }
            xmlReader.readNext();
        }
    }
    file.close();
    qDebug () << "Parsed" <<controlPoints;
    // () << dataMap;
}

void Server::tcpReady() {
    data.clear();
    int user_id;
    QTcpSocket *tmpsocket = static_cast<QTcpSocket*>(sender());
    for (int i = 0; i < sockets.size(); i++){
         if (sockets[i] == tmpsocket)
             user_id = i;
    }
    //qDebug () << "User id: " << user_id;
    QByteArray *buffer = buffers.value(tmpsocket);
    qint32 *s = sizes.value(tmpsocket);
    qint32 size = *s;
    while (tmpsocket->bytesAvailable() > 0){
        buffer->append(tmpsocket->readAll());
        while((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)){
            if (size == 0 && buffer->size() >= 4){
                size = type_cast(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            }
            if (size > 0 && buffer->size() >= size){
                data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;
            }
        }
    }
    getRequest(tmpsocket, user_id);
}



void Server::SendToClient(QTcpSocket* tmpSocket,int command, int user_id) {
    data.clear();
    if (command == 101){
        data.clear();
        int min = start.minute();
        int sec = start.second();
        data.append(reinterpret_cast<char*> (&command), 2);
        data.append(reinterpret_cast<char*> (&min), 2);
        data.append(reinterpret_cast<char*> (&sec), 2);
        tmpSocket->write(type_cast2(data.size()));
        tmpSocket->write(data);
        //tmpSocket->waitForBytesWritten(1000);
        tmpSocket->flush();

    } else if (command == 151){
        data.clear();
        data.append(reinterpret_cast<char*> (&command), 2);
        tmpSocket->write(type_cast2(data.size()));
        tmpSocket->write(data);
        //tmpSocket->waitForBytesWritten(1000);
        tmpSocket->flush();

    }
    else if (command == 301){
           data.clear();
           data.append(reinterpret_cast<char*> (&command), 2);
           tmpSocket->write(type_cast2(data.size()));
           tmpSocket->write(data);
           //tmpSocket->waitForBytesWritten(1000);
           tmpSocket->flush();

    } else if (command == 220){
        //qDebug () << 220;
        tmpSocket->write(type_cast2(dataMap.size()));
        tmpSocket->write(dataMap);
        //tmpSocket->waitForBytesWritten(3000);
        tmpSocket->flush();

    }
}

void Server::window() {
    win->setFixedHeight(900);
    win->setFixedWidth(1200);
    win->setWindowTitle("127.0.0.1 2323 0"); //ip, port, num

}
