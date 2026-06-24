#ifndef DASHBOARD_H
#define DASHBOARD_H
#include <QMainWindow>
#include <QTcpSocket>
#include <QButtonGroup>
#include <QSet>
#include <QVector>
#include <QLabel>

class Select;

namespace Ui {
class Dashboard;
}

class Dashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit Dashboard(QTcpSocket *socket, const QString &name, const QString &depart, const QString &token, QWidget *parent = nullptr);
    ~Dashboard();

private slots:
    void recibirRespuestaServidor();
    void enviarVoto();
    void on_ComboPlanillas_currentIndexChanged(int index);

private:
    Ui::Dashboard *ui;
    QTcpSocket *socket;

    QString voterName;
    QString voterToken;
    QString voterMunicipio;

    QButtonGroup *bgPres;

    QVector<Select*> dipWidgets;
    QVector<Select*> alcWidgets;

    QSet<int> diputadosSeleccionados;
    QSet<int> alcaldeSeleccionado;

    static const int MAX_DIPUTADOS = 23;

    void seleccionarDiputado(int id);
    void seleccionarAlcalde(int id);
    void ajustarAlcaldesMunicipio(const QString &municipio);
    int obtenerNumeroCandidatos(const QString &municipio);
    void setImage(QLabel *label, const QString &path);

    QVector<Select*> labelsExtra;
};

#endif
