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
    Goku(QGraphicsScene *scene, int velocidad, int fotogWidth, int fotogHeight, int nivel, QObject *parent = nullptr);
    ~Goku();

    void iniciar(int x, int y);
    void setBarraVida(Vida* barra);
    void recibirDanio(int cantidad);
    void detener();
    void patadaGokuNivel1();

    QString detectarColision() const;
    bool haTocadoCarro() const;
    bool haTocadoObstaculo() const;

    void setSueloY(float y);
    // metodo getter para obtener la vida actual
    int obtenerVida() const;


protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private slots:
    void mover();

private:
    void cargarImagen();
    void actualizarFrame(int indice);
    void mientrasTocaObstaculo();

    void actualizarSpriteSalto();       // Solo nivel 2
    void actualizarSpriteCaminar(bool derecha); // Solo nivel 2

    QGraphicsScene *scene;
    QTimer *timerMovimiento;
    QTimer *timerSalto;                 // <--- Agregado timerSalto
    QVector<QPixmap> frames;

    QTimer* timerDanio;
    bool puedeRecibirDanio;


    int frameActual;
    int contadorCaminata;  // Solo nivel 2

    int velocidad;
    int fotogWidth;
    int fotogHeight;
    int nivel;

    bool mvtoArriba;
    bool mvtoAbajo;
    bool mvtoIzquierda;
    bool mvtoDerecha;
    bool mirandoDerecha;
    bool yaRecibioDanio;
    Vida* vidaHUD;


    bool tocoCarro;
    bool tocoObstaculo;

    // Físicas para nivel 2
    qreal yInicial;
    bool enSalto;
    float tiempoSalto;
    float gravedad;
    float sueloY;
    float velocidadVertical = 0;

};

#endif // GOKU_H
