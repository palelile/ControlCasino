#ifndef DBCASINO_H
#define DBCASINO_H

#include "dblocal.h"

class dbCasino : public dbLocal
{
public:
	dbCasino();
	bool personas_agregar_muchas(QString datos);
	void consolidar_temporal();
};

#endif // DBCASINO_H
