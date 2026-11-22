#include "../header/reserva.h"
#include "../header/Fecha.h"
#include "../header/utilidades.h"
#include <stdexcept>
#include <vector>
using namespace std;

Reserva::Reserva() : personas(0), tarifaPorNoche(0), estado(EstadoReserva::Pendiente) {}

Reserva::Reserva(string codigo, string documentoHuesped, string codigoHabitacion,
                 string fechaEntrada, string fechaSalida, int personas,
                 double tarifaPorNoche, EstadoReserva estado)
    : codigo(codigo), documentoHuesped(documentoHuesped),
      codigoHabitacion(codigoHabitacion), fechaEntrada(fechaEntrada),
      fechaSalida(fechaSalida), personas(personas),
      tarifaPorNoche(tarifaPorNoche), estado(estado) {
    validar();
}

string Reserva::getCodigo() const { return codigo; }
string Reserva::getDocumentoHuesped() const { return documentoHuesped; }
string Reserva::getCodigoHabitacion() const { return codigoHabitacion; }
string Reserva::getFechaEntrada() const { return fechaEntrada; }
string Reserva::getFechaSalida() const { return fechaSalida; }
int Reserva::getPersonas() const { return personas; }
double Reserva::getTarifaPorNoche() const { return tarifaPorNoche; }
EstadoReserva Reserva::getEstado() const { return estado; }
bool Reserva::estaActiva() const { return estado != EstadoReserva::Cancelada; }

void Reserva::validar() const {
    if (!textoValido(codigo) || !textoValido(documentoHuesped) || !textoValido(codigoHabitacion))
        throw invalid_argument("Codigo, documento y habitacion son obligatorios y no pueden tener comas.");
    if (personas <= 0)
        throw invalid_argument("La reserva debe tener al menos una persona.");
    if (tarifaPorNoche <= 0)
        throw invalid_argument("La tarifa por noche debe ser mayor que cero.");
    Fecha::diasEntre(fechaEntrada, fechaSalida); // lanza error si las fechas no sirven
}

int Reserva::calcularNoches() const {
    return Fecha::diasEntre(fechaEntrada, fechaSalida);
}

double Reserva::calcularValorEstimado() const {
    return calcularNoches() * tarifaPorNoche;
}

// Dos reservas chocan si son de la misma habitacion, ambas estan activas y las fechas se cruzan.
bool Reserva::seSuperponeCon(const Reserva& otra) const {
    if (!estaActiva() || !otra.estaActiva()) return false;
    if (codigoHabitacion != otra.codigoHabitacion) return false;
    return Fecha::seSuperponen(fechaEntrada, fechaSalida, otra.fechaEntrada, otra.fechaSalida);
}

void Reserva::cambiarFechas(string nuevaEntrada, string nuevaSalida) {
    if (!estaActiva()) throw invalid_argument("No puedes editar una reserva cancelada.");
    Fecha::diasEntre(nuevaEntrada, nuevaSalida);
    fechaEntrada = nuevaEntrada;
    fechaSalida = nuevaSalida;
}

void Reserva::cambiarPersonas(int nuevasPersonas) {
    if (!estaActiva()) throw invalid_argument("No puedes editar una reserva cancelada.");
    if (nuevasPersonas <= 0) throw invalid_argument("Debe haber al menos una persona.");
    personas = nuevasPersonas;
}

void Reserva::cancelar() {
    if (!estaActiva()) throw invalid_argument("Esta reserva ya estaba cancelada.");
    estado = EstadoReserva::Cancelada;
}

string Reserva::nombreEstado(EstadoReserva estado) {
    if (estado == EstadoReserva::Pendiente) return "Pendiente";
    if (estado == EstadoReserva::Confirmada) return "Confirmada";
    return "Cancelada";
}

// Formato del CSV: codigo,documento,habitacion,entrada,salida,personas,tarifa,estado
string Reserva::serializar() const {
    validar();
    return codigo + "," + documentoHuesped + "," + codigoHabitacion + "," +
           fechaEntrada + "," + fechaSalida + "," + to_string(personas) + "," +
           to_string(tarifaPorNoche) + "," + nombreEstado(estado);
}

Reserva Reserva::deserializar(const string& linea) {
    vector<string> c = dividirCampos(linea);
    if (c.size() != 8)
        throw invalid_argument("La linea de reserva debe tener 8 campos.");
    int personas = 0;
    double tarifa = 0;
    try {
        personas = stoi(c[5]);
        tarifa = stod(c[6]);
    } catch (...) {
        throw invalid_argument("Personas o tarifa con formato incorrecto.");
    }
    return Reserva(c[0], c[1], c[2], c[3], c[4], personas, tarifa,
                   convertirEstadoReserva(c[7]));
}
