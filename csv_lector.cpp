#include "csv_lector.h"

csv_lector::csv_lector()
{
    mi_db = NULL;
}

bool csv_lector::leer_archivo_db(QString nombre_archivo, dbLocal *base_datos)
{
	mi_db = base_datos;
	QFile file(nombre_archivo);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	QTextStream linea(&file);

	linea.setCodec(QTextCodec::codecForName("ISO-8859-15"));
	linea.readLine(); // Asumo que tiene cabecera
	QString datos ="";
	int i = 1;
	while ( !linea.atEnd() )
	{
		datos.append( leer_linea_temporal_db( linea.readLine() ) );
		i++;
		if ( i%300 == 0 || linea.atEnd() )
		{
			mi_db->personas_agregar_muchas( datos );
			datos.clear();
		}
	}
	mi_db->consolidar_temporal();
	file.close();
	return true;
}

QString csv_lector::leer_linea_temporal_db(QString linea)
{
	QStringList campos = linea.split(";");
	if ( campos.isEmpty() || campos.count() != 25)
		return "";
	QString salida = "('";
	for ( int i = 0; i < 25; i++ )
		salida.append(campos.value(i) + "','");
	salida.truncate( salida.length() - 3);
	salida.append("'),");
	return salida;
}

QString csv_lector::leer_linea_curso_db(QString linea)
{
	QStringList campos = linea.split(";");
	if ( campos.isEmpty() || campos.count() != 25)
		return "";
	QString salida = "";
    salida.append("('" + campos.value(0) +
                  "','" + campos.value(2) +
                  "','" + campos.value(3) +
                  "','" + campos.value(5) +
                  "','" + campos.value(4) +
                  "'),");
	return salida;
}
