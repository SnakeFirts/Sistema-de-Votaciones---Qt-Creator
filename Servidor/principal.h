#ifndef PRINCIPAL_H
#define PRINCIPAL_H
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>

namespace Ui {
class Principal;
}

class Principal : public QMainWindow
{
    Q_OBJECT

public:
    explicit Principal(QWidget *parent = 0);
    ~Principal();

private slots:
    void cargarUsuarios();
    void mostrarUsuariosQueVotaron();
    void mostrarResultados();

private:
    Ui::Principal *ui;
    QTcpServer *servidor;
    QTcpSocket *cliente;
    QMap<QString, QString> tokens;

    QSet<QString> usuariosQueVotaron;
    QMap<int, int> votosPresidente;
};

#endif // PRINCIPAL_H
