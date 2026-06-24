#include "principal.h"
#include "ui_principal.h"
#include <QDebug>
#include <QCryptographicHash>
#include <QUuid>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QDateTime>


QString hashPassword(const QString &pass, const QByteArray &salt){
    QByteArray hash = pass.toUtf8() + salt;
    for (int i = 0; i < 100000; ++i)
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    return hash.toHex();
}

Principal::Principal(QWidget *parent):QMainWindow(parent),ui(new Ui::Principal){
    ui->setupUi(this);

    connect(ui->btnActualizar, &QPushButton::clicked, this, &Principal::cargarUsuarios);
    connect(ui->btnActualizar, &QPushButton::clicked, this, &Principal::mostrarUsuariosQueVotaron);
    connect(ui->btnActualizar, &QPushButton::clicked, this, &Principal::mostrarResultados);
    connect(ui->btnCerrar, &QPushButton::clicked, this, &QMainWindow::close);

    servidor = new QTcpServer(this);
    if(!servidor->listen(QHostAddress::Any, 1234))
        qDebug() << "Servidor NO iniciado";
    else
        qDebug() << "Servidor conectado en el puerto 1234 =)";

    connect(servidor, &QTcpServer::newConnection, [=]() {
        cliente = servidor->nextPendingConnection();
        qDebug() << "Cliente conectado desde:" << cliente->peerAddress().toString();

        connect(cliente, &QTcpSocket::readyRead, [=]() {
            QByteArray raw = cliente->readAll();
            QString data = QString::fromUtf8(raw).trimmed();

            qDebug() << "Datos recibidos:" << data;

            QStringList partes = data.split("|");
            QString cmd = partes.value(0);
            if(partes.length() < 1){
                cliente->write(QString("ERROR|Trama vacía").toUtf8());
                cliente->flush();
                return;
            }

            if (cmd == "REGISTER") {

                if (partes.size() < 4) {
                    cliente->write("ERROR|REGISTER incompleto");
                    cliente->flush();
                    return;
                }

                QString dni = partes[1];
                QString pass = partes[2];
                QString municipio = partes[3];

                QFile file("users.dat");

                if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream in(&file);
                    while (!in.atEnd()) {
                        if (in.readLine().startsWith(dni + "|")) {
                            cliente->write("ERROR|Usuario ya existe");
                            cliente->flush();
                            return;
                        }
                    }
                    file.close();
                }

                QByteArray salt(16, 0);
                QRandomGenerator::global()->generate(salt.begin(), salt.end());

                QString hash = hashPassword(pass, salt);

                if (!file.open(QIODevice::Append | QIODevice::Text)) {
                    cliente->write("ERROR|No se pudo guardar");
                    cliente->flush();
                    return;
                }

                QTextStream out(&file);
                out<<dni<<"|"<<salt.toHex()<<"|"<<hash<<"|"<< municipio<<"\n";

                file.close();

                cliente->write("OK|REGISTER_OK");
                cliente->flush();
                return;
            }

            else if (cmd == "LOGIN") {

                if (partes.size() < 3) {
                    cliente->write("ERROR|LOGIN incompleto");
                    cliente->flush();
                    return;
                }

                QString dni = partes[1];
                QString pass = partes[2];

                QFile file("users.dat");
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    cliente->write("ERROR|No hay usuarios");
                    cliente->flush();
                    return;
                }

                QTextStream in(&file);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    QStringList p = line.split("|");

                    if (p.size() < 4) continue;

                    if (p[0] == dni) {
                        QByteArray salt = QByteArray::fromHex(p[1].toUtf8());
                        QString hash = hashPassword(pass, salt);

                        if (hash == p[2]) {
                            QString municipio = p[3];

                            QString token = QUuid::createUuid().toString();
                            tokens[token] = dni;

                            cliente->write(
                                QString("OK|%1|Francisco Morazán|%2|%3")
                                    .arg(dni)
                                    .arg(municipio)
                                    .arg(token)
                                    .toUtf8()
                                );
                            cliente->flush();
                            file.close();
                            return;
                        }
                    }
                }

                file.close();
                cliente->write("ERROR|Credenciales incorrectas");
                cliente->flush();
            }



            else if (cmd == "VOTO") {

                if (partes.size() < 5) {
                    cliente->write("ERROR|VOTO incompleto");
                    cliente->flush();
                    return;
                }
                int presId = partes[1].toInt();
                QString dips = partes[2];
                QString alc  = partes[3];
                QString token = partes[4];

                if (!tokens.contains(token)) {
                    cliente->write("ERROR|Token inválido");
                    cliente->flush();
                    return;
                }

                QString dni = tokens[token];
                if (usuariosQueVotaron.contains(dni)) {
                    cliente->write("ERROR|Ya votó");
                    cliente->flush();
                    return;
                }

                usuariosQueVotaron.insert(dni);
                for(int x = 0; x < 2; x++){
                    votosPresidente[presId]++;
                }

                QFile f("resultados.dat");
                if (f.open(QIODevice::Append | QIODevice::Text)) {
                    QTextStream out(&f);
                    out << dni << "|" << presId << "|" << dips << "|" << alc << "|"<< QDateTime::currentDateTime().toString() << "\n";
                    f.close();
                }

                cliente->write("OK|VOTO_REGISTRADO");
                cliente->flush();
            }

        });
    });
}

void Principal::cargarUsuarios(){
    ui->tableUsuarios->setRowCount(0);

    QFile file("users.dat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    int row = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList p = line.split("|");

        if (p.size() < 4) continue;

        ui->tableUsuarios->insertRow(row);
        ui->tableUsuarios->setItem(row, 0, new QTableWidgetItem(p[0]));
        ui->tableUsuarios->setItem(row, 1, new QTableWidgetItem(p[3]));
        row++;
    }

    file.close();
}

void Principal::mostrarUsuariosQueVotaron(){
    ui->listVotaron->clear();

    for (const QString &dni : usuariosQueVotaron)
        ui->listVotaron->addItem(dni);
}


void Principal::mostrarResultados(){
    ui->tableResultados->setRowCount(0);
    int row = 0;

    for (auto it = votosPresidente.begin(); it != votosPresidente.end(); ++it) {
        ui->tableResultados->insertRow(row);
        ui->tableResultados->setItem(
            row, 0,
            new QTableWidgetItem("Presidente " + QString::number(it.key()))
            );
        ui->tableResultados->setItem(
            row, 1,
            new QTableWidgetItem(QString::number(it.value()))
            );
        row++;
    }
}




Principal::~Principal()
{
    delete ui;
}
