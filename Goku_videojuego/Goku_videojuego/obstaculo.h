#ifndef OBSTACULO_H
#define OBSTACULO_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>


class obstaculo : public QObject
{
    Q_OBJECT
public:
    static int contObsta;
    enum Tipo {Ave, Montania, Roca, Explosion};
    obstaculo(QGraphicsScene *scene, Tipo tipo, int velocidad, QObject *parent = nullptr);
    ~obstaculo();

    void cargarImagenes();
    virtual void iniciar(int x = -1, int y = -1);
    int getAltura() const;
    QGraphicsPixmapItem* getSprite() const { return sprite; }

    //cambio para poder realizar herencia a explosion
    QGraphicsPixmapItem *sprite;
    QGraphicsScene *scene;
    QVector<QPixmap> frames;  // Para almacenar los fotogramas del sprite del ave

private slots:
    void mover();
    void actualizar();

private:

    QTimer *timerMovimiento;
    QTimer *timerAnimacion;
    int frameActual;
    int velocidad;
    int coordX;
    int coordY;
    int fotogWidth;
    int fotogHeight;
    Tipo tipo;


};
#endif // OBSTACULO_H
