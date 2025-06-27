#include <QWidget>
#include <QLabel>

class Progreso : public QWidget
{
    Q_OBJECT

public:
    explicit Progreso(QWidget *parent = nullptr);
    void actualizarProgreso(float posicionGoku, float inicio, float fin);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    float porcentaje;
    QLabel *iconoCarro;
};
