#pragma once
#include "usuario.h"
using namespace std;

class Recepcionista : public Usuario {
private:
    string turno;

public:
    Recepcionista(string nombreUser, string contra, string turno);

    bool tienePermiso(string operacion) const override;
    string descripcionRol() const override;
    string serializar() const override;
    string getDatoExtra() const override;
    void setDatoExtra(string dato) override;

    string getTurno() const;
};
