#include "track.h"
#include <iostream>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <QtMath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QHostInfo>
#include <QNetworkConfigurationManager>
#include <QDialogButtonBox>
#include <QSettings>
#include <QGuiApplication>
#include <QStyleHints>
#include <QGroupBox>
#include <QApplication>
#include <QDesktopWidget>
#include <QtMultimedia/QMediaPlayer>
#include <QMediaPlaylist>
#include <QUrl>
#include <QTime>
#include <QHBoxLayout>
#include <QGraphicsScene>

Track::Track(){
    startGame();

}

void Track::checkControlPoints(){
    if (pointsCoordss.size() != 0){
        times.clear();
        flag = false;
        foreach (int key, users.keys()) {
           times.push_back(0);
        }
        //qDebug () << pointsCoordss;
        foreach (int key, users.keys()){
            if (users[key].second[0] >= pointsCoordss[0][0] && users[key].second[1] >= pointsCoordss[0][1] && users[key].second[1] <= pointsCoordss[0][1]+pointsCoordss[0][3] && users[key].second[0] <= pointsCoordss[0][0]+pointsCoordss[0][2] ||
                    users[key].second[0] >= pointsCoordss[1][0] && users[key].second[1] >= pointsCoordss[1][1] && users[key].second[1] <= pointsCoordss[1][1]+pointsCoordss[1][3] && users[key].second[0] <= pointsCoordss[1][0]+pointsCoordss[1][2]){
                QTime tmp = QTime::currentTime();
                qDebug () << tmp.minute() << min << tmp.second() << sec;
                times[key] = 60*(tmp.minute()-min)+sec-tmp.second();
                flag = true;
            }
        }
        emit table();
    }
}

void Track::checkPoints(){
    checkControlPoints();
}

void Track::closeEvent(QCloseEvent *event){
    emit disc();
}

void Track::startGame(){
    Qt::WindowFlags flags = Qt::Dialog | Qt::WindowMaximizeButtonHint| Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint;
    setWindowFlags(flags);
    //setWindowFlags (Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
    setFixedHeight(QApplication::desktop()->height());
    setFixedWidth(QApplication::desktop()->width()/2);

    width_road = 200;
    setField();
    initialize_constant_of_moving();
    QTimer* move_the_car = new QTimer();
    QTimer* timer = new QTimer();
    QTimer* startMoving = new QTimer();
    QTimer* speedLow = new QTimer();
    QTimer* checkBox = new QTimer();

    connect(checkBox, SIGNAL(timeout()), this, SLOT(checkPoints()));
    connect(move_the_car, SIGNAL(timeout()), this, SLOT(moveCar()));
    connect(timer, SIGNAL(timeout()), this, SLOT(sendDataToServer()));
    connect(startMoving, SIGNAL(timeout()), this, SLOT(speedUp()));
    connect(speedLow, SIGNAL(timeout()), this, SLOT(speedDown()));

    move_the_car->start(50);
    timer->start(50);
    startMoving->start(50);
    speedLow->start(125);
    checkBox->start(50);
}

void Track::speedDown(){
    if (speed > 0 && !forward && !back  && !checkSpace){
        speed -= delta_speed_up;
        //qDebug () << "Slow";
        //qDebug  () << speed;
        speed_x = qCos(qDegreesToRadians(alpha)) * speed;
        speed_y = qSin(qDegreesToRadians(alpha)) * speed;
        emit signal();
    }

}

void Track::speedUp(){
    if (speed < 5 && (forward || back) && !checkSpace){
        speed += delta_speed_up;
//        QMediaPlayer *player = new QMediaPlayer(this);
//        QMediaPlaylist *playlist = new QMediaPlaylist(player);
//        player->setPlaylist(playlist);
//        playlist->addMedia(QUrl::fromLocalFile("/mnt/C/Users/vovag/Dekstop/GameNoSOund/MyClient/images/soundCar.ogg"));
//        player->setVolume(50);
//        player->play();
    }
}

void Track::sendDataToServer(){
    emit signal();
}

void Track::updateField(){
    foreach (int key, users.keys()) {
        if (users[key].first == nickname){
            x = users[key].second[0];
            y = users[key].second[1];
            speed_x = users[key].second[2];
            speed_y = users[key].second[3];
            //qDebug () << users[key].second[0] << users[key].second[1] << users[key].second[2] << users[key].second[3];
        }
    }
    update();
}

void Track::drawCars(QPainter &painter){
    //qDebug () << pictures;
    //qDebug () << users;
    QString path = QCoreApplication::applicationDirPath();
    path.replace("build-GameSoundClient-Desktop-Debug", "MyClient/images/");
    //qDebug () << users;
    foreach ( int key, users.keys()) {
        tmp.load(path+"car"+QString::number(key)+".png");
        painter.resetTransform();
        //painter.translate(users[key].second[0], users[key].second[1]);
        QMatrix rot;
        rot.rotate(qRadiansToDegrees(atan2(users[key].second[3], users[key].second[2]))+90);
        QImage out = tmp.transformed(rot);
        //painter.rotate(qRadiansToDegrees(atan2(users[key].second[3], users[key].second[2])));
        //painter.translate(-20, -10);Y
        painter.drawImage(users[key].second[0], users[key].second[1], out);

    }
}

