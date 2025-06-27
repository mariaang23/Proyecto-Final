#ifndef GOKU_H
#define GOKU_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>
#include <QVector>
#include "vida.h"

class Goku : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Goku(QGraphicsScene *scene, int _velocidad, int _fotogWidth, int _fotogHeight, int _nivel, QObject *parent = nullptr);
    ~Goku();

    void cargarImagen();
    void iniciar(int x = -1, int y = -1);

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void setBarraVida(Vida* barra);
    void recibirDanio(int cantidad);

    void actualizarFrame(int indice);
    QString detectarColision() const;
    bool haTocadoCarro() const;
    bool haTocadoObstaculo() const;

    void patadaGokuNivel1();

    // Detiene por completo el desplazamiento y la entrada de teclas
    void detener();


private slots:
    void mover();

private:
    QGraphicsScene *scene;
    QVector<QPixmap> frames;
    int frameActual;
    QTimer *timerMovimiento;
    int velocidad;
    int fotogWidth;
    int fotogHeight;
    int nivel;

    bool mvtoArriba;
    bool mvtoAbajo;
    bool tocoCarro;
    bool tocoObstaculo;
    bool yaRecibioDanio;

    Vida* vidaHUD = nullptr; // Nueva: barra de vida en HUD
};

#endif // GOKU_H
