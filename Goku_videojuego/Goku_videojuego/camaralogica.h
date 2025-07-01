#ifndef CAMARALOGICA_H
#define CAMARALOGICA_H

#include <QObject>
#include <QGraphicsView>
#include <QTimer>
#include "goku.h"

class camaraLogica : public QObject
{
    Q_OBJECT

public:
    static int contCamara ;
    camaraLogica(QGraphicsView *vista, int velocidad, QObject *parent = nullptr);
    ~camaraLogica();
    void iniciarMovimiento();
    void detenerMovimiento();
    void seguirAGoku(Goku* goku);

    //void setNubes(QList<QGraphicsPixmapItem*> nubes);

private slots:
    void moverVista();

private:
    QGraphicsView *view;
    QTimer *timer;
    int velocidad;
    //QList<QGraphicsPixmapItem*> nubes;
};

#endif // CAMARALOGICA_H
