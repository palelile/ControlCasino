#include "dbremota.h"

dbRemota::dbRemota()
{
	baseDatos = NULL;
}

bool dbRemota::conectar(QString servidor, QString puerto, QString base, QString usuario, QString contr)
{
		if (baseDatos != NULL)
			return false;
		baseDatos = new QSqlDatabase();
		*baseDatos = QSqlDatabase::addDatabase("QMYSQL");
		baseDatos->setHostName(servidor);
		baseDatos->setDatabaseName(base);
		baseDatos->setPort( puerto.toInt() );
		baseDatos->setUserName( usuario );
		baseDatos->setPassword( contr );

		return baseDatos->open();
}
