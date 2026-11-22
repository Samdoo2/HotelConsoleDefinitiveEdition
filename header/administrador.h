#pragma once
#include "usuario.h"
using namespace std;

class Administrador : public Usuario {
public:
    Administrador(string nombreUser, string contra);

    bool tienePermiso(string operacion) const override;
    string descripcionRol() const override;
    string serializar() const override;
};
