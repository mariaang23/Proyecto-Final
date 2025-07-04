#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivel.h"
#include "robot.h"
#include "carro.h"

class Nivel1 : public Nivel
{
    Q_OBJECT

public:
    Nivel1(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent = nullptr);
    ~Nivel1();

    void iniciarNivel() override;

    // Métodos sobrescritos de la clase base
    void actualizarNivel() override;
    void cargarFondoNivel(const QString &ruta) override;
    void agregarGoku() override;

    // Métodos específicos del nivel 1
    void agregarObstaculos();
    void agregarCarroFinal();
    void quitarCarroVista();
    void gameOver();
    bool haTerminado() const;
    bool getPerdioGoku() const;
    Goku* getGoku() const;

private:
    void agregarRobots(); // Crea los robots enemigos cuando el carro aterriza

    Carro* carroFinal = nullptr;
    bool gokuYaPateo;
    bool robotsCreados;
    Robot *r1;
    Robot *r2;
    Robot *r3;
    bool nivelTerminado; // Indica si el nivel ha terminado exitosamente
    bool perdioGoku;


signals:
    void nivelCompletado();
};

#endif // NIVEL1_H
