#include "casino.h"
#include "ui_casino.h"

Casino::Casino(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Casino)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	mi_db = dbLocal::getInstance();
	mi_db->conectar("db_local.db");
	ui->setupUi(this);
	cargarOpciones();

	connect(ui->toolBox, SIGNAL(currentChanged(int)), this, SLOT(cambioDePagina(int)));
	connect(ui->bot_imp_sige, SIGNAL(pressed()), this, SLOT(importar_Sige()));
	connect(ui->salir, SIGNAL(pressed()), this, SLOT(close()) );
	connect(ui->asistenciaRut, SIGNAL(returnPressed()), this, SLOT(agregarAsistencia()));
	connect(ui->autorizarRut, SIGNAL(returnPressed()), this, SLOT(autorizarRut()) );
	connect(ui->desautorizarRut, SIGNAL(returnPressed()), this, SLOT(desAutorizarRut()) );
	connect(ui->asistenciaDes, SIGNAL(pressed()), ui->asistenciaRut, SLOT(setFocus()) );
	connect(ui->asistenciaAlm, SIGNAL(pressed()), ui->asistenciaRut, SLOT(setFocus()) );
	connect(ui->synSincronizar, SIGNAL(pressed()), this, SLOT(sincronizarDB()) );
	connect(ui->repOpcion, SIGNAL(currentIndexChanged(int)), this, SLOT(poblarTabla()) );
	connect(ui->repFechaFin, SIGNAL(dateChanged(QDate)), this, SLOT(poblarTabla()) );
	connect(ui->repFechaInicio, SIGNAL(dateChanged(QDate)), this, SLOT(poblarTabla()) );
	connect(ui->botonGuardar, SIGNAL(pressed()), this, SLOT(GuardarReporte()) );
	connect(ui->botonImprimir, SIGNAL(pressed()), this, SLOT(ImprimirReporte()) );

	this->showMaximized();
	ui->toolBox->setCurrentIndex(0);
	ui->asistenciaRut->setFocus();
}

Casino::~Casino()
{
	guardarOpciones();
	delete ui;
}

// Guarda las opciones del programa de forma permanente
void Casino::guardarOpciones()
{
	QSettings opciones(QSettings::IniFormat, QSettings::UserScope, "Ashtaroth", "AsistenciaCasino");
	opciones.beginGroup("DB");
	opciones.setValue("Direccion", ui->syncDireccion->text());
	opciones.setValue("Puerto", ui->syncPuerto->text());
	opciones.setValue("DB", ui->syncDb->text());
	opciones.setValue("Usuario", ui->syncUsuario->text());
	opciones.setValue("Password", ui->syncPassword->text());
	opciones.setValue("IDAlumnos", dbAlumnosId);
	opciones.endGroup();
}

// Carga las opciones del programa
void Casino::cargarOpciones(){
	QSettings opciones(QSettings::IniFormat, QSettings::UserScope, "Ashtaroth", "AsistenciaCasino");
	opciones.beginGroup("DB");
	ui->syncDireccion->setText( opciones.value("Direccion").toString() );
	ui->syncPuerto->setText( opciones.value("Puerto").toString() );
	ui->syncDb->setText( opciones.value("DB").toString() );
	ui->syncUsuario->setText( opciones.value("Usuario").toString() );
	ui->syncPassword->setText( opciones.value("Password").toString() );
	dbAlumnosId = opciones.value("IDAlumnos").toInt();
	opciones.endGroup();
}

void Casino::cambioDePagina(int pagina){
	switch (pagina) {
	case 0:
	{
		QTime medioDia(12, 0, 0, 0);
		if ( QTime::currentTime().operator <(medioDia) )
			ui->asistenciaDes->setChecked(true);
		else
			ui->asistenciaAlm->setChecked(true);
		ui->asistenciaRut->setFocus();
		break;
	}
	case 1:
		ui->autorizarRut->setFocus();
		break;
	case 2:
	{
		QDate hoy = QDate::currentDate();
		ui->repFechaFin->setDate( hoy );
		ui->repFechaInicio->setDate( hoy.addDays(-7) );
		ui->repOpcion->setCurrentIndex( 0 );
		poblarTabla();
	}
		break;
	case 3:
	{
		ui->synSincronizar->setFocus();
		ui->progressBar->setVisible( false );
	}
		break;
	default:
		break;
	}
}

