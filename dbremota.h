#ifndef DBREMOTA_H
#define DBREMOTA_H

#include "db.h"
#include <QSqlDriver>

class dbRemota : public db
{
public:
	dbRemota();
	bool conectar(QString servidor, QString puerto, QString base, QString usuario, QString contr);
};

#endif // DBREMOTA_H
