#include "../header/usuario.h"
#include "../header/utilidades.h"
#include <stdexcept>
using namespace std;

Usuario::Usuario(string nombreUsuario, string contrasena, string rol)
    : nombreUsuario(nombreUsuario), contrasena(contrasena), rol(rol) {
    if (!textoValido(nombreUsuario) || !textoValido(contrasena))
        throw invalid_argument("El usuario y la contrasena son obligatorios y no pueden tener comas.");
}

string Usuario::getDatoExtra() const { return ""; }
void Usuario::setDatoExtra(string dato) {}

bool Usuario::autenticar(string clave) const {
    return contrasena == clave;
}

void Usuario::cambiarContrasena(string nueva) {
    if (!textoValido(nueva))
        throw invalid_argument("La contrasena es obligatoria y no puede tener comas.");
    contrasena = nueva;
}

void Usuario::cambiarNombre(string nuevo) {
    if (!textoValido(nuevo))
        throw invalid_argument("El nombre de usuario es obligatorio y no puede tener comas.");
    nombreUsuario = nuevo;
}


// Getters
string Usuario::getNombreUsuario() const { return nombreUsuario; }
string Usuario::getContra() const { return contrasena; }
string Usuario::getRol() const { return rol; }