bool Casino::hayAsistenciaPrevia( QString rut, int opcion ){
	QString cond = "rut='"+rut+"' AND fecha='"+QDate::currentDate().toString("yyyy-MM-dd")+"'";
	if (opcion == 1)
		return mi_db->buscar("id", "asistenciadesayuno", cond);
	return mi_db->buscar("id", "asistenciaalmuerzo", cond);
}

bool Casino::agregarRegistroDb( QString rut, int opcion ){
	QString fecha = QDate::currentDate().toString("yyyy-MM-dd");
	QString valores = "'"+rut+"','"+fecha+"'";
	switch ( opcion )
	{
	case 1:
		if ( !hayAsistenciaPrevia( rut, 1 ) )
			return mi_db->agregar("asistenciadesayuno", "rut,fecha", valores);
		break;
	case 2:
		if ( !hayAsistenciaPrevia( rut, 2 ) )
			return mi_db->agregar("asistenciaalmuerzo", "rut,fecha", valores);
		break;
	}
	return false;
}

bool Casino::agregarAsistencia(){
	QString rut = ui->asistenciaRut->text();
	if ( !mi_db->buscar("id", "alumnos", "rut='"+rut+"'") ){
		mensajeErrorAsistencia(1 , "");
		return false;
	}
	QString alumno = mi_db->buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
								   "alumnos",
								   "rut='"+rut+"'")->at(0);
	if (!mi_db->buscar("id", "autorizados", "rut='"+rut+"'"))
	{
		mensajeErrorAsistencia(2, alumno);
		return false;
	}

	bool estado = false;
	if ( ui->asistenciaDes->isChecked() )
		estado = agregarRegistroDb( rut, 1 );
	else
		estado = agregarRegistroDb( rut, 2 );

	if (! estado)
		mensajeErrorAsistencia(3, alumno);
	else
		mensajeErrorAsistencia(0, alumno);
	return estado;
}

// Mensaje de información de error por ventana
void Casino::mensajeErrorAsistencia(int opcion, QString alumno) {
	QString mensaje = "";
	switch (opcion) {
	case 0:
		mensaje = alumno + "<br />se ha ingresado a la asistencia.";
		break;
	case 1:
		mensaje = "RUT no pertenece a ningún alumno";
		break;
	case 2:
		mensaje = alumno + ",<br />NO autorizado.";
		break;
	case 3:
		mensaje = alumno + ",<br />ya registra asistencia.";
		break;
	default:
		mensaje = "Error sin definir.";
		break;
	}
	ui->asistenciaInfo->setText(mensaje);
	if ( opcion > 0 )
		ui->asistenciaInfo->setStyleSheet("font-size : 26px; background-color : #BC3A00; color : #FFFFFF;");
	else
		ui->asistenciaInfo->setStyleSheet("font-size : 26px; background-color : GREEN; color : #FFFFFF;");
	ui->asistenciaRut->setText(NULL);
}

void Casino::autorizarRut( ){
	QString rut = ui->autorizarRut->text();
	ui->autorizarRut->setText(NULL);
	if ( !mi_db->buscar("id", "alumnos", "rut='"+rut+"'") )
		return mensajeAutorizar(1, "");
	QString alumno = mi_db->buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
								   "alumnos",
								   "rut='"+rut+"'")->at(0);
	if ( !mi_db->buscar("id", "autorizados", "rut='"+rut+"'") )
	{
		mi_db->agregar("autorizados", "rut", rut);
		return mensajeAutorizar(0, alumno);
	}
	else
		return mensajeAutorizar(3, alumno);
}

void Casino::desAutorizarRut(){
	QString rut = ui->desautorizarRut->text();
	ui->desautorizarRut->setText(NULL);
	if ( !mi_db->buscar("id", "alumnos", "rut='"+rut+"'") )
		return mensajeAutorizar(1, "");
	QString alumno = mi_db->buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
								   "alumnos",
								   "rut='"+rut+"'")->at(0);
	if ( !mi_db->buscar("id", "autorizados", "rut='"+rut+"'") )
		return mensajeAutorizar(2, alumno);
	mi_db->borrar("autorizados", "rut='"+rut+"'");
	mensajeAutorizar(-1, alumno);
}

