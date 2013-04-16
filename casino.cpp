#include "casino.h"
#include "ui_casino.h"

Casino::Casino(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Casino)
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf8"));
	conectar("db_local.db");
	ui->setupUi(this);
	cargarOpciones();

	connect(ui->toolBox, SIGNAL(currentChanged(int)), this, SLOT(cambioDePagina(int)));
	connect(ui->bot_imp_sige, SIGNAL(pressed()), this, SLOT(importar_Sige()));

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

// Convierte una cadena de texto de entrada a cada palabra con la primera letra
// en mayuscula
QString cap(const QString &str)
{
	QString tmp = str;
	tmp = tmp.toLower();
	tmp[0] = str[0].toUpper();
	for ( int i = 1; i < tmp.length(); i++ )
		if ( tmp[i-1] == ' ' )
			tmp[i] = str[i].toUpper();
	tmp += " ";
	return tmp;
}

// Verifica que el rut de entrada se encuentr en el listado de autorizados
bool Casino::validarAutorizacion(QString rut)
{
	return buscar("id", "autorizados", "rut='"+rut+"'");
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
		return buscar("id", "asistenciadesayuno", cond);
	return buscar("id", "asistenciaalmuerzo", cond);
}

bool Casino::agregarRegistroDb( QString rut, int opcion ){
	QDate fecha = QDate::currentDate();
	QString valores = "'"+rut+"','"+fecha.toString("yyyy-MM-dd")+"'";
	switch ( opcion )
	{
	case 1:
		if ( !hayAsistenciaPrevia( rut, 1 ) )
			return agregar("asistenciadesayuno", "rut,fecha", valores);
		break;
	case 2:
		if ( !hayAsistenciaPrevia( rut, 2 ) )
			return agregar("asistenciaalmuerzo", "rut,fecha", valores);
		break;
	}
	return false;
}

bool Casino::agregarAsistencia(){
	QString rut = ui->asistenciaRut->text();
	if ( !buscar("id", "alumnos", "rut='"+rut+"'") ){
		mensajeErrorAsistencia(1 , "");
		return false;
	}
	if (!validarAutorizacion( rut )){
		mensajeErrorAsistencia(2, buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
										 "alumnos",
										 "rut='"+rut+"'")->at(0));
		return false;
	}
	bool estado = false;
	if ( ui->asistenciaDes->isChecked() )
		estado = agregarRegistroDb( rut, 1 );
	else
		estado = agregarRegistroDb( rut, 2 );
	if (! estado)
		mensajeErrorAsistencia(3, buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
										 "alumnos",
										 "rut='"+rut+"'")->at(0));
	else
		mensajeErrorAsistencia(0, buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
										 "alumnos",
										 "rut='"+rut+"'")->at(0));
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
	if ( !buscar("id", "alumnos", "rut='"+rut+"'") )
		mensajeAutorizar(1, "");
	if ( !validarAutorizacion( rut ) )
	{
		agregar("autorizados", "rut", rut);
		return mensajeAutorizar(0, buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
								   "alumnos",
								   "rut='"+rut+"'")->at(0));
	}
	else
		return mensajeAutorizar(3, buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
								   "alumnos",
								   "rut='"+rut+"'")->at(0));
}

