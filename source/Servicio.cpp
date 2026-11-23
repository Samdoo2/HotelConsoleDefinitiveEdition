#include "../header/Servicio.h"
#include <stdexcept>

using namespace std;

Servicio::Servicio() : codigo(""), nombre(""), desc(""), precio(0.0), activo(false) {}

Servicio::Servicio(string codigo, string nombre, string desc, double precio, bool activo)
    : codigo(codigo), nombre(nombre), desc(desc), precio(precio), activo(activo) {
    validar();
}

void Servicio::activar() { activo = true; }
void Servicio::desactivar() { activo = false; }

// Cambia nombre, descripcion y precio. Si algo es invalido, deja los datos como estaban.
void Servicio::actualizarDatos(string nuevoNombre, string nuevaDesc, double nuevoPrecio) {
    string nombreViejo = nombre;
    string descViejo = desc;
    double precioViejo = precio;
    nombre = nuevoNombre;
    desc = nuevaDesc;
    precio = nuevoPrecio;
    try {
        validar();
    } catch (...) {
        nombre = nombreViejo;
        desc = descViejo;
        precio = precioViejo;
        throw;
    }
}

void Servicio::validar() const {
    if (codigo.empty() || nombre.empty())
        throw invalid_argument("El codigo y el nombre del servicio son obligatorios.");
    if (precio <= 0)
        throw invalid_argument("El precio del servicio debe ser mayor a cero.");
    if (codigo.find_first_of(",\r\n") != string::npos ||
        nombre.find_first_of(",\r\n") != string::npos ||
        desc.find_first_of(",\r\n") != string::npos) {
        throw invalid_argument("Los datos del servicio no pueden contener comas ni saltos de linea.");
    }
}

string Servicio::serializar() const {
    validar();
    return getTipoServicio() + "," + codigo + "," + nombre + "," + desc + "," +
           to_string(precio) + "," + (activo ? "1" : "0"); // El ? funciona como un if comprimido
}

string Servicio::getCodigo() const { return codigo; }
string Servicio::getNombre() const { return nombre; }
string Servicio::getDesc() const { return desc; }
double Servicio::getPrecio() const { return precio; }
bool Servicio::isActivo() const { return activo; }
