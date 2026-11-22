#pragma once
#include <string>
using namespace std;

// Clase base abstracta. Cada rol decide sus permisos y como se guarda.
class Usuario {
protected:
    string nombreUsuario;
    string contrasena;
    string rol;

public:
    Usuario(string nombreUsuario, string contrasena, string rol);
    virtual ~Usuario() {}

    // Metodos polimorficos (cada hijo los sobrescribe)
    virtual bool tienePermiso(string operacion) const = 0;
    virtual string descripcionRol() const = 0;
    virtual string serializar() const = 0;
    virtual string getDatoExtra() const;      // turno en el recepcionista
    virtual void setDatoExtra(string dato);   // turno en el recepcionista

    bool autenticar(string clave) const;
    void cambiarContrasena(string nueva);
    void cambiarNombre(string nuevo);

    string getNombreUsuario() const;
    string getContra() const;
    string getRol() const;
};
