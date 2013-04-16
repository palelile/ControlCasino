#include "rut.h"
#include <algorithm>

rut::rut()
{

}

bool rut::validar(QString rut, QString digito)
{
	rut = invertir_rut( rut );
	return digito_a_validador(rut) == digito_a_numero(digito );
}

QString rut::invertir_rut(QString rut)
{
	QByteArray ba = rut.toUtf8();
	char *d = ba.data();
	std::reverse(d, d+rut.length());
	return QString(d);
}

int rut::digito_a_validador(QString rut)
{
	int total = 0;
	for ( int i = 0; i < rut.length(); i++ )
		total += rut.mid(i, 1).toInt() * (i % 6 + 2);
	return 11 - (total % 11);
}

int rut::digito_a_numero( QString digito )
{
	if ( digito.compare("K") == 0 || digito.compare("k") == 0 )
		return 10;
	if ( digito.toInt() == 0 )
		return 11;
	return digito.toInt();
}
