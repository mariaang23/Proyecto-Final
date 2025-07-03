#ifndef GOKU_H
#define GOKU_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include <QVector>
#include "vida.h"

class Goku : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Goku(QGraphicsScene *scene, int velocidad, int fotogWidth, int fotogHeight, QObject *parent = nullptr);
    virtual ~Goku();

    void setBarraVida(Vida* barra);
    void recibirDanio(int cantidad);
    int obtenerVida() const; // Getter de vida actual

    virtual void iniciar(int x, int y) = 0; // Método abstracto para inicializar
    virtual void detener() = 0;             // Método abstracto para detenerse

protected:
    virtual void keyPressEvent(QKeyEvent *event) override = 0;
    virtual void keyReleaseEvent(QKeyEvent *event) override = 0;
    virtual void cargarImagen() = 0;
    virtual void mover() = 0;

    void actualizarFrame(int indice);

    QGraphicsScene *scene;
    QTimer *timerMovimiento;
    QTimer *timerDanio;

    QVector<QPixmap> frames;
    int frameActual;

    int velocidad;
    int fotogWidth;
    int fotogHeight;
    bool mirandoDerecha;

    Vida* vidaHUD;
    bool puedeRecibirDanio;

};

#endif // GOKU_H