void Casino::desAutorizarRut(){
	QString rut = ui->desautorizarRut->text();
	ui->desautorizarRut->setText(NULL);
	if ( !buscar("id", "alumnos", "rut='"+rut+"'") )
		return mensajeAutorizar(1, "");
	if ( !validarAutorizacion( rut ) )
		return mensajeAutorizar(2, buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
										  "alumnos",
										  "rut='"+rut+"'")->at(0));
	borrar("autorizados", "rut='"+rut+"'");
	mensajeAutorizar(-1, buscar("nombre||' '||apellidoP||' '||apellidoM||' '||curso AS info",
											  "alumnos",
											  "rut='"+rut+"'")->at(0));
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
	sincronizador = new dbSinc();
	ui->progressBar->setVisible( true );

	if ( sincronizador->conectarLocal("datos.db") &&
		 sincronizador->conectarRemota(ui->syncDireccion->text(),
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
	sincronizador->desconectar();
	conectar("db_local.db");
}

QList<QString*> Casino::generarReporte(int *nAlumnos, int tipo, int *fin)
{
	QDate fechaInicio = ui->repFechaInicio->date();
	QDate fechaFin = ui->repFechaFin->date();
	QString cond = "fecha>='"+fechaInicio.toString("yyyy-MM-dd")+"' AND fecha<='"+fechaFin.toString("yyyy-MM-dd")+"'";
	QSqlQueryModel * modelo = NULL;
	QList<QString*> alumnos;
	switch ( tipo )
	{
	case 0:     // Asistencia diaria al desayuno
		modelo = modeloConsulta(this, "SELECT fecha,count(fecha) AS cantidad FROM asistenciadesayuno WHERE "
								+cond+" GROUP BY fecha");
		break;
	case 1:     // Asistencia diaria al almuerzo
		modelo = modeloConsulta(this,"SELECT fecha,count(fecha) FROM asistenciaalmuerzo WHERE "
								+cond+"GROUP BY fecha");
		break;
	case 2:     // Asistencia por alumno a los desayunos
		modelo = modeloConsulta(this, "SELECT alumnos.curso||' - '||alumnos.nombre||' '||alumnos.apellidoP||' '||alumnos.apellidoM,count(fecha) "
								" FROM asistenciadesayuno,alumnos "
								" WHERE alumnos.rut=asistenciadesayuno.rut "
								" AND " +cond+
								" GROUP BY asistenciadesayuno.rut "
								" ORDER BY alumnos.curso,alumnos.apellidoP;");
		break;
	case 3:     // Asistencia por alumno al almuerzo
		modelo = modeloConsulta(this, "SELECT alumnos.curso||' - '||alumnos.nombre||' '||alumnos.apellidoP||' '||alumnos.apellidoM,count(fecha) "
								" FROM asistenciaalmuerzo,alumnos "
								" WHERE alumnos.rut=asistenciaalmuerzo.rut "
								" AND "+cond+
								" GROUP BY asistenciaalmuerzo.rut "
								" ORDER BY alumnos.curso,alumnos.apellidoP;");
		break;
	case 4:     // Alumnos autorizados para usar el casino
		modelo = modeloConsulta(this,
								" SELECT alumnos.curso,alumnos.rut,alumnos.nombre||' '||alumnos.apellidoP||' '||alumnos.apellidoM AS alumno "
								" FROM autorizados,alumnos "
								" WHERE alumnos.rut=autorizados.rut "
								" ORDER BY alumnos.curso,alumnos.apellidoP;");
		break;
	default:
		return alumnos;
		break;
	}

	QString * alumno;

	*fin = modelo->columnCount();
	*nAlumnos = modelo->rowCount();
	for ( int i = 0; i < modelo->rowCount(); i++ )
	{
		alumno = new QString[*fin];
		for ( int j = 0; j < *fin; j++ )
			alumno[j] = cap( modelo->record(i).value(j).toString() );
		alumnos.append(alumno);
	}

	return alumnos;
}

void Casino::poblarTabla()
{
	int indice = ui->repOpcion->currentIndex();
	QStringList labels;
	QTableWidget *tabla = ui->repTabla;
	int nAlumnos = 0;
	int columnas = 0;
	QList<QString*> alumnos = generarReporte( &nAlumnos, indice, &columnas );
	switch (indice){
	case 0:
		labels << "Fecha" << "Desayunos";
		break;
	case 1:
		labels << "Fecha" << "Almuerzos";
		break;
	case 2:
		labels << "Alumno" << "Desayunos";
		break;
	case 3:
		labels << "Alumno" << "Almuerzos";
		break;
	case 4:
		labels << "Curso" << "RUT" << "Alumno";
		break;
	}
	tabla->setColumnCount(columnas);
	tabla->setHorizontalHeaderLabels(labels);
	tabla->verticalHeader()->hide();
	tabla->setShowGrid(false);
	for (int i=0; tabla->rowCount() > 0; i++)
		tabla->removeRow(0);
	for (int i=0; i< nAlumnos; i++){
		int row = tabla->rowCount();
		tabla->insertRow( row );
		for (int j=0; j< columnas;j++)
			tabla->setItem(row, j, new QTableWidgetItem(alumnos[i][j], 0));
	}
	tabla->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	tabla->resizeColumnsToContents();
}

void Casino::GuardarReporte()
{
	int tipo_reporte = ui->repOpcion->currentIndex();
	QString documento_cadena;
	QTextStream documento_html(&documento_cadena);
	documento_html.setCodec("UTF-8");
	QTableWidget * tabla = ui->repTabla;

	const int cantidad_filas = tabla->model()->rowCount();
	const int cantidad_columnas = tabla->model()->columnCount();

	documento_html << "<html>\n";
	documento_html << "<head>\n";
	documento_html << "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />\n";
	documento_html << "<style type=\"text/css\">*{font-size: 9px;}table{margin: 5px;}td { padding: 3px; text-align: right;}th { padding: 5px; text-align: center;}</style>";
	documento_html <<  "</head>\n";
	documento_html << "<body>\n<center>";
	switch ( tipo_reporte )
	{
	case 0:
		documento_html << QString("<h1>Reporte de asistencia diaria<br />desayuno</h1>");
		break;
	case 1:
		documento_html << QString("<h1>Reporte de asistencia diaria<br />almuerzo</h1>");
		break;
	case 2:
		documento_html << QString("<h1>Reporte de asistencia por alumno<br />desayuno</h1>");
		break;
	case 3:
		documento_html << QString("<h1>Reporte de asistencia por alumno<br />almuerzo</h1>");
		break;
	case 4:
		documento_html << QString("<h1>Alumnos autorizados</h1>");
		break;
	}
	documento_html << QString("<p>Fecha inicio: %1<br />").arg(ui->repFechaInicio->date().toString("dd.MM.yyyy"));
	documento_html << QString("Fecha fin: %1<br />").arg(ui->repFechaFin->date().toString("dd.MM.yyyy"));
	documento_html<< "<table style=\"padding:5px;\">\n";

	// headers
	documento_html << "<tr>";
	for (int columna = 0; columna < cantidad_columnas; columna++)
		if (!tabla->isColumnHidden(columna))
			documento_html << QString("<th>%1</th>").arg(tabla->model()->headerData(columna, Qt::Horizontal).toString());
	documento_html << "</tr>\n";

	// data table
	for (int fila = 0; fila < cantidad_filas; fila++) {
		documento_html << "<tr>";
		for (int columna = 0; columna < cantidad_columnas; columna++) {
			if (!tabla->isColumnHidden(columna)) {
				QString data = tabla->model()->data(tabla->model()->index(fila, columna)).toString().simplified();
				documento_html << QString("<td>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
			}
		}
		documento_html << "</tr>\n";
	}
	QTextDocument *documento = new QTextDocument();
	documento->setHtml(documento_cadena);

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

void Casino::ImprimirReporte()
{
	int tipo = ui->repOpcion->currentIndex();
	QString strStream;
	QTextStream out(&strStream);
	out.setCodec("UTF-8");
	QTableWidget * tabla = ui->repTabla;

	const int rowCount = tabla->model()->rowCount();
	const int columnCount = tabla->model()->columnCount();

	out << "<html>\n";
	out   << "<head>\n";
	out   << "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />\n";
	out   << "<style type=\"text/css\">*{font-size: 9px;}table{margin: 5px;}td { padding: 3px; text-align: right;}th { padding: 5px; text-align: center;}</style>";
	out<<  "</head>\n";
	out<< "<body>\n";
	switch ( tipo )
	{
	case 0:
		out << QString("<h1>Reporte de asistencia diaria<br />desayuno</h1>");
		break;
	case 1:
		out << QString("<h1>Reporte de asistencia diaria<br />almuerzo</h1>");
		break;
	case 2:
		out << QString("<h1>Reporte de asistencia por alumno<br />desayuno</h1>");
		break;
	case 3:
		out << QString("<h1>Reporte de asistencia por alumno<br />almuerzo</h1>");
		break;
	case 4:
		out << QString("<h1>Alumnos autorizados</h1>");
		break;
	}
	out << QString("<p>Fecha inicio: %1<br />").arg(ui->repFechaInicio->date().toString("dd.MM.yyyy"));
	out << QString("<p>Fecha fin: %1<br />").arg(ui->repFechaFin->date().toString("dd.MM.yyyy"));
	out<< "<table style=\"padding:5px;\">\n";

	// headers
	out << "<tr>";
	for (int column = 0; column < columnCount; column++)
		if (!tabla->isColumnHidden(column))
			out << QString("<th>%1</th>").arg(tabla->model()->headerData(column, Qt::Horizontal).toString());
	out << "</tr>\n";

	// data table
	for (int row = 0; row < rowCount; row++) {
		out << "<tr>";
		for (int column = 0; column < columnCount; column++) {
			if (!tabla->isColumnHidden(column)) {
				QString data = tabla->model()->data(tabla->model()->index(row, column)).toString().simplified();
				out << QString("<td>%1</td>").arg((!data.isEmpty()) ? data : QString("&nbsp;"));
			}
		}
		out << "</tr>\n";
	}
	out << "</table>\n" << "</body>\n" << "</html>\n";

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
		csv.leer_archivo_db( archivo, this );
	}
}
