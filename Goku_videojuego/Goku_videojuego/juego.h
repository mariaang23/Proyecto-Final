#ifndef JUEGO_H
#define JUEGO_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "nivel1.h"
#include "nivel2.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class juego;
}
QT_END_NAMESPACE

class juego : public QMainWindow
{
    Q_OBJECT

public:
    explicit juego(QWidget *parent = nullptr);
    ~juego();
    void iniciarJuego();
    void cambiarNivel(int numero);

private:
    Ui::juego *ui;
    QGraphicsScene *scene;
    QGraphicsView *view;
    Nivel1* nivel1;
    Nivel2* nivel2;
    Nivel* nivelActual;
};

#endif // JUEGO_H
