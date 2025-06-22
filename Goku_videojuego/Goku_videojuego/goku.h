#ifndef GOKU_H
#define GOKU_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QKeyEvent>
#include <QVector>

class Goku : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Goku(QGraphicsScene *scene, int _velocidad, int _fotogWidth, int _fotogHeight, int _nivel, QObject *parent = nullptr);
    //void cargarSprite(const QString &ruta);
    //void moverArriba();
    //void moverAbajo();
    //void avanzar();
    void cargarImagen();
    void iniciar(int x = -1, int y = -1);
    void keyPressEvent(QKeyEvent *event) override; //para manejar teclas
    void keyReleaseEvent(QKeyEvent *event) override;

protected:
    //void keyPressEvent(QKeyEvent *event) override;
    //QGraphicsPixmapItem *sprite;

private slots:
    //void actualizar();
    void mover();

private:
    QGraphicsScene *scene;
    QVector<QPixmap> frames;
    int frameActual;
    QTimer *timerMovimiento;
    int velocidad;
    int fotogWidth;
    int fotogHeight;
    int nivel;
    bool mvtoArriba;  //moviendo hacia arriba
    bool mvtoAbajo; //moviendo hacia abajo
};

#endif // GOKU_H
