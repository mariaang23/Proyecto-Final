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
    int numeroNivel = 1;
    QPixmap nube;
    QPixmap background;
    QGraphicsView *vista;
    QGraphicsScene *escena;

    camaraLogica *camara;
    Carro* carroFinal = nullptr;
    QTimer* timerColision = nullptr;
    QTimer* timerNubes = nullptr;
    Goku* goku = nullptr;

    std::vector<obstaculo*> listaObstaculos;
    std::vector<QGraphicsPixmapItem*> listaFondos;
    std::vector<QGraphicsPixmapItem*> listaNubes;

public:
    explicit nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent, int numero);
    virtual ~nivel();

    void cargarFondoNivel(const QString &ruta);
    void agregarObstaculos();
    void agregarGokuNivel1();
    void agregarCarroFinal();

private slots:
    void moverNubes();

protected:
    QTimer *temporizador = nullptr;
    int tiempoRestante = 0;
};

#endif // NIVEL_H
