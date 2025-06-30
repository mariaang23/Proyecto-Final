#ifndef POCION_H
#define POCION_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QVector>
#include <QObject>

class Pocion : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Pocion(const QVector<QPixmap>& framesOriginales, int anchoVista, int fila, int columna, int columnas, QGraphicsItem* parent = nullptr);
    ~Pocion();

private slots:
    void moverYAnimar();

private:
    QVector<QPixmap> frames;
    int indiceFrame;
    QTimer* timer;

    int fila;
    int columna;
    int columnasTotales;
    int anchoVista;
};

#endif // POCION_H
