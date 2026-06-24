#ifndef CREARCUENTA_H
#define CREARCUENTA_H
#include <QDialog>
#include <QTcpSocket>

namespace Ui {
class CrearCuenta;
}

class CrearCuenta : public QDialog
{
    Q_OBJECT

public:
    explicit CrearCuenta(QTcpSocket *socket, QWidget *parent = nullptr);
    ~CrearCuenta();

private slots:
    void on_btnRegistrar_clicked();

private:
    Ui::CrearCuenta *ui;
    QTcpSocket *socket;
};

#endif
