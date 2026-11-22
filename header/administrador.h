#pragma once
#include "usuario.h"
using namespace std;

/***
* @file administrador.h
* @brief Clase Administrador que hereda de Usuario
* 
*
*
*/

class Administrador : public Usuario {
public:
    Administrador(string nombreUser, string contra);

	bool tienePermiso(string operacion) const override;  ///< Verifica si el administrador tiene permiso
	string descripcionRol() const override; ///< Retorna la descripción del rol del administrador
	string serializar() const override; ///< simplemente muestra el nombre de usuario, la contraseña y el rol "Administrador"
};
