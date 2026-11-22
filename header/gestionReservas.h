#pragma once
#include <string>
#include <vector>
#include "habitacion.h"
#include "reserva.h"
using namespace std;

void inicializarArchivoReservas();
vector<Reserva> cargarReservas();
void guardarReservas(const vector<Reserva>& lista);
Reserva obtenerReserva(const string& codigo);   // lanza error si no existe

// Disponibilidad
bool habitacionLibre(const string& codigoHabitacion, const string& entrada,
                     const string& salida, const vector<Reserva>& reservas,
                     const string& ignorarCodigo);
vector<Habitacion> buscarHabitacionesLibres(const string& entrada, const string& salida,
                                            int personas, const string& tipo);
// "Disponible" = operativa y libre hoy. Si no, "Ocupada", "Mantenimiento" o "Fuera de servicio".
string estadoActualHabitacion(const Habitacion& h, const vector<Reserva>& reservas);

void mostrarResumenReserva(const Reserva& r);
void consultarDisponibilidadMenu();
void menuReservas();
