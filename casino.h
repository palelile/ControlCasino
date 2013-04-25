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
//#include <dbsinc.h>
#include <csv_lector.h>
#include <dblocal.h>
#include <dbsinc.h>

namespace Ui {
class Casino;
}

class Casino : public QMainWindow
{
	Q_OBJECT

public:
	explicit Casino(QWidget *parent = 0);
	~Casino();

private:
    dbLocal * mi_db;
	Ui::Casino *ui;
//	dbSinc * sincronizador;
	int dbAlumnosId;
    bool agregarRegistroDb( QString rut, int opcion );
	bool hayAsistenciaPrevia(QString rut , int opcion);
	void mensajeErrorAsistencia(int opcion, QString alumno);

	void mensajeAutorizar(int opcion, QString alumno);
    void guardarOpciones();
	void cargarOpciones();
	QSqlQueryModel * generarReporte(int tipo);
	QString construirHtmlReporte(int tipo, QSqlQueryModel * modelo);

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
