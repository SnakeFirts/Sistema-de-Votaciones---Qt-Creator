#include "principal.h"
#include "ui_principal.h"
#include "crearcuenta.h"
#include "dashboard.h"
#include <QMessageBox>

Principal::Principal(QWidget *parent):QMainWindow(parent), ui(new Ui::Principal){
    ui->setupUi(this);
    socket = new QTcpSocket(this);
    connect(ui->lblCrearCuenta, &QLabel::linkActivated, this, &Principal::abrirCrearCuenta);
}

Principal::~Principal()
{
    delete ui;
}

void Principal::abrirCrearCuenta(const QString &link)
{
    Q_UNUSED(link);

    CrearCuenta dialog(socket, this);
    dialog.exec();
}

void Principal::Login()
{
    QString dni = ui->txtDNI->text();
    QString password = ui->txtPassword->text();

    if (dni.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Campo vacío", "Por favor ingrese su DNI y contraseña");
        return;
    }

    if (socket->state() != QAbstractSocket::ConnectedState) {
        socket->connectToHost("127.0.0.1", 1234);

        if (!socket->waitForConnected(3000)) {
            QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
            return;
        }
    }

    if (!socket->waitForConnected(3000)) {
        QMessageBox::critical(this, "Error", "No se pudo conectar al servidor");
    }


    QString msn = "LOGIN|" + dni + "|" + password;
    socket->write(msn.toUtf8());
    socket->flush();

    if (socket->waitForReadyRead(3000)) {
        QString respuesta = socket->readAll();

        if (respuesta.startsWith("OK")) {
            QStringList datos = respuesta.split("|");

            QString nombre = datos.value(1);
            QString depart = datos.value(2);
            QString token  = datos.value(4);

            Dashboard *dash = new Dashboard(socket, nombre, depart, token);
            dash->show();
            this->hide();
        } else {
            QMessageBox::warning(this, "Login fallido", respuesta);
        }
    }
}
