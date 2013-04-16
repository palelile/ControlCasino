#ifndef DBLOCAL_H
#define DBLOCAL_H

#include "db.h"

class dbLocal : public db
{
public:
    dbLocal();
    bool conectar(QString archivo);
};

#endif // DBLOCAL_H
