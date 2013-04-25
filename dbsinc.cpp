#include "dbsinc.h"

dbSinc::dbSinc()
{
}

bool dbSinc::conectarRemota(QString servidor, QString puerto, QString base, QString usuario, QString contr)
{
    db_remota = new dbRemota();
    return db_remota->conectar(servidor,puerto, base, usuario, contr);
}

void dbSinc::desconectar()
{
	db_remota->desconectar();
}

QStringList * dbSinc::TablaTemporal(db *base_datos, QString tabla, QString campos, int ultimo_id)
{
    if ( !base_datos->isOpen() )
        return NULL;
    QSqlQueryModel * modelo = base_datos->modeloConsulta(NULL, "SELECT "+campos+
                                                         " FROM "+tabla+
                                                         " WHERE id>'"+QString::number(ultimo_id)+
                                                         "'");

    QStringList * Listas = new QStringList();
    while (modelo->canFetchMore()) modelo->fetchMore();

    QString valores = "";

    for (int i = 0; i < modelo->rowCount(); i++)
    {	valores = "('";
        for ( int j = 0; j < modelo->columnCount(); j++)
            if ( modelo->record(i).value(j).isNull() )
                valores.append( "','" );
            else
                valores.append( modelo->record(i).value(j).toString() + "','" );
        valores.truncate(valores.length()-2);
        valores.append(")");
        Listas->append(valores);
    }
    return Listas;
}

QStringList * dbSinc::TablaTemporalCadenas(db *base_datos, QString tabla, int ultimo_id)
{
	if ( !base_datos->isOpen() )
		return NULL;
	QStringList * Listas = new QStringList();

	QSqlQueryModel * modelo = base_datos->modeloConsulta(NULL,
														 "SELECT id FROM " +tabla+
                                                         " WHERE id>'"+ultimo_id+"';"
                                                         );
    while (modelo->canFetchMore()) modelo->fetchMore();

    for ( int i = 0; i < modelo->rowCount(); i++)
        Listas->append( modelo->record(i).value(0).toString() );

    return Listas;
}

bool dbSinc::PersistirTablaTemporal(db *base_datos, QString tabla, QString campos, QStringList *listas)
{
	if ( !base_datos->isOpen() )
		return NULL;
    QString valores = "";
	for ( int i = 0; i < listas->count(); i++)
	{
		valores.append(listas->at(i)+",");
        if ( (i % 200 == 0) || (i == listas->count()-1) )
		{
            valores.truncate(valores.length()-1);
            if ( !base_datos->agregarM(tabla, campos, valores) )
				return false;
            valores = "";
		}
	}
	return true;
}

int dbSinc::ultimoRegistro(db *base_datos, QString tabla)
{
	QStringList * salida = NULL;
	QString cond = "tabla='"+tabla+"'";
	if ( base_datos == db_local )
        salida = db_local->buscar("ultimoregistrolocal", "indices", cond);
	else
        salida = db_local->buscar("ultimoregistroremoto", "indices", cond);
	if (salida)
        return salida->at(0).toInt();
	return -1;
}

int dbSinc::ultimoRegistroDB(db *base_datos, QString tabla)
{
    QStringList *salida = base_datos->buscar("MAX(id)", tabla, "1=1");
	if (salida)
		return salida->at(0).toInt();
	return -1;
}

bool dbSinc::SubirTabla(QString tabla_local, QString tabla_remota, QString campos) {
    db_local = dbLocal::getInstance();
    if ( !db_local->isOpen() || !db_remota->isOpen() )
		return false;
	// Recupero indice de los últimos registros sincronizados localmente y remotamente
	int ultimo_registro_local = ultimoRegistro(db_local, tabla_local);
	int ultimo_registro_remoto = ultimoRegistro(db_remota, tabla_local);
	// Tablas temporales con los registros que no han sido sincronizados
	QStringList * tablaTemporalLocal = TablaTemporal(db_local, tabla_local, campos, ultimo_registro_local);
	QStringList * tablaTemporalRemoto = TablaTemporal(db_remota, tabla_remota, campos, ultimo_registro_remoto);
	// Se hace permanente los registros en las bases de datos respectivas
//	borrarRegistroEliminadoDBRemota(db_local, db_remota, tabla_local, tabla_remota);
//	borrarRegistroEliminadoDBRemota(db_remota, db_local, tabla_remota, tabla_local);
	PersistirTablaTemporal(db_remota, tabla_remota, campos, tablaTemporalLocal);
	PersistirTablaTemporal(db_local, tabla_local, campos, tablaTemporalRemoto);
	ultimo_registro_local = ultimoRegistroDB( db_local, tabla_local );
	ultimo_registro_remoto = ultimoRegistroDB( db_remota, tabla_remota);
	return guardarUltimoRegistro(tabla_local, ultimo_registro_local, ultimo_registro_remoto);
}

bool dbSinc::borrarRegistroEliminadoDBRemota(db *db_1, db *db_2, QString tabla1, QString tabla2)
{
	if ( !db_1->isOpen() || !db_2->isOpen() )
		return false;
	int ultimo_registro_1 = ultimoRegistro(db_1, tabla1);
    int ultimo_registro_2 = ultimoRegistro(db_2, tabla1);
	QStringList * tablaTemporal1 = TablaTemporalCadenas(db_1, tabla1, ultimo_registro_1);
	QStringList * tablaTemporal2 = TablaTemporalCadenas(db_2, tabla2, ultimo_registro_2);

	for (int i=0; i < tablaTemporal1->count(); i++ )
		if ( !tablaTemporal2->contains( tablaTemporal1->value( i ) ))
			db_1->borrar(tabla1, "id='"+tablaTemporal1->value( i )+"'");
	for (int i=0; i < tablaTemporal2->count(); i++ )
		if ( !tablaTemporal1->contains( tablaTemporal2->value( i ) ))
			db_2->borrar(tabla2, "id='"+tablaTemporal2->value( i )+"'");
	return true;
}


bool dbSinc::guardarUltimoRegistro(QString tabla, int ultimo_registro_local, int ultimo_registro_remoto)
{
	db_local->borrar("indices", "tabla='"+tabla+"'");

    QString valores = "'"+tabla+"','"+QString::number(ultimo_registro_local)+"','"+QString::number(ultimo_registro_remoto)+"'";
	return db_local->agregar("indices", "tabla,ultimoregistrolocal,ultimoregistroremoto", valores);
}
