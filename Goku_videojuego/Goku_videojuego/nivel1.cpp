#include "nivel1.h"
#include "goku.h"
#include "robot.h"
#include "obstaculo.h"
#include <QRandomGenerator>

// Constructor
Nivel1::Nivel1(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 1), gokuYaPateo(false), robotsCreados(false)
{
    // No se hace nada más aquí para evitar errores con funciones virtuales
}

// Destructor
Nivel1::~Nivel1()
{
    qDebug() << "Destructor de Nivel1 llamado";
    delete carroFinal;
    delete r1;
    delete r2;
    delete r3;

    // Eliminar todos los obstaculos en listaObstaculos
    for (auto *obs : listaObstaculos) {
        delete obs;
    }
    listaObstaculos.clear();
}

// Método separado para iniciar el nivel (llamado después del constructor)
void Nivel1::iniciarNivel()
{
    cargarFondoNivel(":/images/background1.png");

    generarNubes();
    agregarGoku();
    agregarCarroFinal();
    agregarObstaculos();

    camara = new camaraLogica(vista, 6, this);
    camara->iniciarMovimiento();

    timerNivel = new QTimer(this);
    connect(timerNivel, &QTimer::timeout, this, &Nivel1::actualizarNivel);
    timerNivel->start(20);
}

// Implementación de método obligatorio: carga fondo
void Nivel1::cargarFondoNivel(const QString &ruta)
{
    QPixmap fondo(ruta);
    int ancho = fondo.width();
    int cantidad = 5;

    for (int i = 0; i < cantidad; ++i) {
        QGraphicsPixmapItem* fondoItem = escena->addPixmap(fondo);
        fondoItem->setZValue(-10);
        fondoItem->setPos(i * ancho, 0);
        listaFondos.push_back(fondoItem);
    }
}

// Agrega a Goku al nivel
void Nivel1::agregarGoku()
{
    int posX = 100;
    int posY = 400;
    int velocidad = 6;

    barraVida = new Vida(vista);
    barraVida->move(20, 20);
    barraVida->show();

    barraProgreso = new Progreso(vista);
    barraProgreso->move(20, 60);
    barraProgreso->show();

    goku = new Goku(escena, velocidad, 200, 249, 1, this);
    goku->setBarraVida(barraVida);
    goku->iniciar(posX, posY);
    goku->setFocus();

    QTimer* timerProgreso = new QTimer(this);
    connect(timerProgreso, &QTimer::timeout, this, [=]() {
        if (goku && carroFinal && barraProgreso) {
            float inicio = 0;
            float fin = carroFinal->getSprite()->x();
            barraProgreso->actualizarProgreso(goku->x(), inicio, fin);
        }
    });
    timerProgreso->start(50);
}

// Agrega el carro final al nivel
void Nivel1::agregarCarroFinal()
{
    carroFinal = new Carro(escena, 0, this);
    carroFinal->iniciar(4700, 500);
}

// Agrega obstáculos al nivel
void Nivel1::agregarObstaculos()
{
    int x = 2000;
    int velocidad = 10;

    for (int i = 0; i < 16; ++i) {
        int tipo = QRandomGenerator::global()->bounded(0, 3);
        obstaculo* obs = nullptr;

        switch (tipo) {
        case 0: {
            obs = new obstaculo(escena, obstaculo::Ave, velocidad, this);
            int y = QRandomGenerator::global()->bounded(100, 150);
            obs->iniciar(x, y);
            x += 500;
            break;
        }
        case 1: {
            obs = new obstaculo(escena, obstaculo::Montania, velocidad, this);
            obs->cargarImagenes();
            int y = escena->height() - obs->getAltura();
            obs->iniciar(x, y);
            x += 600;
            break;
        }
        case 2: {
            obs = new obstaculo(escena, obstaculo::Roca, velocidad, this);
            int y = QRandomGenerator::global()->bounded(350, 550);
            obs->iniciar(x, y);
            x += 700;
            break;
        }
        }

        listaObstaculos.push_back(obs);
    }
}

// Lógica del nivel ejecutada en cada ciclo
void Nivel1::actualizarNivel()
{
    if (!goku || !carroFinal) return;

    if (goku->haTocadoCarro() && !gokuYaPateo) {
        gokuYaPateo = true;
        goku->patadaGokuNivel1();
        carroFinal->iniciarMovimientoEspiral();
    }

    if (carroFinal->espiralHecha && carroFinal->haLlegadoAlSuelo()) {
        agregarRobots();
    }
}

// Agrega enemigos robots cuando el carro cae
void Nivel1::agregarRobots()
{
    if (robotsCreados || !carroFinal || !carroFinal->haLlegadoAlSuelo())
        return;

    robotsCreados = true;
    int ySuelo = 400;
    int velocidad = 6;

    r1 = new Robot(escena, velocidad, 1, this);
    r2 = new Robot(escena, velocidad, 2, this);
    r3 = new Robot(escena, velocidad, 3, this);

    r1->iniciar(5000, ySuelo, 5000);
    r1->desplegarRobot();

    QTimer::singleShot(600, this, [=]() {
        r2->iniciar(5300, ySuelo, 5300);
        r2->desplegarRobot();
    });

    QTimer::singleShot(1200, this, [=]() {
        r3->iniciar(5600, ySuelo, 5600);
        r3->desplegarRobot();
    });

    QTimer::singleShot(1800, this, [=]() {
        r1->detenerMvtoRobot();
        r2->detenerMvtoRobot();
        r3->detenerMvtoRobot();
    });
}
