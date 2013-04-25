#ifndef DBREMOTA_H
#define DBREMOTA_H

#include "db.h"
#include <QSqlDriver>

class dbRemota : public db
{
public:
    dbRemota();
    static dbRemota * getInstance();
	bool conectar(QString servidor, QString puerto, QString base, QString usuario, QString contr);
    bool conectar();
    bool conectar(QString archivo);
};

#endif // DBREMOTA_H
