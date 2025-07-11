#include "nivel1.h"
#include "goku1.h"
#include "robot.h"
#include "obstaculo.h"
#include <QRandomGenerator>
#include <QMessageBox>

// Inicialización del contador
int Nivel1::contador = 0;

// Constructor
Nivel1::Nivel1(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 1)
{
    nivelTerminado = false;
    perdioGoku = false;
    // No se hace nada más aquí para evitar errores con funciones virtuales
}

// Destructor
Nivel1::~Nivel1()
{
    //qDebug() << "Destructor de Nivel1 llamado";

    // Limpiar la cámara
    if (camara) {
        camara->detenerMovimiento();
        delete camara;
    }

    if (timerNivel) {
        timerNivel->stop();
        disconnect(timerNivel, nullptr, this, nullptr);
    }

    // Eliminar carro (usando getSprite())
    if (carroFinal) {
        escena->removeItem(carroFinal->getSprite());
        delete carroFinal;
    }

    // Eliminar robots (usando getSprite())
    if (r1) { escena->removeItem(r1->getSprite()); delete r1; }
    if (r2) { escena->removeItem(r2->getSprite()); delete r2; }
    if (r3) { escena->removeItem(r3->getSprite()); delete r3; }

    // Eliminar obstáculos (usando getSprite())
    for (auto* obs : listaObstaculos) {
        if (obs) {
            escena->removeItem(obs->getSprite());
            delete obs;
        }
    }
    listaObstaculos.clear();
}

// Método separado para iniciar el nivel
void Nivel1::iniciarNivel()
{
    cargarFondoNivel(":/images/background1.png");

    generarNubes();
    agregarGoku();
    agregarCarroFinal();
    agregarObstaculos();

    camara = new camaraLogica(vista, this);
    camara->seguirAGoku(goku);
    camara->iniciarMovimiento();

    // 4) Arrancamos el timer del nivel como antes
    timerNivel = new QTimer(this);
    connect(timerNivel, &QTimer::timeout, this, &Nivel1::actualizarNivel);
    timerNivel->start(20);
}

// Carga el fondo repetido horizontalmente
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

    // Barra de vida en la esquina superior izquierda
    barraVida = new Vida(vista);
    barraVida->move(20, 20);
    barraVida->show();

    // Nueva barra de progreso usando el ícono del carro y tipo Horizontal
    barraProgreso = new Progreso(Horizontal, ":/images/icono_carro.png", vista);
    barraProgreso->move(20, 60);
    barraProgreso->show();

    // Goku nivel 1
    goku = new Goku1(escena, velocidad, 200, 249, this);
    static_cast<Goku1*>(goku)->cargarImagen();
    goku->setBarraVida(barraVida);
    goku->iniciar(posX, posY);

    // Timer para actualizar el progreso de avance
    QTimer* timerProgreso = new QTimer(this);
    connect(timerProgreso, &QTimer::timeout, this, [=]() {
        //qDebug() << "timer progreso en nivel1 llamado  "<<contador++;
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
    carroFinal->iniciar(4900, 550);
}

// Agrega obstáculos de varios tipos
void Nivel1::agregarObstaculos()
{
    int x = 1500;
    int velocidad = 10;

    for (int i = 0; i < 18; ++i) {
        int tipo = QRandomGenerator::global()->bounded(0, 3);
        obstaculo* obs = nullptr;

        switch (tipo) {
        case 0:
            obs = new obstaculo(escena, obstaculo::Ave, velocidad, this);
            obs->iniciar(x, QRandomGenerator::global()->bounded(100, 150));
            x += 500;
            break;
        case 1:
            obs = new obstaculo(escena, obstaculo::Montania, velocidad, this);
            obs->cargarImagenes();
            obs->iniciar(x, escena->height() - obs->getAltura());
            x += 600;
            break;
        case 2:
            obs = new obstaculo(escena, obstaculo::Roca, velocidad, this);
            obs->iniciar(x, QRandomGenerator::global()->bounded(350, 550));
            x += 700;
            break;
        }

        listaObstaculos.push_back(obs);
    }
}

// Lógica de actualización del nivel en cada ciclo
void Nivel1::actualizarNivel()
{
    //qDebug() << "timeractualizar nivel en nivel1 llamado  "<<contador++;
    if (!goku || !carroFinal) return;

    // Si se acabó la vida de Goku
    if (goku->obtenerVida() <= 0) {
        perdioGoku = true;
        gameOver();
    }

    // Detectar si Goku llegó al carro
    Goku1* goku1 = dynamic_cast<Goku1*>(goku);
    if (goku1 && goku1->haTocadoCarro() && !gokuYaPateo) {
        gokuYaPateo = true;
        goku1->patadaGokuNivel1();

        float posXpatada = goku1->x();
        //qDebug() << "posición de Goku en x = " << posXpatada;
        carroFinal->iniciarMovimientoEspiral(posXpatada);
    }

    if (carroFinal->espiralHecha && carroFinal->haLlegadoAlSuelo()) {
        agregarRobots();
        quitarCarroVista();

        // Espera 5 segundos antes de emitir la señal
        QTimer::singleShot(5000, this, [this]() {

            //qDebug() << "timer single shot mostrar nivelcompletado en nivel 1 llamado  " <<contador++;
            emit nivelCompletado();
        });
    }
}

// Agrega los tres robots cuando el carro cae
void Nivel1::agregarRobots()
{
    if (robotsCreados || !carroFinal || !carroFinal->haLlegadoAlSuelo())
        return;

    robotsCreados = true;
    int ySuelo = 500;
    int velocidad = 6;

    r1 = new Robot(escena, velocidad, 1, this);
    r2 = new Robot(escena, velocidad, 2, this);
    r3 = new Robot(escena, velocidad, 3, this);

    r1->iniciar(5000, ySuelo, 5300);
    r1->desplegarRobot();

    QTimer::singleShot(600, this, [=]() {

        //qDebug() << "timer singles  robots 1 nivel1 llamado  "<<contador++;
        r2->iniciar(5300, ySuelo, 5600);
        r2->desplegarRobot();
    });

    QTimer::singleShot(1200, this, [=]() {

        //qDebug() << "timer singles  robots 2 nivel1 llamado  "<<contador++;
        r3->iniciar(5600, ySuelo, 5900);
        r3->desplegarRobot();
    });

    QTimer::singleShot(1800, this, [=]() {

        //qDebug() << "timer singles  robots 3 nivel1 llamado  "<<contador++;
        r1->detenerMvtoRobot();
        r2->detenerMvtoRobot();
        r3->detenerMvtoRobot();
    });
}

// Elimina el carro de la escena
void Nivel1::quitarCarroVista()
{
    if (carroFinal && escena) {
        escena->removeItem(carroFinal->getSprite());
        delete carroFinal;
        carroFinal = nullptr;
    }
}

void Nivel1::gameOver()
{
    if (gameOverProcesado) return;//si ya se llamo (v), no se llame mas

    //pasamos a estado verdadero si ya se llamo
    gameOverProcesado = true;

    if (timerNivel) {
        timerNivel->stop();
        disconnect(timerNivel, nullptr, this, nullptr);  //desconexión explícita
    }
    mostrarGameOver();
    emit gokuMurio();
}

//indica goku muere
bool Nivel1::getPerdioGoku() const{
    return perdioGoku;
}

// Indica si el nivel ha terminado exitosamente
bool Nivel1::haTerminado() const
{
    return nivelTerminado;
}

// Devuelve el puntero al Goku del nivel
Goku* Nivel1::getGoku() const
{
    return goku;
}
