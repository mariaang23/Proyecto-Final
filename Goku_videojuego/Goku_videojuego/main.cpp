#include "juego.h"

#include <QApplication>
#include <QDebug>

using namespace std;
int liberar = 0;

void* operator new(size_t size){
    void * p = malloc(size);
    liberar +=1;

    //qDebug() << "a liberar "<<liberar;
    return p;
}

void operator delete(void* p, std::size_t){
    liberar-=1;

    //qDebug() << "memoria restante "<<liberar;
    free(p);
}

void operator delete(void* p) noexcept {
    free(p);
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    juego w;
    w.show();
    return a.exec();
}
