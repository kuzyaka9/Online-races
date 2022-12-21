#ifndef TRACK_H
#define TRACK_H


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
#include <QDockWidget>
#include <QTableWidget>
#include <QMainWindow>
//class ServerAttributes;

class Track : public QWidget
{
    Q_OBJECT

public:
    Track();
    void setCar();

    float getCoordX();
    float getCoordY();
    float getSpeedX();
    float getSpeedY();
    int getDirection();
    void updateField();
    void drawCars(QPainter &);
    void setUsers(QMap<int, QPair<QString, QVector<float>>> tmp){
        users = tmp;
    }
    void setNickname(QString tmp){
        nickname = tmp;
    }
    QVector<QPair<QPair<int, int>, QVector<float> > > mapCoords;
    QMap<QString, QString> pictures;
    QString picturePath;
    int min;
    int sec;
    QVector<int> times;
    bool flag = false;
    bool f = true;
signals:
    void signal();
    void disc();
    void table();
public slots:
    void moveCar();
    void sendDataToServer();
    void speedUp();
    void speedDown();
    void checkPoints();
private:
    QVector<QVector<float>> pointsCoordss;
    QImage tmp;
    QPainter cp;
    QVector<QPainterPath> cars;
    QString nickname;
    QMap<int, QPair<QString, QVector<float>>> users;
    QPainterPath car;
    QPainterPath field;
    QPainterPath points;
    int width_road;
    int direction;
    int new_direction;

    bool prev;
    bool forward;
    bool back;
    bool right;
    bool left;
    bool speed_up;
    bool speed_down;
    bool checkSpace;
    bool checkShift;

    float delta_speed_down;
    float delta_speed_up;
    float speed;
    float alpha;
    float speed_x;
    float speed_y;
    float delta_speed_x;
    float delta_speed_y;
    float alpha_step;
    QList<QChar> pressed_buttons;
    void paintEvent(QPaintEvent *event);
    void startGame();
    void setField();
    void define_the_direction_of_moving();
    void drawField(QPainter &painter);
    void initialize_constant_of_moving();
    void checkControlPoints();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event);
    float x = 0;
    float y = 0;
    int center_x_window;
    int center_y_window;
    //char forward = ' ';
    QPainter* my_painter = nullptr;
    void keyReleaseEvent(QKeyEvent *event);
};

#endif // TRACK_H
