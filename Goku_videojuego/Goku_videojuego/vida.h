#ifndef VIDA_H
#define VIDA_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QHBoxLayout>

class Vida : public QWidget
{
    Q_OBJECT

public:
    explicit Vida(QWidget *parent = nullptr);
    virtual ~Vida();

    void restar(int cantidad);       // Reduce la vida
    void reiniciar();                // Restaura a 100
    int obtenerVida() const;         // Devuelve el valor actual

private:
    const int vidaMaxima = 100;
    int vidaActual = 100;

    QLabel* texto;
    QProgressBar* barra;

    void actualizarColor();          // Cambia el color de la barra
};

#endif // VIDA_H
