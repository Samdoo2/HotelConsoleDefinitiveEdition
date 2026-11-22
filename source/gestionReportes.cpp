#include "../header/gestionReportes.h"
#include "../header/gestionHabitaciones.h"
#include "../header/gestionReservas.h"
#include "../header/gestionConsumos.h"
#include "../header/gestionFacturas.h"
#include "../header/gestionServicios.h"
#include "../header/utilidades.h"
#include "../header/Fecha.h"
#include <iostream>
#include <stdexcept>
using namespace std;

// Cada reporte arma una lista de lineas de texto y las guarda con escribirLineas.

static void reporteReservas() {
    vector<Reserva> reservas = cargarReservas();
    vector<string> lineas;
    lineas.push_back("REPORTE DE RESERVAS");
    lineas.push_back("Fecha del reporte: " + Fecha::hoy());
    lineas.push_back("Total de reservas: " + to_string(reservas.size()));
    lineas.push_back("");
    for (size_t i = 0; i < reservas.size(); i++) {
        const Reserva& r = reservas[i];
        lineas.push_back(r.getCodigo() + " | Doc " + r.getDocumentoHuesped() +
                         " | Hab " + r.getCodigoHabitacion() +
                         " | " + r.getFechaEntrada() + " a " + r.getFechaSalida() +
                         " | " + to_string(r.getPersonas()) + " persona(s)" +
                         " | " + Reserva::nombreEstado(r.getEstado()) +
                         " | $" + dinero(r.calcularValorEstimado()));
    }
    escribirLineas(rutaArchivo("reporte_reservas.txt"), lineas);
}

static void reporteOcupacion() {
    vector<Habitacion> habitaciones = cargarHabitaciones();
    vector<Reserva> reservas = cargarReservas();
    vector<string> lineas;
    lineas.push_back("REPORTE DE OCUPACION");
    lineas.push_back("Fecha del reporte: " + Fecha::hoy());
    lineas.push_back("");
    int ocupadasHoy = 0;
    for (size_t i = 0; i < habitaciones.size(); i++) {
        int reservasActivas = 0;
        int nochesReservadas = 0;
        for (size_t j = 0; j < reservas.size(); j++) {
            if (reservas[j].getCodigoHabitacion() == habitaciones[i].getCodigo() &&
                reservas[j].estaActiva()) {
                reservasActivas++;
                nochesReservadas = nochesReservadas + reservas[j].calcularNoches();
            }
        }
        string estado = estadoActualHabitacion(habitaciones[i], reservas);
        if (estado == "Ocupada") ocupadasHoy++;
        lineas.push_back("Hab " + habitaciones[i].getCodigo() + " | " +
                         Habitacion::nombreTipo(habitaciones[i].getTipo()) +
                         " | Hoy: " + estado +
                         " | Reservas activas: " + to_string(reservasActivas) +
                         " | Noches reservadas: " + to_string(nochesReservadas));
    }
    lineas.push_back("");
    lineas.push_back("Habitaciones ocupadas hoy: " + to_string(ocupadasHoy) +
                     " de " + to_string(habitaciones.size()));
    escribirLineas(rutaArchivo("reporte_ocupacion.txt"), lineas);
}

static void reporteIngresos() {
    vector<Factura> facturas = cargarFacturas();
    vector<string> lineas;
    lineas.push_back("REPORTE DE INGRESOS");
    lineas.push_back("Fecha del reporte: " + Fecha::hoy());
    lineas.push_back("");
    double pagado = 0, pendiente = 0, impuestos = 0;
    for (size_t i = 0; i < facturas.size(); i++) {
        if (facturas[i].getEstadoPago() == "Pagada") {
            pagado = pagado + facturas[i].getTotalPagar();
            impuestos = impuestos + facturas[i].getImpuestos();
        } else {
            pendiente = pendiente + facturas[i].getTotalPagar();
        }
        lineas.push_back(facturas[i].getCodigo() + " | " + facturas[i].getFechaEmision() +
                         " | $" + dinero(facturas[i].getTotalPagar()) +
                         " | " + facturas[i].getEstadoPago());
    }
    lineas.push_back("");
    lineas.push_back("Total cobrado (facturas pagadas): $" + dinero(pagado));
    lineas.push_back("  de los cuales impuestos: $" + dinero(impuestos));
    lineas.push_back("Total por cobrar (facturas pendientes): $" + dinero(pendiente));
    escribirLineas(rutaArchivo("reporte_ingresos.txt"), lineas);
}

