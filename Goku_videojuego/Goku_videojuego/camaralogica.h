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
    camaraLogica(QGraphicsView *vista, QObject *parent = nullptr);
    ~camaraLogica();

    void iniciarMovimiento();
    void detenerMovimiento();
    void seguirAGoku(Goku* goku);

private slots:
    void moverVista();

private:
    QGraphicsView *view;
    QTimer *timer;
    Goku *objetivo = nullptr;
};

#endif // CAMARALOGICA_H
