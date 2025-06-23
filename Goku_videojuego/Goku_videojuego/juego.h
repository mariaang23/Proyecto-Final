#ifndef JUEGO_H
#define JUEGO_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "nivel.h"

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
    nivel* nivel1;
    nivel* nivel2;
    nivel* nivelActual;
};

#endif // JUEGO_H
