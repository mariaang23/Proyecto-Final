#ifndef GOKU1_H
#define GOKU1_H

#include "goku.h"
#include <QTimer>
#include <QString>

class Goku1 : public Goku {
    Q_OBJECT

public:
    Goku1(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, QObject* parent = nullptr);
    ~Goku1();

    void iniciar(int x, int y) override;
    void detener() override;
    void cargarImagen() override;

    QString detectarColision() const;
    bool haTocadoCarro() const;
    bool haTocadoObstaculo() const;
    void patadaGokuNivel1();

protected:
    void mover() override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void actualizarFrame(int i);
    void mientrasTocaObstaculo();

    QVector<QPixmap> frames;        // Frames del sprite
    QTimer* timerMovimiento;        // Timer para mover a Goku
    QTimer* timerDanio;             // Timer entre daños

    int frameActual;                // Frame actual del sprite
    int contadorCaminata;           // Control para animación de caminar
    bool mvtoArriba;                // Movimiento con tecla W
    bool mvtoAbajo;                 // Movimiento con tecla S

    bool yaRecibioDanio;            // Flag para recibir daño una vez
    bool puedeRecibirDanio;         // Temporizador para daño por colisión
    bool tocoCarro;                 // Si colisionó con el carro
    bool tocoObstaculo;             // Si colisionó con un obstáculo
};

#endif // GOKU1_H