static void reporteServicios() {
    vector<Servicio*> servicios = cargarServicios();
    vector<ConsumoServicio> consumos = cargarConsumos();
    vector<string> lineas;
    lineas.push_back("REPORTE DE SERVICIOS CONSUMIDOS");
    lineas.push_back("Fecha del reporte: " + Fecha::hoy());
    lineas.push_back("");
    double granTotal = 0;
    for (size_t i = 0; i < servicios.size(); i++) {
        int unidades = 0;
        double total = 0;
        for (size_t j = 0; j < consumos.size(); j++) {
            if (consumos[j].getCodigoServicio() == servicios[i]->getCodigo()) {
                unidades = unidades + consumos[j].getCantidad();
                total = total + consumos[j].getValorTotal();
            }
        }
        granTotal = granTotal + total;
        lineas.push_back(servicios[i]->getCodigo() + " | " + servicios[i]->getNombre() +
                         " | Unidades: " + to_string(unidades) + " | Total $" + dinero(total));
    }
    lineas.push_back("");
    lineas.push_back("Total de servicios consumidos: $" + dinero(granTotal));
    liberarServicios(servicios);
    escribirLineas(rutaArchivo("reporte_servicios.txt"), lineas);
}

static void reporteFacturas() {
    vector<Factura> facturas = cargarFacturas();
    vector<string> lineas;
    lineas.push_back("REPORTE DE FACTURAS");
    lineas.push_back("Fecha del reporte: " + Fecha::hoy());
    lineas.push_back("Total de facturas: " + to_string(facturas.size()));
    lineas.push_back("");
    for (size_t i = 0; i < facturas.size(); i++) {
        const Factura& f = facturas[i];
        lineas.push_back(f.getCodigo() + " | Reserva " + f.getCodigoReserva() +
                         " | " + f.getFechaEmision() +
                         " | Noches $" + dinero(f.getValorNoches()) +
                         " | Servicios $" + dinero(f.getSubtotalServicios()) +
                         " | Impuestos $" + dinero(f.getImpuestos()) +
                         " | TOTAL $" + dinero(f.getTotalPagar()) +
                         " | " + f.getEstadoPago());
    }
    escribirLineas(rutaArchivo("reporte_facturas.txt"), lineas);
}

void menuReportes(const Usuario& usuario) {
    if (!usuario.tienePermiso("reportes")) {
        cout << "Acceso denegado para este rol.\n";
        return;
    }
    int opcion;
    do {
        cout << "\n=== REPORTES (se guardan en la carpeta Persistencia) ===\n"
             << "1. Reporte de reservas\n"
             << "2. Reporte de ocupacion\n"
             << "3. Reporte de ingresos\n"
             << "4. Reporte de servicios\n"
             << "5. Reporte de facturas\n"
             << "6. Generar todos los reportes\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: reporteReservas(); cout << "Archivo reporte_reservas.txt generado.\n"; break;
                case 2: reporteOcupacion(); cout << "Archivo reporte_ocupacion.txt generado.\n"; break;
                case 3: reporteIngresos(); cout << "Archivo reporte_ingresos.txt generado.\n"; break;
                case 4: reporteServicios(); cout << "Archivo reporte_servicios.txt generado.\n"; break;
                case 5: reporteFacturas(); cout << "Archivo reporte_facturas.txt generado.\n"; break;
                case 6:
                    reporteReservas();
                    reporteOcupacion();
                    reporteIngresos();
                    reporteServicios();
                    reporteFacturas();
                    cout << "Los 5 reportes fueron generados.\n";
                    break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}
