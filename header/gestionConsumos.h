#pragma once
#include <string>
#include <vector>
#include "ConsumoServicio.h"
using namespace std;

void inicializarArchivoConsumos();
vector<ConsumoServicio> cargarConsumos();
double subtotalServicios(const string& codigoReserva);      // suma de los consumos de una reserva
void mostrarConsumosDeReserva(const string& codigoReserva);
void menuConsumos();
