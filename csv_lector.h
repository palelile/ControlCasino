#ifndef CSV_LECTOR_H
#define CSV_LECTOR_H

#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QIODevice>
#include <QString>
#include <QStringList>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <dblocal.h>

class csv_lector
{
public:
    csv_lector();
    bool leer_archivo_db(QString nombre_archivo, dbLocal *base_datos);

private:
    QFile archivo_csv;
    dbLocal * mi_db;
    QString leer_linea_temporal_db(QString linea);
    QString leer_linea_curso_db(QString linea);
};

#endif // CSV_LECTOR_H
