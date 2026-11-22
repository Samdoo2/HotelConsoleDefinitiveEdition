#pragma once
#include <string>
#include <vector>
#include "huesped.h"
using namespace std;

void inicializarArchivoHuespedes();
vector<Huesped> cargarHuespedes();
void guardarHuespedes(const vector<Huesped>& lista);
bool existeHuesped(const string& documento);
void registrarHuesped(const Huesped& nuevo);  // rechaza documentos duplicados
void menuHuespedes();
