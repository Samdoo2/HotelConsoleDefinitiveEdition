#pragma once
#include <string>
using namespace std;

class Fecha {
private:
    static bool esBisiesto(int anio);
    static int diasDelMes(int mes, int anio);
    static int contarDias(string fecha);

public:
    static string hoy();                       // fecha de hoy en formato AAAA-MM-DD
    static bool esValida(string fecha);
    static int diasEntre(string fechaEntrada, string fechaSalida);
    static bool seSuperponen(string entrada1, string salida1, string entrada2, string salida2);
};
