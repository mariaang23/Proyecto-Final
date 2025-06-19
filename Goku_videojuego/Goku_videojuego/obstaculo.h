#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>


class obstaculo : public QObject
{
    Q_OBJECT
public:
    enum Tipo {Ave, Montania, Roca};
    obstaculo(QGraphicsScene *scene, Tipo tipo, int velocidad, QObject *parent = nullptr);
    ~obstaculo();

    void cargarImagenes();
    void iniciar(int x = -1, int y = -1);

private slots:
    void mover();
    void actualizar();

private:
    QVector<QPixmap> frames;  // <- Para almacenar los fotogramas del sprite del ave

    QGraphicsPixmapItem *sprite;
    QGraphicsScene *scene;
    QTimer *timerMovimiento;
    QTimer *timerAnimacion;
    int frameActual;
    int velocidad;
    int coordX;
    int coordY;
    int fotogWidth;
    int fotogHeight;
    Tipo tipo;

};

#endif // OBSTACULO_H
