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
#include "progreso.h"

class nivel : public QWidget
{
    Q_OBJECT

private:
    int numeroNivel = 1;
    const int margenHUD = 70;
    QPixmap nube;
    QPixmap background;
    QGraphicsView *vista;
    QGraphicsScene *escena;
    QTimer *timerNivel = nullptr;

    camaraLogica *camara;
    Carro* carroFinal = nullptr;
    QTimer* timerColision = nullptr;
    QTimer* timerNubes = nullptr;
    Goku* goku = nullptr;
    Vida* barraVida = nullptr;
    Progreso *barraProgreso = nullptr;


    std::vector<obstaculo*> listaObstaculos;
    std::vector<QGraphicsPixmapItem*> listaFondos;
    std::vector<QGraphicsPixmapItem*> listaNubes;

    bool robotsCreados = false; //para incluir los robot nivel 1

public:
    explicit nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent, int numero);
    virtual ~nivel();

    void cargarFondoNivel(const QString &ruta);
    void agregarObstaculos();
    void agregarGokuNivel1();
    void agregarCarroFinal();
    int getMargenHUD() const;
    void agregarRobotsNivel1();




private slots:
    void moverNubes();
    void actualizarNivel();


protected:
    QTimer *temporizador = nullptr;
    int tiempoRestante = 0;
    bool gokuYaPateo = false;   // true cuando la patada ya se ejecutó
};

#endif // NIVEL_H
