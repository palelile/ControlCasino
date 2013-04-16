#ifndef DB_H
#define DB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlRecord>
#include <QStringList>
#include <QString>
#include <QDate>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlRelationalTableModel>
#include <QSqlQueryModel>

class db
{
public:
	db();

    // Funciones a ser portadas a distintos motores de bases de datos
    virtual bool conectar();
	bool desconectar();
	bool isOpen();
    int estado();

    // Función generica de  manejo de datos
    bool agregar(QString tabla, QString campos, QString valores);
    bool modificar(QString tabl, QString cond, QString cam, QString val);
    bool borrar(QString tabl, QString cond);
    QStringList *buscar(QString ret, QString tabl, QString cond);
    QStringList *buscarColumna(QString col, QString tabl, QString cond);

    // Función de asistencia
    QString construirSet(QString campos, QString valores);

    // Modelo asociado a base de datos
    QSqlRelationalTableModel * modelo(QObject *parent);
    QSqlQueryModel * modeloConsulta(QObject *parent, QString consulta);

protected:
    QSqlDatabase * baseDatos;
};

#endif // DB_H
