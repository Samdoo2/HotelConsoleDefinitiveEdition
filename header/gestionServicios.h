#pragma once
#include <string>
#include <vector>
#include "Servicio.h"
#include "usuario.h"
using namespace std;

void inicializarArchivoServicios();
vector<Servicio*> cargarServicios();           // el que llama debe liberar con liberarServicios
void liberarServicios(vector<Servicio*>& lista);
void mostrarServiciosActivos();
// Calcula el costo de un servicio activo (usa el calcularCosto de cada clase hija).
double calcularCostoServicio(const string& codigoServicio, int cantidad);
void menuServicios(const Usuario& usuario);
