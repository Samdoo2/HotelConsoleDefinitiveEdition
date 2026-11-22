#pragma once
#include <string>
#include "usuario.h"
using namespace std;

// Crea usuarios.csv con el administrador inicial si no existe o esta vacio.
void inicializarArchivoUsuarios();

// Devuelve un Usuario nuevo (el que llama debe hacer delete) o lanza excepcion.
Usuario* iniciarSesion(const string& nombre, const string& clave);

// Menu de gestion de usuarios (solo administrador).
void menuUsuarios(const Usuario& actual);
