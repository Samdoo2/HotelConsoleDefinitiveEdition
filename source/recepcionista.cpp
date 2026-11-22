#include "../header/recepcionista.h"
#include "../header/utilidades.h"
#include <stdexcept>
using namespace std;

Recepcionista::Recepcionista(string nombreUser, string contra, string turno)
    : Usuario(nombreUser, contra, "Recepcionista"), turno(turno) {
    if (!textoValido(turno))
        throw invalid_argument("El turno es obligatorio y no puede tener comas.");
}

// Atiende huespedes, reservas, consumos y facturas.
// No administra usuarios, inventario de habitaciones, servicios ni reportes.
bool Recepcionista::tienePermiso(string operacion) const {
    return operacion == "habitaciones_consultar" || operacion == "huespedes" ||
           operacion == "reservas" || operacion == "consumos" ||
           operacion == "facturacion";
}

string Recepcionista::descripcionRol() const {
    return "Recepcionista (turno " + turno + ")";
}

string Recepcionista::serializar() const {
    return nombreUsuario + "," + contrasena + ",Recepcionista," + turno;
}

string Recepcionista::getDatoExtra() const { return turno; }

void Recepcionista::setDatoExtra(string dato) {
    if (!textoValido(dato))
        throw invalid_argument("El turno es obligatorio y no puede tener comas.");
    turno = dato;
}

string Recepcionista::getTurno() const { return turno; }
