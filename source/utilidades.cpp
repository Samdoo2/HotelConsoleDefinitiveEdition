#include "../header/utilidades.h"

#include <cctype> // Esta libreria sirve para usar la funcion tolower() que convierte un caracter a minuscula
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
using namespace std;

bool textoValido(const string& texto) {
	if (texto.find_first_not_of(" \t") == string::npos) return false; ///< Verifica si el texto es solo espacios o tabulaciones
    if (texto.find_first_of(",\r\n") != string::npos) return false; ///< Verifica si el texto contiene comas, saltos de linea o el \r (retorno de carro)
    return true; 
}

string aMinusculas(string texto) {
    for (size_t i = 0; i < texto.size(); i++)
        texto[i] = (char)tolower((unsigned char)texto[i]); ///< Convierte cada caracter a minuscula usando la funcion tolower()
    return texto;
}

// Muestra el valor sin decimales.
string dinero(double valor) {
	long long entero = (long long)(valor + 0.5); ///< Redondea el valor a su entero más cercano
    return to_string(entero);
}

// Crea codigos como R001, F002, C003...
string generarCodigo(const string& prefijo, int numero) {
    string texto = to_string(numero);
	while (texto.size() < 3) texto = "0" + texto;  ///< Agrega ceros a la izquierda hasta que el tamaño del texto sea 3
    return prefijo + texto;
}

// Los archivos siempre se buscan en la carpeta Persistencia del proyecto.
string rutaArchivo(const string& nombre) {
#ifdef HOTEL_PROJECT_ROOT
	return string(HOTEL_PROJECT_ROOT) + "/Persistencia/" + nombre;  ///< Devuelve la ruta completa del archivo en la carpeta Persistencia del proyecto
#else
    return "Persistencia/" + nombre; ///< Si se compila sin cmake entonces ruta relativa
#endif
}

// Esta funcion simplemente verifica si el archivo existe y se puede abrir.
bool existeArchivo(const string& ruta) {
    ifstream archivo(ruta);
    return archivo.is_open();
}

vector<string> leerLineas(const string& ruta) {
    ifstream archivo(ruta);
    if (!archivo.is_open())
        throw runtime_error("No se pudo abrir el archivo: " + ruta);
    vector<string> lineas;
    string linea;
    while (getline(archivo, linea)) {
        if (!linea.empty() && linea[linea.size() - 1] == '\r') 
            linea.erase(linea.size() - 1);            ///< Esto es para borrar el \r
		if (!linea.empty()) lineas.push_back(linea); ///< Si la linea no esta vacia la agrega al vector de lineas y ya
    }
    return lineas;
}


// Esta funcion sobreescribe el archivo con las lineas que se le pasan. Si el archivo no existe entonces lo crea
void escribirLineas(const string& ruta, const vector<string>& lineas) {
    ofstream archivo(ruta, ios::trunc);
    if (!archivo.is_open())
        throw runtime_error("No se pudo escribir el archivo: " + ruta);
    for (size_t i = 0; i < lineas.size(); i++)
        archivo << lineas[i] << "\n";
    archivo.flush();
    if (archivo.fail())
        throw runtime_error("Error al guardar el archivo: " + ruta);
}


// Esta funcion divide una linea de texto en campos separados por comas y devuelve un vector con los campos
vector<string> dividirCampos(const string& linea) {
    vector<string> campos;
    string campo;
    istringstream ss(linea);
    while (getline(ss, campo, ',')) campos.push_back(campo);
    if (!linea.empty() && linea[linea.size() - 1] == ',') campos.push_back("");
    return campos;
}


// Esta funcion valida que unicamente entre un entero
int leerOpcion() {
    while (true) {
        cout << "Seleccione una opcion: ";
        string texto;
        if (!getline(cin, texto)) return 0;
        istringstream ss(texto);
        int valor;
        char sobra;
		if ((ss >> valor) && !(ss >> sobra)) return valor; ///< Si se pudo leer un entero y no hay mas caracteres entonces retorna el valor
        cout << "Escribe un numero valido.\n";
    }
}

// Flujo de entrada de datos por la consola
string leerTexto(const string& mensaje) {
    cout << mensaje;
    string valor;
    if (!getline(cin, valor))
        throw runtime_error("Se interrumpio la entrada de datos.");
    return valor;
}

string leerTextoObligatorio(const string& mensaje) {
    string valor = leerTexto(mensaje);
    if (!textoValido(valor))
        throw invalid_argument("El dato es obligatorio y no puede contener comas.");
    return valor;
}



// Esta funcion valida que el entero no sea menor que el minimo y no mayor que el maximo (algo parecido a leerOpcion)
int leerEntero(const string& mensaje, int minimo, int maximo) {
    string texto = leerTexto(mensaje);
    istringstream ss(texto);
    int valor;
    char sobra;
    if (!(ss >> valor) || (ss >> sobra) || valor < minimo || valor > maximo) ///< Siempre se esta preguntando si ese coso es invalido
        throw invalid_argument("Escribe un numero entero entre " + to_string(minimo) +
                               " y " + to_string(maximo) + ".");
    return valor;
}

double leerDecimal(const string& mensaje, double minimoExclusivo) {
    string texto = leerTexto(mensaje);
    istringstream ss(texto);
    double valor;
    char sobra;
    if (!(ss >> valor) || (ss >> sobra) || valor <= minimoExclusivo)
        throw invalid_argument("Escribe un numero mayor que " + to_string((long long)minimoExclusivo) + ".");
    return valor;
}


// Esta funcion confirma una accion con el usuario y ya
bool confirmar(const string& mensaje) {
    string texto = leerTexto(mensaje + " (S/N): ");
    return texto == "S" || texto == "s";
}
