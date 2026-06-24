#include "dashboard.h"
#include "ui_dashboard.h"
#include "select.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QSet>
#include <QVector>

Dashboard::Dashboard(QTcpSocket *sock, const QString &name, const QString &depart, const QString &token, QWidget *parent):QMainWindow(parent), ui(new Ui::Dashboard), socket(sock), voterName(name), voterToken(token), bgPres(nullptr){
    ui->setupUi(this);

    bgPres = new QButtonGroup(this);
    bgPres->setExclusive(true);

    bgPres->addButton(ui->voteC1, 1);
    bgPres->addButton(ui->voteC2, 2);
    bgPres->addButton(ui->voteC3, 3);
    bgPres->addButton(ui->voteC4, 4);
    bgPres->addButton(ui->voteC5, 5);

    ui->widget->setStyleSheet("background-color: white;" "color: black;");

    setImage(ui->lblCNE, ":/img/ELECCIONES/1.svg.png");
    setImage(ui->lblELE, ":/img/ELECCIONES/2.jpg");
    setImage(ui->lbl25, ":/img/ELECCIONES/3.jpg");
    setImage(ui->lblCandidatos, ":/img/ELECCIONES/4.jpg");

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        QDateTime now = QDateTime::currentDateTime();
        ui->lblFechaHora->setText(now.toString("dd/MM/yyyy  hh:mm:ss"));
    });
    timer->start(1000);

    connect(ui->btnConfirmar, &QPushButton::clicked, this, &Dashboard::enviarVoto);

    connect(socket, &QTcpSocket::readyRead, this, &Dashboard::recibirRespuestaServidor);

    QList<Select*> dips = findChildren<Select*>();

    for (Select *w : dips) {
        QVariant v = w->property("dip_id");
        if (!v.isValid()) continue;

        int id = v.toInt();
        if (id <= 0) continue;

        dipWidgets.append(w);

        connect(w, &Select::clicked, this, [=]() {
            seleccionarDiputado(id);
        });
    }

    QList<Select*> alc = findChildren<Select*>();

    for (Select *w : alc) {
        QVariant v = w->property("alc_id");
        if (!v.isValid()) continue;

        int id = v.toInt();
        if (id <= 0) continue;

        alcWidgets.append(w);

        connect(w, &Select::clicked, this, [=]() {
            seleccionarAlcalde(id);
        });
    }

}

void Dashboard::setImage(QLabel *label, const QString &path){
    QPixmap pix(path);
    label->setPixmap(pix.scaled(label->width(),label->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Dashboard::enviarVoto()
{
    int idPres = bgPres->checkedId();
    if (idPres == -1) {
        QMessageBox::warning(this, "Error", "Debe seleccionar un presidente");
        return;
    }

    if (diputadosSeleccionados.isEmpty()) {
        QMessageBox::warning(this, "Error", "Debe seleccionar al menos un diputado");
        return;
    }

    QStringList dipList;
    for (int id : diputadosSeleccionados)
        dipList << QString::number(id);

    QString alcId = "0";
    if (!alcaldeSeleccionado.isEmpty())
        alcId = QString::number(*alcaldeSeleccionado.begin());

    QString mensaje = "VOTO|" + QString::number(idPres) + "|" + dipList.join(",") + "|" + alcId + "|" + voterToken;
    socket->write(mensaje.toUtf8());
    socket->flush();

    QMessageBox::information(this, "Voto", "Voto enviado correctamente");
}

void Dashboard::seleccionarDiputado(int id)
{
    Select *w = nullptr;

    for (Select *s : dipWidgets) {
        if (s->property("dip_id").toInt() == id) {
            w = s;
            break;
        }
    }

    if (!w) return;

    if (diputadosSeleccionados.contains(id)) {
        diputadosSeleccionados.remove(id);
        w->clear();
        return;
    }

    //Limite
    if (diputadosSeleccionados.size() >= MAX_DIPUTADOS) {
        QMessageBox::warning(this, "Límite alcanzado", "Solo puede seleccionar hasta 23 diputados");
        return;
    }

    diputadosSeleccionados.insert(id);
    w->setPixmap(QPixmap(":/img/ELECCIONES/x.png"));
}

void Dashboard::seleccionarAlcalde(int id)
{
    Select *w = nullptr;

    for (Select *s : alcWidgets) {
        if (s->property("alc_id").toInt() == id) {
            w = s;
            break;
        }
    }

    if (!w) return;

    //Solo un alcalde
    if (alcaldeSeleccionado.contains(id)) {
        alcaldeSeleccionado.clear();
        w->clear();
        return;
    }

    for (Select *s : alcWidgets)
        s->clear();

    alcaldeSeleccionado.clear();
    alcaldeSeleccionado.insert(id);

    w->setPixmap(QPixmap(":/img/ELECCIONES/x.png"));
}

void Dashboard::ajustarAlcaldesMunicipio(const QString &municipio)
{
    QList<Select*> labelsBase = { ui->ALC, ui->ALC_2, ui->ALC_3, ui->ALC_4, ui->ALC_5 };

    for (Select* l : labelsBase) {
        l->clear();
        l->show();
    }

    for (Select* l : labelsExtra) {
        l->hide();
        l->deleteLater();
    }
    labelsExtra.clear();

    int numCandidatos = obtenerNumeroCandidatos(municipio); // 4,5 o 6

    if (numCandidatos == 4) {
        labelsBase[2]->hide();
    }
    else if (numCandidatos == 6) {
        Select *nuevo = new Select(this);
        nuevo->setGeometry(ui->ALC_5->geometry().translated(-100,0)); // ajusta la posición
        nuevo->setProperty("alc_id", 6); // nuevo ID
        nuevo->show();
        labelsExtra.append(nuevo);

        connect(nuevo, &Select::clicked, this, [=]() { seleccionarAlcalde(6); });
    }
}

int Dashboard::obtenerNumeroCandidatos(const QString &municipio)
{
    if (municipio == "Tegucigalpa") return 5;
    if (municipio == "Tatumbla") return 6;
    if (municipio == "Santa Lucía") return 4;
    // Agrega el resto de municipios según tus datos
    return 5; // default
}


void Dashboard::on_ComboPlanillas_currentIndexChanged(int index)
{
    ui->stackPlanillas->setCurrentIndex(index);

    if (index == 2) {
        ajustarAlcaldesMunicipio(voterMunicipio); // tu municipio del login
    }
}

void Dashboard::recibirRespuestaServidor(){
    QString data = socket->readAll();
    qDebug() << "Servidor:" << data;

    if (data.startsWith("OK|VOTO_REGISTRADO")) {
        QMessageBox::information(this, "Voto", "Voto registrado exitosamente");

        ui->btnConfirmar->setEnabled(false);
        ui->btnEstadisticas->setEnabled(true);
    }
    else if (data.startsWith("ERROR|Ya votó")) {
        QMessageBox::warning(this, "Error", "Este usuario ya votó");
        ui->btnConfirmar->setEnabled(false);
    }
}



Dashboard::~Dashboard()
{
    delete ui;
}
