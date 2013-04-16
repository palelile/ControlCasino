#ifndef CASINO_H
#define CASINO_H

#include <QMainWindow>
#include <QTextCodec>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QString>
#include <QTextStream>
#include <QTextDocument>
#include <QFile>
#include <QPrinter>
#include <QPrintDialog>
#include <QSettings>
#include <QStringList>
#include <QDebug>
#include <QFileDialog>
#include "dbcasino.h"
#include "dbsinc.h"
#include "csv_lector.h"

namespace Ui {
class Casino;
}

class Casino : public QMainWindow, dbCasino
{
	Q_OBJECT

public:
	explicit Casino(QWidget *parent = 0);
	~Casino();

private:
	Ui::Casino *ui;
	dbSinc * sincronizador;
	int dbAlumnosId;
    bool validarAutorizacion(QString rut);
    bool agregarRegistroDb( QString rut, int opcion );
	bool hayAsistenciaPrevia(QString rut , int opcion);
	void mensajeErrorAsistencia(int opcion, QString alumno);

	void mensajeAutorizar(int opcion, QString alumno);
    void guardarOpciones();
	void cargarOpciones();
    QList<QString*> generarReporte(int *nAlumnos, int tipo, int *fin);

private slots:
    bool agregarAsistencia();
    void autorizarRut();
    void desAutorizarRut();
    void sincronizarDB();
    void cambioDePagina(int pagina);
    void poblarTabla();
    void GuardarReporte();
    void ImprimirReporte();
	void importar_Sige();
};

#endif // CASINO_H
