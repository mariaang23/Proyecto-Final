#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include <QTimer>

class Nivel2 : public Nivel
{
    Q_OBJECT

public:
    Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent = nullptr);
    ~Nivel2();

    void iniciarNivel();

    // Implementación obligatoria de métodos virtuales puros de Nivel
    void cargarFondoNivel(const QString &ruta) override;
    void agregarGoku() override;
    void actualizarNivel() override;

private:
    void agregarPociones();
    void agregarRobotInicial();

    QTimer* timerNivel = nullptr;

    bool robotInicialCreado = false;
    bool pocionesAgregadas = false;
    Carro* carroFinal = nullptr;
};

#endif // NIVEL2_H
