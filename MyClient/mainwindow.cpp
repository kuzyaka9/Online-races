#include "mainwindow.h"
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
#include <QApplication>
#include <QStyleHints>
#include <QGroupBox>
#include <QRect>
#include <QDesktopWidget>
#include <network.h>
#include <Qt>
#include <QTableWidget>
#include <QDockWidget>
#include <QTableWidgetItem>


MainWindow::MainWindow(): QMainWindow()
{
    dockWidget = new QDockWidget("Time");
    dockWidget->setFeatures(QDockWidget::DockWidgetMovable);
    //dockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    tableWidget = new QTableWidget;
    dockWidget->setWidget(tableWidget);
    tableWidget->setMaximumSize(400,400);
    addDockWidget(Qt::RightDockWidgetArea,dockWidget);

    attrWindow = new ServerAttributes;
    playerWindow = new PlayerAttributes;
    network = new Network;
    connect(network, SIGNAL(needToPaint()), this, SLOT(sendToPainter()));
    connect(network, SIGNAL(sendMapCoords()),this, SLOT(sendMapData()));
    connect(network, SIGNAL(sendPictures()), this, SLOT(sendPicturesToGame()));

    //connect(socket)
    QVBoxLayout *box = new QVBoxLayout;
    QMenuBar *bar = new QMenuBar();
    QMenu *menu1 = new QMenu("&Game");
    menu1->addAction(tr("&Connect"), this, SLOT(connectToServer()));
    menu1->addAction(tr("&Disonnect"), this, SLOT(serverDisconnect()));
    QMenu *menu2 = new QMenu("&Settings");
    QMenu *menu2_1 = new QMenu("&Network");
    QMenu *menu2_2 = new QMenu("&Player");
    bar->addMenu(menu1);
    bar->addMenu(menu2);
    menu2->addMenu(menu2_1);
    menu2->addMenu(menu2_2);
    menu2_1->addAction(tr("&Server"), this, SLOT(openServerAttributes()));
    menu2_2->addAction(tr("&Profile"), this, SLOT(openPlayerAttributes()));
    menu1->addAction(tr("&Exit"), this, SLOT(serverDisconnect()));
    this->setLayout(box);
    this->layout()->setMenuBar(bar);

    readSettings();
}

void MainWindow::sendPicturesToGame(){
    gameWindow->pictures = network->pictures;
//    qDebug () <<gameWindow->pictures;
//    qDebug () << network->getUsers();
//    gameWindow->updateField();
    gameWindow->picturePath = network->picturePath;
}

void MainWindow::sendMapData(){
    gameWindow->mapCoords = network->mapCoords;
}

void MainWindow::closeEvent(QCloseEvent *event){
    serverDisconnect();
}

void MainWindow::sendToPainter(){
    //qDebug() << "-----------------------------------------------------------";
    gameWindow->setUsers(network->getUsers());
    gameWindow->min = network->min;
    gameWindow->sec = network->sec;
    gameWindow->updateField();
}

void MainWindow::readSettings(){
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::serverDisconnect(){
    network->disconnect();
    gameWindow->close();
}

void MainWindow::fillTable(){
    if (gameWindow->flag){
        qDebug () << "Main" << gameWindow->times;
        tableWidget->setRowCount(gameWindow->times.size());
        tableWidget->setColumnCount(2);
        for (int i = 0; i < gameWindow->times.size(); i++){
            tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i+1)));
            tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(abs(gameWindow->times[i]))));
            dockWidget->setWidget(tableWidget);
            addDockWidget(Qt::RightDockWidgetArea,dockWidget);
        }
    }
}

void MainWindow::connectToServer(){
    gameWindow = new Track;
    network->connectTo(attrWindow->getServetIp(), attrWindow->getPort().toInt(), playerWindow->getNickname());
    connect(gameWindow, &Track::signal, this, &MainWindow::sendCoordsToServer);
    connect(gameWindow, &Track::disc, this, &MainWindow::serverDisconnect);
    connect(gameWindow, &Track::table,this, &MainWindow::fillTable);

    gameWindow->setNickname(playerWindow->getNickname());
    gameWindow->setCar();
    gameWindow->show();
}

