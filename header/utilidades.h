#pragma once
#include <string>
#include <vector>
using namespace std;

// Validacion y texto
bool textoValido(const string& texto);
string aMinusculas(string texto);
string dinero(double valor);                              // 150000.0 -> "150000"
string generarCodigo(const string& prefijo, int numero);  // ("R", 7) -> "R007"

// Archivos
string rutaArchivo(const string& nombre);
bool existeArchivo(const string& ruta);
vector<string> leerLineas(const string& ruta);
void escribirLineas(const string& ruta, const vector<string>& lineas);
vector<string> dividirCampos(const string& linea);

// Entrada por consola (lanzan excepcion si el dato es incorrecto)
int leerOpcion();
string leerTexto(const string& mensaje);
string leerTextoObligatorio(const string& mensaje);
int leerEntero(const string& mensaje, int minimo, int maximo);
double leerDecimal(const string& mensaje, double minimoExclusivo);
bool confirmar(const string& mensaje);
