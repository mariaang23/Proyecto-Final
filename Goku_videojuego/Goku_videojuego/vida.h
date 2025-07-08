#ifndef VIDA_H
#define VIDA_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>

class Vida : public QWidget
{
    Q_OBJECT

public:
    explicit Vida(QWidget *parent = nullptr);
    virtual ~Vida();

    void restar(int cantidad);
    void reiniciar();
    int obtenerVida() const;

private:
    const int vidaMaxima = 100;
    int vidaActual = 100;
    QLabel* texto;
    QProgressBar* barra;

    void actualizarColor();
};

#endif // VIDA_H
