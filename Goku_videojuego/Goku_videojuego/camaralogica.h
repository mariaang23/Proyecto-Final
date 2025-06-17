#ifndef CAMARALOGICA_H
#define CAMARALOGICA_H

#include <QObject>
#include <QGraphicsView>
#include <QTimer>

class camaraLogica : public QObject
{
    Q_OBJECT

public:
    camaraLogica(QGraphicsView *vista, int velocidad, QObject *parent = nullptr);
    void iniciarMovimiento();
    void detenerMovimiento();

private slots:
    void moverVista();

private:
    QGraphicsView *view;
    QTimer *timer;
    int velocidad;
};

#endif // CAMARALOGICA_H
