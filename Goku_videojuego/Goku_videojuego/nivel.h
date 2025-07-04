#ifndef NIVEL_H
#define NIVEL_H

#include <QWidget>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>

#include "camaralogica.h"
#include "goku.h"
#include "obstaculo.h"
#include "vida.h"
#include "progreso.h"

class Nivel : public QWidget
{
    Q_OBJECT

protected:
    // Elementos comunes a todos los niveles
    int numeroNivel;
    const int margenHUD = 70;

    QPixmap nube;
    QPixmap background;

    QGraphicsView *vista;
    QGraphicsScene *escena;

    camaraLogica *camara = nullptr;
    Goku* goku = nullptr;
    Vida* barraVida = nullptr;
    Progreso* barraProgreso = nullptr;

    QTimer* timerNivel = nullptr;
    QTimer* timerNubes = nullptr;

    std::vector<QGraphicsPixmapItem*> listaFondos;
    std::vector<QGraphicsPixmapItem*> listaNubes;

    std::vector<obstaculo*> listaObstaculos;

    void generarNubes();

    //final gameover
    void mostrarGameOver();
    QLabel* overlayGameOver = nullptr;


public:
    static int contNubes;
    explicit Nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent, int numero);
    virtual ~Nivel();

    int getMargenHUD() const;

    // Métodos virtuales que cada subclase debe implementar
    virtual void iniciarNivel()=0;
    virtual void cargarFondoNivel(const QString &ruta) = 0;
    virtual void agregarGoku() = 0;
    virtual void actualizarNivel() = 0;

    // nivel.h
    Goku* getGoku() const { return goku; }


protected slots:
    void moverNubes();

signals:
    void gokuMurio();
};

#endif // NIVEL_H
