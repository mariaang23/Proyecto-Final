#ifndef NIVEL_H
#define NIVEL_H

#include <QWidget>
#include <QTimer>
#include <QList>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include "camaralogica.h"
#include "carro.h"

class nivel : public QWidget
{
    Q_OBJECT

private:
    QPixmap nube;
    QPixmap background;
    QGraphicsView *vista;
    QGraphicsScene *escena;
    camaraLogica *camara;
    Carro* carroFinal;
    QTimer* timerColision;
    QGraphicsPixmapItem* goku;



public:
    explicit nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent = nullptr);
    virtual ~nivel();
    void extracted();
    void cargarFondoNivel1(const QString &ruta);
    void agregarCarroFinal();

protected:
    QTimer *temporizador;
    int tiempoRestante;


};

#endif // NIVEL_H