// Mensaje de información de error por ventana
void Casino::mensajeAutorizar(int opcion, QString alumno) {
	QString mensaje = "";
	switch (opcion) {
	case -1:
		mensaje = alumno + "<br />se ha desautorizado.";
		break;
	case 0:
		mensaje = alumno + "<br />se ha autorizado.";
		break;
	case 1:
		mensaje = "RUT no pertenece a ningún alumno";
		break;
	case 2:
		mensaje = alumno + ",<br /> no autorizado.";
		break;
	case 3:
		mensaje = alumno + ",<br /> ya está autorizado.";
		break;
	default:
		mensaje = "Error sin definir.";
		break;
	}
	ui->autorizarInfo->setText(mensaje);
	if ( opcion > 0 )
		ui->autorizarInfo->setStyleSheet("font-size : 26px; background-color : #BC3A00; color : #FFFFFF;");
	else
		ui->autorizarInfo->setStyleSheet("font-size : 26px; background-color : GREEN; color : #FFFFFF;");
}


void Casino::sincronizarDB()
{
	dbSinc * sincronizador = NULL;
	sincronizador = new dbSinc();
	ui->progressBar->setVisible( true );

	if ( sincronizador->conectarRemota(ui->syncDireccion->text(),
									   ui->syncPuerto->text(),
									   ui->syncDb->text(),
									   ui->syncUsuario->text(),
									   ui->syncPassword->text()))
	{
		sincronizador->SubirTabla("alumnos",
								  "alumnos",
								  "rut,nombre,apellidoP,apellidoM,curso");
		ui->progressBar->setValue(25);

		sincronizador->SubirTabla("asistenciadesayuno",
								  "asistenciaCasinoDesayuno",
								  "rut,fecha");
		ui->progressBar->setValue(50);

		sincronizador->SubirTabla("asistenciaalmuerzo", "asistenciaCasinoAlmuerzo", "rut,fecha");
		ui->progressBar->setValue(75);

		sincronizador->SubirTabla("autorizados",
								  "autorizadosCasino",
								  "rut");
		ui->progressBar->setValue(100);
	}
}

QSqlQueryModel * Casino::generarReporte(int tipo)
{
	QString fechaInicio = ui->repFechaInicio->date().toString("yyyy-MM-dd");
	QString fechaFin = ui->repFechaFin->date().toString("yyyy-MM-dd");
	QString cond = "fecha>='"+fechaInicio+"' AND fecha<='"+fechaFin+"'";
	QSqlQueryModel * modelo = NULL;
	switch ( tipo )
	{
	case 0:     // Asistencia diaria al desayuno
		modelo = mi_db->modeloConsulta(this, "SELECT fecha AS Fecha,count(fecha) AS Cantidad FROM asistenciadesayuno WHERE "
									   +cond+" GROUP BY fecha");
		break;
	case 1:     // Asistencia diaria al almuerzo
		modelo = mi_db->modeloConsulta(this, "SELECT fecha AS Fecha,count(fecha) AS Cantidad FROM asistenciaalmuerzo WHERE "
									   +cond+" GROUP BY fecha");
		break;
	case 2:     // Asistencia por alumno a los desayunos
		modelo = mi_db->modeloConsulta(this, "SELECT alumnos.curso||' - '||alumnos.nombre||' '||alumnos.apellidoP||' '||alumnos.apellidoM AS Alumno,count(fecha) AS Cantidad "
									   " FROM asistenciadesayuno,alumnos "
									   " WHERE alumnos.rut=asistenciadesayuno.rut "
									   " AND " +cond+
									   " GROUP BY asistenciadesayuno.rut "
									   " ORDER BY alumnos.curso,alumnos.apellidoP;");
		break;
	case 3:     // Asistencia por alumno al almuerzo
		modelo = mi_db->modeloConsulta(this, "SELECT alumnos.curso||' - '||alumnos.nombre||' '||alumnos.apellidoP||' '||alumnos.apellidoM AS Alumno,count(fecha) AS Cantidad "
									   " FROM asistenciaalmuerzo,alumnos "
									   " WHERE alumnos.rut=asistenciaalmuerzo.rut "
									   " AND "+cond+
									   " GROUP BY asistenciaalmuerzo.rut "
									   " ORDER BY alumnos.curso,alumnos.apellidoP;");
		break;
	case 4:     // Alumnos autorizados para usar el casino
		modelo = mi_db->modeloConsulta(this,
									   " SELECT alumnos.curso AS Curso,alumnos.rut AS RUT,alumnos.nombre||' '||alumnos.apellidoP||' '||alumnos.apellidoM AS Alumno "
									   " FROM autorizados,alumnos "
									   " WHERE alumnos.rut=autorizados.rut "
									   " ORDER BY alumnos.curso,alumnos.apellidoP;");
		break;
	default:
		break;
	}

	return modelo;
}

