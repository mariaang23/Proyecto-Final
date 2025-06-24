#ifndef NIVEL_H
#define NIVEL_H

#include <QWidget>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>  // Usamos std::vector

#include "camaralogica.h"
#include "carro.h"
#include "goku.h"
#include "obstaculo.h"

class nivel : public QWidget
{
    Q_OBJECT

private:
    QPixmap nube;
    QPixmap background;
    QGraphicsView *vista;
    QGraphicsScene *escena;

    camaraLogica *camara;
    Carro* carroFinal = nullptr;
    QTimer* timerColision = nullptr;
    Goku* goku = nullptr;

    std::vector<obstaculo*> listaObstaculos;
    std::vector<QGraphicsPixmapItem*> listaFondos;
    std::vector<QGraphicsPixmapItem*> listaNubes;

public:
    explicit nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent = nullptr);
    virtual ~nivel();

    void cargarFondoNivel1(const QString &ruta);
    void agregarObstaculos();
    void agregarGokuNivel1();
    void agregarCarroFinal();

protected:
    QTimer *temporizador = nullptr;
    int tiempoRestante = 0;
};

#endif // NIVEL_H
