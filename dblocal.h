#ifndef DBLOCAL_H
#define DBLOCAL_H

#include "db.h"

class dbLocal : public db
{
public:
    static dbLocal * getInstance()
    {
        if ( unicaLocal == NULL )
            dbLocal::unicaLocal = new dbLocal();
        return dbLocal::unicaLocal;
    }

    bool conectar();
    bool conectar(QString archivo);
    bool conectar(QString servidor, QString puerto, QString base, QString usuario, QString contr);
    bool personas_agregar_muchas(QString datos);
    void consolidar_temporal();
private:
    dbLocal();
    static dbLocal * unicaLocal;
};

#endif // DBLOCAL_H
