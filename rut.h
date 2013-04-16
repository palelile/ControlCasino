#ifndef RUT_H
#define RUT_H

#include <QString>

class rut
{
public:
	rut();
	static bool validar(QString rut, QString digito);
private:
	static QString invertir_rut( QString rut);
	static int digito_a_validador( QString rut );
	static int digito_a_numero( QString digito );
};

#endif // RUT_H
