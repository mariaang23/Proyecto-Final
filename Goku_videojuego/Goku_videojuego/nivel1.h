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

    void iniciarNivel(); // Llamado después del constructor

    // Métodos sobrescritos de la clase base
    void actualizarNivel() override;
    void cargarFondoNivel(const QString &ruta) override;
    void agregarGoku() override;

    //metodos de la clase
    void agregarObstaculos();
    void agregarCarroFinal();
    void quitarCarroVista();
    Goku* getGoku() const;


private:

    void agregarRobots(); // Crea los robots enemigos cuando el carro aterriza

    Carro* carroFinal = nullptr;
    bool gokuYaPateo;
    bool robotsCreados;
    Robot *r1;
    Robot *r2;
    Robot *r3;
};

#endif // NIVEL1_H
