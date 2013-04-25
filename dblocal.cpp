#include "dblocal.h"

dbLocal * dbLocal::unicaLocal = NULL;

dbLocal::dbLocal()
{

}

bool dbLocal::conectar()
{
    return false;
}

bool dbLocal::conectar(QString archivo)
{
    if (baseDatos != NULL && baseDatos->isOpen())
        return true;
    baseDatos = new QSqlDatabase();
    if ( QSqlDatabase::connectionNames().contains( archivo ))
    {
        *baseDatos = QSqlDatabase::cloneDatabase( QSqlDatabase::database( archivo ), archivo+"1" );
        return true;
    }
    *baseDatos = QSqlDatabase::addDatabase("QSQLITE", archivo);
    baseDatos->setDatabaseName(archivo);
    return baseDatos->open();
}

bool dbLocal::conectar(QString servidor, QString puerto, QString base, QString usuario, QString contr)
{
    return false;
}

/*******************************************************************************
 * Funciones de importación TXT
 ******************************************************************************/

bool dbLocal::personas_agregar_muchas(QString datos)
{
    QSqlQuery consulta(*baseDatos);
    datos.truncate( datos.length() - 1 );
    consulta.prepare( "INSERT OR IGNORE INTO temporal "
                      "VALUES " + datos + ";");
    return consulta.exec();
}

void dbLocal::consolidar_temporal()
{
    QSqlQuery consulta(*baseDatos);
    consulta.exec("INSERT OR IGNORE INTO 'alumnos' "
                  "(rut,nombre,apellidoP,apellidoM,curso)"
                  "SELECT run_alumno,nombre_alumno,ape_paterno_alumno,ape_materno_alumno,"
                  "glosa_grado||' '||letra_curso "
                  "FROM temporal ORDER BY run_alumno");
    consulta.exec("DELETE FROM temporal");
}
