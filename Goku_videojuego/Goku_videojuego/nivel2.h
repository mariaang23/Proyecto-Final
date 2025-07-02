#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "pocion.h"
#include <vector>
#include <QVector>

class Nivel2 : public Nivel
{
    Q_OBJECT

public:
    Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent = nullptr);
    ~Nivel2();

    void iniciarNivel();
    void cargarFondoNivel(const QString &ruta) override;
    void agregarGoku() override;
    void actualizarNivel() override;
    Goku* getGoku() const;

private:
    void agregarPociones();
    void agregarRobotInicial();

    QVector<QPixmap> framesPocion;     // Frames de animación de la poción
    std::vector<Pocion*> listaPociones;

    bool robotInicialCreado = false;
    bool pocionesAgregadas = false;
};

#endif // NIVEL2_H
