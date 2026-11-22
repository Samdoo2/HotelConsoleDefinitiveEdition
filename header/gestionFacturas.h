#pragma once
#include <string>
#include <vector>
#include "Factura.h"
using namespace std;

void inicializarArchivoFacturas();
vector<Factura> cargarFacturas();
bool reservaTieneFactura(const string& codigoReserva);
void menuFacturacion();
