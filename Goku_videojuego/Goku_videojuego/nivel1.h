#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivel.h"
#include <QTimer>

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
    void agregarObstaculos() override;
    void agregarCarroFinal() override;

private:
    void agregarRobots(); // Crea los robots enemigos cuando el carro aterriza

    bool gokuYaPateo;
    bool robotsCreados;
};

#endif // NIVEL1_H
