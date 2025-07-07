#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "pocion.h"
#include "progreso.h"
#include "robot.h"
#include "explosion.h"
#include <QVector>

class Nivel2 : public Nivel
{
    Q_OBJECT

public:
    Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent = nullptr);
    // Destructor: libera memoria de pociones, explosiones y temporizador
    ~Nivel2();

    void iniciarNivel() override;
    void cargarFondoNivel(const QString &ruta) override;
    void agregarGoku() override;
    void actualizarNivel() override;
    Goku* getGoku() const;

    void pocionRecolectada();
    void agregarExplosion(Explosion* e);  // Registra una nueva explosión

private:
    void agregarPociones();              // Agrega la primera poción y frames
    void agregarRobot();                 // Posiciona y escala el robot enemigo
    void agregarPocionAleatoria();       // Genera nuevas pociones durante el juego
    void gameOver();                     // Finaliza el nivel por derrota

    QVector<QPixmap> framesPocion;           // Animaciones de la poción
    QVector<Pocion*> listaPociones;          // Pociones activas en la escena
    QVector<Explosion*> listaExplosiones;    // Explosiones activas en la escena

    bool robotInicialCreado = false;
    bool pocionesAgregadas = false;

    Progreso* barraProgreso = nullptr;
    const int totalPociones = 5;

    QTimer* temporizadorPociones = nullptr;

    Robot* robot = nullptr;
    Explosion* explosion = nullptr;

    bool perdioGoku = false;

signals:
    void nivelCompletado();  // Se emite al terminar exitosamente el nivel
};

#endif // NIVEL2_H
