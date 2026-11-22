#include "../header/administrador.h"
using namespace std;

Administrador::Administrador(string nombreUser, string contra)
    : Usuario(nombreUser, contra, "Administrador") {}

// El administrador puede hacer todo.
bool Administrador::tienePermiso(string operacion) const {
    return true;
}

string Administrador::descripcionRol() const {
    return "Administrador";
}

string Administrador::serializar() const {
    return nombreUsuario + "," + contrasena + ",Administrador";
}
