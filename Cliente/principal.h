#ifndef PRINCIPAL_H
#define PRINCIPAL_H
#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>

namespace Ui {
class Principal;
}

class Principal : public QMainWindow
{
    Q_OBJECT

public:
    explicit Principal(QWidget *parent = nullptr);
    ~Principal();

private slots:
    void Login();
    void abrirCrearCuenta(const QString &link);

private:
    Ui::Principal *ui;
    QTcpSocket *socket;
};

#endif // PRINCIPAL_H
