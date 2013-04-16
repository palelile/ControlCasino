#include "dbcasino.h"

dbCasino::dbCasino()
{
}

/*******************************************************************************
 * Funciones de importación TXT
 ******************************************************************************/

bool dbCasino::personas_agregar_muchas(QString datos)
{
	QSqlQuery consulta(*baseDatos);
	datos.truncate( datos.length() - 1 );
	consulta.prepare( "INSERT OR IGNORE INTO temporal "
					  "VALUES " + datos + ";");
	return consulta.exec();
}

void dbCasino::consolidar_temporal()
{
	QSqlQuery consulta(*baseDatos);
	consulta.exec("DELETE FROM alumnos");
	consulta.exec("INSERT OR IGNORE INTO 'alumnos' "
				  "(rut,nombre,apellidoP,apellidoM,curso)"
				  "SELECT run_alumno,nombre_alumno,ape_paterno_alumno,ape_materno_alumno,"
				  "glosa_grado||' '||letra_curso "
				  "FROM temporal ORDER BY run_alumno");
	consulta.exec("DELETE FROM temporal");
}
