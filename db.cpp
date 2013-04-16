#include "db.h"

db::db()
{
    baseDatos = NULL;
}

bool db::conectar()
{
    return false;
}

bool db::desconectar()
{
	baseDatos->close();
	return !baseDatos->isOpen();
}

bool db::isOpen()
{
	return baseDatos->isOpen();
}

bool db::agregar(QString tabla, QString campos, QString valores)
{
    QSqlQuery consulta(*baseDatos);
    consulta.prepare("INSERT INTO " + tabla +
                     " (" + campos +
                     ") VALUES (" + valores + ")");
    return consulta.exec();
}


bool db::modificar(QString tabl, QString cond, QString cam, QString val)
{
    QSqlQuery consulta(*baseDatos);
    consulta.prepare("UPDATE " + tabl +
                     " SET " + construirSet(cam, val) +
                     " WHERE " + cond);
    return consulta.exec();
}

bool db::borrar(QString tabl, QString cond)
{
    QSqlQuery consulta(*baseDatos);
    consulta.prepare("DELETE FROM " + tabl +
                     " WHERE " + cond);
    return consulta.exec();
}

QStringList * db::buscar(QString ret, QString tabl, QString cond)
{
    QStringList * salida = new QStringList();
    salida->append( ret.split(",") );
    QSqlQuery consulta(*baseDatos);
    consulta.prepare("SELECT " + ret +
                     " FROM " + tabl +
                     " WHERE " + cond);
    if ( !consulta.exec() || !consulta.next() )
        return NULL;
    for ( int i = 0; i < salida->count(); i++ )
        (*salida)[i] = consulta.record().value(i).toString();
    return salida;
}

QStringList * db::buscarColumna(QString col, QString tabl, QString cond)
{
    QStringList * salida = new QStringList();
    QSqlQuery consulta(*baseDatos);
    consulta.prepare("SELECT " + col +
                     " FROM " + tabl +
                     " WHERE " + cond);
    if ( !consulta.exec() || !consulta.next() )
        return NULL;
    do
        salida->append( consulta.record().value(0).toString() );
    while ( consulta.next() );
    return salida;
}

QString db::construirSet(QString campos, QString valores)
{
    QStringList lcampos = campos.split(",");
    QStringList lvalores = valores.split(",");
    if ( lcampos.count() != lvalores.count() )
        return NULL;
    QString salida = "";
    for (int i=0; i<lcampos.count(); i++ )
        salida.append(lcampos.at(i) + '=' + lvalores.at(i) + ',');
    salida.truncate( salida.length() -1 );
    return salida;
}


QSqlRelationalTableModel *db::modelo(QObject *parent)
{
    return new QSqlRelationalTableModel(parent, *baseDatos);
}

QSqlQueryModel * db::modeloConsulta(QObject *parent, QString consulta)
{
    QSqlQueryModel * modelo = new QSqlQueryModel(parent);
    modelo->setQuery(consulta, *baseDatos);
    return modelo;
}