void MainWindow::sendCoordsToServer(){
    if (gameWindow->getDirection() == 0)
        network->move(gameWindow->getCoordX(), gameWindow->getCoordY(), gameWindow->getSpeedX(), -gameWindow->getSpeedY());
    else if (gameWindow->getDirection() == 1)
        network->move(gameWindow->getCoordX(), gameWindow->getCoordY(), gameWindow->getSpeedX(), -gameWindow->getSpeedY());
    else if (gameWindow->getDirection() == 2)
        network->move(gameWindow->getCoordX(), gameWindow->getCoordY(), gameWindow->getSpeedX(), -gameWindow->getSpeedY());
    else if (gameWindow->getDirection() == 3)
        network->move(gameWindow->getCoordX(), gameWindow->getCoordY(), -gameWindow->getSpeedX(), gameWindow->getSpeedY());
    else if (gameWindow->getDirection() == 4)
        network->move(gameWindow->getCoordX(), gameWindow->getCoordY(), -gameWindow->getSpeedX(), gameWindow->getSpeedY());
    else if (gameWindow->getDirection() == 5)
        network->move(gameWindow->getCoordX(), gameWindow->getCoordY(), -gameWindow->getSpeedX(), gameWindow->getSpeedY());

}

void MainWindow::openServerAttributes(){
   attrWindow->show();
}

void MainWindow::openPlayerAttributes(){
    playerWindow->show();
}


MainWindow::~MainWindow(){

}

ServerAttributes::ServerAttributes(QWidget *parent): QWidget (parent){
    this->setWindowTitle("Server Attributes");

    serverLineEdit = new QLineEdit("127.0.0.1");
    portLineEdit = new QLineEdit("2323");
    confirmButton = new QPushButton("Save Data");
    portLineEdit->setValidator(new QIntValidator(1, 65535, this));
    auto hostLabel = new QLabel(tr("&Server name:"));
    hostLabel->setBuddy(serverLineEdit);
    auto portLabel = new QLabel(tr("&Server port:"));
    portLabel->setBuddy(portLineEdit);
    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(confirmButton,QDialogButtonBox::ActionRole);
    connect(confirmButton, &QAbstractButton::clicked, this, &QWidget::close);

    confirmButton->setDefault(true);
    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->addWidget(hostLabel, 0, 0);
    mainLayout->addWidget(portLabel, 1, 0);
    mainLayout->addWidget(serverLineEdit, 0, 1);
    mainLayout->addWidget(portLineEdit, 1, 1);
    mainLayout->addWidget(buttonBox);

    this->setLayout(mainLayout);
}

void ServerAttributes::closeEvent(QCloseEvent *event){
    dataSaved();
}

void ServerAttributes::dataSaved(){
    serverIp = serverLineEdit->text();
    serverPort = portLineEdit->text();
}

PlayerAttributes::PlayerAttributes(QWidget *parent): QWidget (parent){
    this->setWindowTitle("Player Attributes");
    nicknameEdit= new QLineEdit("Vova");
    confirmButton = new QPushButton("Save Data");

    auto nickLabel = new QLabel(tr("&Nickname:"));
    nickLabel->setBuddy(nicknameEdit);
    auto buttonBox = new QDialogButtonBox;
    buttonBox->addButton(confirmButton,QDialogButtonBox::ActionRole);
    connect(confirmButton, &QAbstractButton::clicked, this, &QWidget::close);

    confirmButton->setDefault(true);
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(nickLabel, 0, 0);
    mainLayout->addWidget(nicknameEdit, 0, 1);
    mainLayout->addWidget(buttonBox);
    this->setLayout(mainLayout);
}

void PlayerAttributes::closeEvent(QCloseEvent *event){
    saveNickname();
}

void PlayerAttributes::saveNickname(){
    nickname = nicknameEdit->text();
}



