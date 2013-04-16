#include "dblocal.h"

dbLocal::dbLocal()
{
}

bool dbLocal::conectar(QString archivo)
{
    baseDatos = new QSqlDatabase();
    *baseDatos = QSqlDatabase::addDatabase("QSQLITE", archivo);
    baseDatos->setDatabaseName(archivo);
    return baseDatos->open();
}
