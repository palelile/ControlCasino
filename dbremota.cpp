#include "dbremota.h"

dbRemota::dbRemota()
{
	baseDatos = NULL;
}

//dbRemota * dbRemota::getInstance()
//{
//    if ( !dbRemota::instanceFlagRemota )
//    {
//        dbRemota::unicaRemota = new dbRemota();
//        dbRemota::instanceFlagRemota = true;
//    }
//    return dbRemota::unicaRemota;
//}

bool dbRemota::conectar(QString servidor, QString puerto, QString base, QString usuario, QString contr)
{
		if (baseDatos != NULL)
			return false;
		baseDatos = new QSqlDatabase();
		*baseDatos = QSqlDatabase::addDatabase("QMYSQL");
//        baseDatos->setDatabaseName("Remota");
		baseDatos->setHostName(servidor);
		baseDatos->setDatabaseName(base);
		baseDatos->setPort( puerto.toInt() );
		baseDatos->setUserName( usuario );
		baseDatos->setPassword( contr );

		return baseDatos->open();
}

bool dbRemota::conectar()
{
    return false;
}

bool dbRemota::conectar(QString archivo)
{
    if (&archivo != NULL)
        return false;
    return false;
}
