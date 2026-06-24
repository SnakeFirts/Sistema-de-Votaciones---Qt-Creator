#include "crearcuenta.h"
#include "ui_crearcuenta.h"

CrearCuenta::CrearCuenta(QTcpSocket *socket, QWidget *parent):QDialog(parent), ui(new Ui::CrearCuenta), socket(socket){
    ui->setupUi(this);

    ui->cbMunicipio->addItems({
        "Distrito Central","Alubarén","Cedros","Curarén","El Porvenir",
        "Guaimaca","La Libertad","La Venta","Lepaterique","Maraita",
        "Marale","Nueva Armenia","Ojojona","Orica","Reitoca",
        "Sabanagrande","San Antonio de Oriente","San Buenaventura",
        "San Ignacio","San Juan de Flores","San Miguelito",
        "Santa Ana","Santa Lucía","Talanga","Tatumbla",
        "Valle de Ángeles","Villa de San Francisco","Vallecillo"
    });
}

CrearCuenta::~CrearCuenta()
{
    delete ui;
}

void CrearCuenta::on_btnRegistrar_clicked()
{
    QString dni = ui->leDNI->text();
    QString pass = ui->lePassword->text();
    QString pass2 = ui->lePassword2->text();

    if (dni.isEmpty() || pass.isEmpty() || pass2.isEmpty()) {
        ui->lblMensaje->setText("Complete todos los campos");
        return;
    }

    if (pass != pass2) {
        ui->lblMensaje->setText("Las contraseñas no coinciden");
        return;
    }

    if (socket->state() != QAbstractSocket::ConnectedState) {
        socket->connectToHost("127.0.0.1", 1234);
        if (!socket->waitForConnected(3000)) {
            ui->lblMensaje->setText("No se pudo conectar");
            return;
        }
    }

    socket->write(QString("REGISTER|%1|%2|%3").arg(dni, pass, ui->cbMunicipio->currentText()).toUtf8());
    socket->flush();
    accept();
}
