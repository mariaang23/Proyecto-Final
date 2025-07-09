#ifndef JUEGO_H
#define JUEGO_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include "nivel1.h"
#include "nivel2.h"
#include "ui_juego.h"

class juego : public QMainWindow
{
    Q_OBJECT

public:
    explicit juego(QWidget *parent = nullptr);
    ~juego();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void iniciarJuego();
    void regresarAlMenuTrasDerrota();
    void mostrarTransicion();
    void mostrarExito();


private:
    Ui::juego *ui;
    QGraphicsScene *scene;
    QGraphicsView *view;
    Nivel1 *nivel1;
    Nivel2 *nivel2;
    Nivel *nivelActual;
    QLabel *exito;
    QTimer *timerFoco = nullptr;//foco en goku

    static int contador;

    void cambiarNivel(int numero);
    void mostrarPantallaInicio();
    void cerrarNivel(bool mostrarMenu);
};

#endif // JUEGO_H