void Track::initialize_constant_of_moving(){
    speed = 0;
    alpha = 0;
    alpha_step = 5;
    speed_x = 0;
    speed_y = 0;
    x = 20;
    y = 20;

    speed_up = false;
    forward = false;
    back = false;
    right = false;
    left = false;
    prev = false;
    speed_down = false;
    checkSpace = false;
    checkShift = false;

    delta_speed_down = 0.05;
    delta_speed_up = 0.125;
}
void Track::setCar()
{
    //qDebug() << users.keys().size();
        QPainterPath car;
        car.addRoundedRect(0, 0, 40,  20, 3, 3);
        this->car = car;
        update();
}

void Track::setField()
{
//    QPainterPath path;

    qDebug () << mapCoords;
    for (int i = 0; i < mapCoords.size(); i++){
        if (mapCoords[i].first.second == 5){
            points.addRect(mapCoords[i].second[0],mapCoords[i].second[1],mapCoords[i].second[2],mapCoords[i].second[3]);
            pointsCoordss.push_back({mapCoords[i].second[0],mapCoords[i].second[1],mapCoords[i].second[2],mapCoords[i].second[3]});
            f =false;
        }
    }
    //qDebug () << pointsCoordss;
    //this->field.addPath(points);
    update();
}

void Track::drawField(QPainter &painter)
{
    if (f)
        setField();
    QRect rect(10,10,1890, 1590);
    QString path = QCoreApplication::applicationDirPath();
    path.replace("build-GameSoundClient-Desktop-Debug", "MyClient/images/");
    QImage fieldPng(path+"back.png");
    painter.drawImage(0, 0, fieldPng);
    painter.resetTransform();
    painter.drawPath(field);
}
void Track::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    my_painter  = &painter;
    painter.setRenderHint(QPainter::Antialiasing);
    painter.save();
    drawField(painter);
    drawCars(painter);
    painter.restore();

}


void Track::keyPressEvent(QKeyEvent *event)
{

    QString text = event->text();
    //qDebug () << event;
    if (event->key() == Qt::Key_Space){
        speed_down = true;
    }
    if (event->key() == Qt::Key_W && event->modifiers() == Qt::ShiftModifier){
        forward = true;
        back = false;
        speed_up = true;
    }
    if (event->key() == Qt::Key_A && event->modifiers() == Qt::ShiftModifier){
        right = false;
        left = true;
        speed_up = true;
    }
    if (event->key() == Qt::Key_S && event->modifiers() == Qt::ShiftModifier){
        back = true;
        forward = false;
        speed_up = true;

    }
    if (event->key() == Qt::Key_D && event->modifiers() == Qt::ShiftModifier){
        right = true;
        left = false;
        speed_up = true;

    }

    if (event->key() == Qt::Key_W){
        forward = true;
        back = false;
    }
    if (event->key() == Qt::Key_A){
        right = false;
        left = true;
    }
    if (event->key() == Qt::Key_S){
        back = true;
        forward = false;
    }
    if (event->key() == Qt::Key_D){
        right = true;
        left = false;
    }

}


void Track::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space){
        speed_down = false;
    }
    if (event->key() == Qt::Key_Shift){
        speed_up = false;
    }
    if (event->key() == Qt::Key_W){
        forward = false;
        prev = true;
    }
    if (event->key() == Qt::Key_A){
        left = false;
    }
    if (event->key() == Qt::Key_S){
        back = false;
        prev = false;
    }
    if (event->key() == Qt::Key_D){
        right = false;
    }
}

void Track::define_the_direction_of_moving(){

    if (forward && left)
        new_direction = 0;
    else if (forward && right)
        new_direction = 2;
    else if (back && left)
        new_direction = 3;
    else if (back && right)
        new_direction = 5;
    else if (forward)
        new_direction = 1;
    else  if (back)
        new_direction = 4;
    else if (prev && right)
        new_direction = 2;
    else if (!prev && left)
        new_direction = 3;
    else if (prev && left)
        new_direction = 0;
    else if (!prev && right)
        new_direction = 5;
    else{
        new_direction = 1000;
    }
}



void Track::moveCar(){
    speed_x = qCos(qDegreesToRadians(alpha)) * speed;
    speed_y = qSin(qDegreesToRadians(alpha)) * speed;
    if (speed_down && !checkSpace){
        speed /= 2;
        checkSpace = true;
    }
    else if (!speed_down && checkSpace){
        speed *= 2;
        checkSpace = false;
    }
    if (speed_up && !checkShift){
        speed *= 2;
        checkShift = true;
    }
    else if (!speed_up && checkShift){
        speed /= 2;
        checkShift = false;
    }

    switch(new_direction){
    case 0:
        x +=  speed_x;
        y -= speed_y;
        alpha += alpha_step;
        break;
    case 1:
        x +=  speed_x;
        y -= speed_y;
        break;
    case 2:
        x +=  speed_x;
        y -= speed_y;
        alpha -= alpha_step;
        break;
    case 3:
        x -=  speed_x;
        y += speed_y;
        alpha -= alpha_step;
        break;
    case 4:
        x -=  speed_x;
        y += speed_y;
        break;
    case 5:

        x -=  speed_x;
        y += speed_y;
        alpha += alpha_step;
        break;
    default:
        ;
    }
    define_the_direction_of_moving();
    if (new_direction != direction && new_direction != 1000){
        emit signal();
        direction = new_direction;
    }
    //update();
}

float Track::getCoordX(){
    return x;
}

float Track::getCoordY(){
    return y;
}

float Track::getSpeedX(){
    return speed_x;
}

float Track::getSpeedY(){
    return speed_y;
}

int Track::getDirection(){
    return new_direction;
}