void Casino::poblarTabla()
{
	QSqlQueryModel * modelo = generarReporte( ui->repOpcion->currentIndex() );
	ui->repTabla->verticalHeader()->hide();
	ui->repTabla->setShowGrid(true);
	ui->repTabla->setModel( modelo );
	ui->repTabla->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui->repTabla->resizeColumnsToContents();
}

void Casino::GuardarReporte()
{
	int tipo = ui->repOpcion->currentIndex();
	QSqlQueryModel * modelo = (QSqlQueryModel *)ui->repTabla->model();
	QString strStream = construirHtmlReporte(tipo, modelo);

	QTextDocument *documento = new QTextDocument();
	documento->setHtml(strStream);

	QPrinter printer;
	QString s = QFileDialog::getSaveFileName(
					this,
					"Elija el archivo donde se guardará el documento",
					"",
					"Archivo PDF (*.pdf)"
					);
	if (s.isEmpty())
		return;
	printer.setOutputFileName( s );
	printer.setPageSize(QPrinter::Letter);
	printer.setPaperSize(QPrinter::Letter);
	printer.setFullPage( false );
	printer.setPageMargins(20,20,20,20,QPrinter::Millimeter);
	printer.setOutputFormat( QPrinter::PdfFormat );
	documento->print(&printer);

	delete documento;
}

QString Casino::construirHtmlReporte(int tipo, QSqlQueryModel * modelo){
	QString strStream = "";
	QString fechaInicio = ui->repFechaInicio->date().toString("dd.MM.yyyy");
	QString fechaFin = ui->repFechaFin->date().toString("dd.MM.yyyy");

	strStream.append("<html><head>"
	"<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />"
	"<style type=\"text/css\">*{font-size: 9px;}table{margin: 5px;}td { padding: 3px; text-align: right;}th { padding: 5px; text-align: center;}</style>"
	"</head><body>");
	switch ( tipo )
	{
	case 0:
		strStream.append("<h1>Reporte de asistencia diaria<br />desayuno</h1>");
		break;
	case 1:
		strStream.append("<h1>Reporte de asistencia diaria<br />almuerzo</h1>");
		break;
	case 2:
		strStream.append("<h1>Reporte de asistencia por alumno<br />desayuno</h1>");
		break;
	case 3:
		strStream.append("<h1>Reporte de asistencia por alumno<br />almuerzo</h1>");
		break;
	case 4:
		strStream.append("<h1>Alumnos autorizados</h1>");
		break;
	}
	strStream.append("<p>Fecha inicio: <br />" + fechaInicio +
	"<p>Fecha fin: <br />" + fechaFin +
	"<table style=\"padding:5px;\">");

	// headers
	strStream.append("<tr>");
	for (int i = 0; i < modelo->columnCount(); i++)
		strStream.append("<th>" + modelo->headerData(i, Qt::Horizontal).toString() + "</th>");
	strStream.append("</tr>");

	// data table
	for (int i = 0; i < modelo->rowCount(); i++) {
		strStream.append("<tr>");
		for (int j = 0; j < modelo->columnCount(); j++) {
			QString data = modelo->record(i).value(j).toString();
			strStream.append("<td>" +data+ "</td>" );
		}
		strStream.append("</tr>");
	}
	strStream.append("</table></body></html>");
	return strStream;
}

void Casino::ImprimirReporte()
{
	int tipo = ui->repOpcion->currentIndex();
	QSqlQueryModel * modelo = (QSqlQueryModel *)ui->repTabla->model();
	QString strStream = construirHtmlReporte(tipo, modelo);

	QTextDocument *document = new QTextDocument();
	document->setHtml(strStream);

	QPrinter printer;
	printer.setPageSize(QPrinter::Letter);
	printer.setPaperSize(QPrinter::Letter);
	printer.setPageMargins(20,20,20,20,QPrinter::Millimeter);
	QPrintDialog *dialog = new QPrintDialog(&printer, NULL);
	if (dialog->exec() == QDialog::Accepted) {
		document->print(&printer);
	}

	delete document;
}


/*******************************************************************************
 * Funciones de importación de datos
 ******************************************************************************/
void Casino::importar_Sige()
{
	QString archivo = QFileDialog::getOpenFileName(
						  this,
						  "Abrir archivo",
						  QDir::currentPath(),
						  "Archivo TXT de SIGE (*.txt)" );
	if( archivo.isNull() )
		return;
	else
	{
		csv_lector csv;
		csv.leer_archivo_db( archivo, mi_db );
	}
}
