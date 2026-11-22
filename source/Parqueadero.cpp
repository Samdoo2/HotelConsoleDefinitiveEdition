#include "../header/Parqueadero.h"
#include "../header/utilidades.h"
#include <stdexcept>
#include <sstream>

using namespace std;

Parqueadero::Parqueadero() : Servicio(), tipoVehiculo("") {}

Parqueadero::Parqueadero(string codigo, string nombre, string descripcion,
                         double precio, bool activo, string tipoVehiculo)
    : Servicio(codigo, nombre, descripcion, precio, activo), tipoVehiculo(tipoVehiculo) {
    if (tipoVehiculo.empty() || tipoVehiculo.find_first_of(",\r\n") != string::npos)
        throw invalid_argument("El tipo de vehiculo es obligatorio y no puede contener comas.");
}

string Parqueadero::getTipoVehiculo() const { return tipoVehiculo; }
string Parqueadero::getUnidadCobro() const { return "dia"; }
string Parqueadero::getTipoServicio() const { return "Parqueadero"; }

string Parqueadero::describir() const {
    return "[" + codigo + "] " + nombre + " (" + tipoVehiculo + ") - $" +
           dinero(precio) + " por " + getUnidadCobro();
}

// Costo = precio por dia por la cantidad de dias.
double Parqueadero::calcularCosto(int cantidad) const {
    if (cantidad <= 0) throw invalid_argument("La cantidad de dias debe ser mayor a cero.");
    if (!activo) throw invalid_argument("El servicio de parqueadero esta inactivo.");
    return precio * cantidad;
}

string Parqueadero::serializar() const {
    return Servicio::serializar() + "," + tipoVehiculo;
}

void Parqueadero::deserializar(const string& linea) {
    stringstream ss(linea);
    string tipo, cod, nom, d, precStr, actStr, vehiculo;
    if (!getline(ss, tipo, ',') || !getline(ss, cod, ',') ||
        !getline(ss, nom, ',') || !getline(ss, d, ',') ||
        !getline(ss, precStr, ',') || !getline(ss, actStr, ',') ||
        !getline(ss, vehiculo)) {
        throw invalid_argument("Formato CSV invalido para Parqueadero.");
    }
    if (!vehiculo.empty() && vehiculo.back() == '\r') vehiculo.pop_back();
    if (tipo != "Parqueadero")
        throw invalid_argument("La linea no corresponde a un servicio de Parqueadero.");

    Parqueadero temp(cod, nom, d, stod(precStr), actStr == "1", vehiculo);
    *this = temp;
}
