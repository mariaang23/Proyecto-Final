#ifndef JUEGO_H
#define JUEGO_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
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

private slots:
    void iniciarJuego();         // Slot que se activa al presionar el botón de inicio

private:
    Ui::juego *ui;
    QGraphicsScene *scene;
    QGraphicsView *view;

    Nivel1 *nivel1;
    Nivel2 *nivel2;
    Nivel *nivelActual;

    QTimer *timerEstado;         // Temporizador para revisar el estado del nivel

    void cambiarNivel(int numero);  // Método privado para cambiar entre niveles
};

#endif // JUEGO_H
