#ifndef DBSINC_H
#define DBSINC_H

#include "dblocal.h"
#include "dbremota.h"

class dbSinc
{
public:
	dbSinc();
	bool conectarRemota(QString servidor, QString puerto, QString base, QString usuario, QString contr);
	void desconectar();

	bool SubirTabla(QString tabla_local, QString tabla_remota, QString campos);
	bool borrarRegistroEliminadoDBRemota(db *db_1, db *db_2, QString tabla1, QString tabla2);
    bool borrarRegistro(db *db_remota, QString tabla, QString registro);
	QStringList *TablaTemporal(db *base_datos, QString tabla, QString campos, int ultimo_id);
	QStringList *TablaTemporalCadenas(db *base_datos, QString tabla, int ultimo_id);
	bool PersistirTablaTemporal(db *base_datos, QString tabla, QString campos, QStringList * listas);
	bool guardarUltimoRegistro(QString tabla, int ultimo_registro_local, int ultimo_registro_remoto);

	int ultimoRegistro(db *base_datos, QString tabla);
	int ultimoRegistroDB(db *base_datos, QString tabla);

private:
    dbLocal * db_local;
    dbRemota * db_remota;
};

#endif // DBSINC_H
