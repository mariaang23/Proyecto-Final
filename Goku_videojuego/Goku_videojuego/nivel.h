#ifndef NIVEL_H
#define NIVEL_H

#include <QWidget>
#include <QTimer>
#include <QList>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include "camaralogica.h"

class nivel : public QWidget
{
    Q_OBJECT

private:
    QPixmap background;
    QGraphicsView *vista;
    QGraphicsScene *escena;
    camaraLogica *camara;

public:
    explicit nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent = nullptr);
    virtual ~nivel();
    void cargarFondo(const QString &ruta);
    //void generarNubes(int cantidad);
    //void moverNubes();

protected:
    QTimer *temporizador;
    int tiempoRestante;


};

#endif // NIVEL_H
