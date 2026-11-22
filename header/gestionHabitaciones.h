#pragma once
#include <string>
#include <vector>
#include "habitacion.h"
#include "usuario.h"
using namespace std;

void inicializarArchivoHabitaciones();
vector<Habitacion> cargarHabitaciones();
void guardarHabitaciones(const vector<Habitacion>& lista);
// El texto "estado" es lo que se muestra al final (Disponible, Ocupada, Mantenimiento...).
void mostrarHabitacion(const Habitacion& h, const string& estado);
void menuHabitaciones(const Usuario& usuario);
