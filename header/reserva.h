#pragma once
#include <string>
#include "Enumeraciones.h"
using namespace std;

// Una reserva relaciona UN huesped con UNA habitacion en un rango de fechas.
class Reserva {
private:
    string codigo;
    string documentoHuesped;
    string codigoHabitacion;
    string fechaEntrada;
    string fechaSalida;
    int personas;
    double tarifaPorNoche;
    EstadoReserva estado;

public:
    Reserva();
    Reserva(string codigo, string documentoHuesped, string codigoHabitacion,
            string fechaEntrada, string fechaSalida, int personas,
            double tarifaPorNoche, EstadoReserva estado = EstadoReserva::Confirmada);

    string getCodigo() const;
    string getDocumentoHuesped() const;
    string getCodigoHabitacion() const;
    string getFechaEntrada() const;
    string getFechaSalida() const;
    int getPersonas() const;
    double getTarifaPorNoche() const;
    EstadoReserva getEstado() const;
    bool estaActiva() const;              // activa = no cancelada

    int calcularNoches() const;
    double calcularValorEstimado() const; // noches * tarifa
    bool seSuperponeCon(const Reserva& otra) const;
    void cambiarFechas(string nuevaEntrada, string nuevaSalida);
    void cambiarPersonas(int nuevasPersonas);
    void cancelar();
    void validar() const;

    string serializar() const;
    static Reserva deserializar(const string& linea);
    static string nombreEstado(EstadoReserva estado);
};
