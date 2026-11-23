#include "../header/gestionReservas.h"
#include "../header/gestionHabitaciones.h"
#include "../header/gestionHuespedes.h"
#include "../header/gestionConsumos.h"
#include "../header/gestionFacturas.h"
#include "../header/utilidades.h"
#include "../header/Fecha.h"
#include <iostream>
#include <stdexcept>
using namespace std;

static string rutaReservas() { return rutaArchivo("reservas.csv"); }

vector<Reserva> cargarReservas() {
    vector<string> lineas = leerLineas(rutaReservas());
    vector<Reserva> lista;
    for (size_t i = 0; i < lineas.size(); i++) {
        try {
            Reserva r = Reserva::deserializar(lineas[i]);
            for (size_t j = 0; j < lista.size(); j++)
                if (lista[j].getCodigo() == r.getCodigo())
                    throw invalid_argument("Codigo de reserva repetido.");
            lista.push_back(r);
        } catch (const exception& e) {
            throw runtime_error("reservas.csv, linea " + to_string(i + 1) + ": " + e.what());
        }
    }
    return lista;
}

void guardarReservas(const vector<Reserva>& lista) {
    vector<string> lineas;
    for (size_t i = 0; i < lista.size(); i++)
        lineas.push_back(lista[i].serializar());
    escribirLineas(rutaReservas(), lineas);
}

void inicializarArchivoReservas() {
    if (!existeArchivo(rutaReservas())) {
        vector<string> vacio;
        escribirLineas(rutaReservas(), vacio);
    }
    cargarReservas(); // valida el formato del archivo
}

Reserva obtenerReserva(const string& codigo) {
    vector<Reserva> lista = cargarReservas();
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getCodigo() == codigo) return lista[i];
    throw invalid_argument("No existe la reserva " + codigo + ".");
}

// Una habitacion esta libre si ninguna reserva activa suya se cruza con las fechas pedidas.
// ignorarCodigo sirve al modificar una reserva (para no chocar consigo misma).
bool habitacionLibre(const string& codigoHabitacion, const string& entrada,
                     const string& salida, const vector<Reserva>& reservas,
                     const string& ignorarCodigo) {
    for (size_t i = 0; i < reservas.size(); i++) {
        const Reserva& r = reservas[i];
        if (!r.estaActiva()) continue;
        if (r.getCodigoHabitacion() != codigoHabitacion) continue;
        if (r.getCodigo() == ignorarCodigo) continue;
        if (Fecha::seSuperponen(entrada, salida, r.getFechaEntrada(), r.getFechaSalida()))
            return false;
    }
    return true;
}

// tipo = "" significa cualquier tipo de habitacion.
vector<Habitacion> buscarHabitacionesLibres(const string& entrada, const string& salida,
                                            int personas, const string& tipo) {
    Fecha::diasEntre(entrada, salida); // valida las fechas
    if (personas <= 0) throw invalid_argument("El numero de personas debe ser positivo.");
    vector<Habitacion> habitaciones = cargarHabitaciones();
    vector<Reserva> reservas = cargarReservas();
    vector<Habitacion> libres;
    for (size_t i = 0; i < habitaciones.size(); i++) {
        const Habitacion& h = habitaciones[i];
        if (!h.estaOperativa()) continue;                 // mantenimiento o fuera de servicio
        if (h.getCapacidad() < personas) continue;        // no cabe la gente
        if (tipo != "" && Habitacion::nombreTipo(h.getTipo()) != tipo) continue;
        if (habitacionLibre(h.getCodigo(), entrada, salida, reservas, ""))
            libres.push_back(h);
    }
    return libres;
}



string estadoActualHabitacion(const Habitacion& h, const vector<Reserva>& reservas) {
    if (!h.estaOperativa()) return Habitacion::nombreEstado(h.getEstado());
    string hoy = Fecha::hoy();
    for (size_t i = 0; i < reservas.size(); i++) {
        const Reserva& r = reservas[i];
        if (r.estaActiva() && r.getCodigoHabitacion() == h.getCodigo() &&
            r.getFechaEntrada() <= hoy && hoy < r.getFechaSalida())
            return "Ocupada";
    }
    return "Disponible";
}

// Fechas de una reserva nueva: validas y no anteriores a hoy.
static void validarFechasNuevas(const string& entrada, const string& salida) {
    Fecha::diasEntre(entrada, salida);
    if (entrada < Fecha::hoy())
        throw invalid_argument("La fecha de entrada no puede ser anterior a hoy.");
}

static string leerTipoFiltro() {
    int t = leerEntero("Tipo (0=Cualquiera, 1=Sencilla, 2=Doble, 3=Familiar, 4=Suite): ", 0, 4);
    if (t == 1) return "Sencilla";
    if (t == 2) return "Doble";
    if (t == 3) return "Familiar";
    if (t == 4) return "Suite";
    return "";
}

static int buscarReserva(const vector<Reserva>& lista, const string& codigo) {
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getCodigo() == codigo) return (int)i;
    return -1;
}

static int buscarHabitacionPorCodigo(const vector<Habitacion>& lista, const string& codigo) {
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getCodigo() == codigo) return (int)i;
    return -1;
}

void mostrarResumenReserva(const Reserva& r) {
    cout << r.getCodigo() << " | Doc " << r.getDocumentoHuesped()
         << " | Hab " << r.getCodigoHabitacion()
         << " | " << r.getFechaEntrada() << " a " << r.getFechaSalida()
         << " | " << r.getPersonas() << " persona(s)"
         << " | " << Reserva::nombreEstado(r.getEstado())
         << " | $" << dinero(r.calcularValorEstimado()) << "\n";
}

static string nombreDelHuesped(const string& documento) {
    vector<Huesped> lista = cargarHuespedes();
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getDocumento() == documento) return lista[i].getNombre();
    return "(no registrado)";
}

static void mostrarDetalleReserva(const Reserva& r) {
    cout << "\n===== RESERVA " << r.getCodigo() << " =====\n"
         << "Huesped: " << r.getDocumentoHuesped() << " - " << nombreDelHuesped(r.getDocumentoHuesped()) << "\n"
         << "Habitacion: " << r.getCodigoHabitacion() << "\n"
         << "Fechas: " << r.getFechaEntrada() << " a " << r.getFechaSalida()
         << " (" << r.calcularNoches() << " noches)\n"
         << "Personas: " << r.getPersonas() << "\n"
         << "Tarifa por noche: $" << dinero(r.getTarifaPorNoche()) << "\n"
         << "Valor estimado de la estadia: $" << dinero(r.calcularValorEstimado()) << "\n"
         << "Estado: " << Reserva::nombreEstado(r.getEstado()) << "\n"
         << "Servicios consumidos:\n";
    mostrarConsumosDeReserva(r.getCodigo());
    cout << "Subtotal servicios: $" << dinero(subtotalServicios(r.getCodigo())) << "\n";
    if (reservaTieneFactura(r.getCodigo())) cout << "Esta reserva ya tiene factura.\n";
}

void consultarDisponibilidadMenu() {
    cout << "\n=== CONSULTAR DISPONIBILIDAD ===\n";
    string entrada = leerTextoObligatorio("Fecha de entrada (AAAA-MM-DD): ");
    string salida = leerTextoObligatorio("Fecha de salida (AAAA-MM-DD): ");
    validarFechasNuevas(entrada, salida);
    int personas = leerEntero("Numero de personas: ", 1, 20);
    string tipo = leerTipoFiltro();
    vector<Habitacion> libres = buscarHabitacionesLibres(entrada, salida, personas, tipo);
    cout << "\nHay " << libres.size() << " habitacion(es) libre(s) para "
         << Fecha::diasEntre(entrada, salida) << " noche(s):\n";
    for (size_t i = 0; i < libres.size(); i++)
        mostrarHabitacion(libres[i], "Libre en esas fechas");
}

static void registrarReservaMenu() {
    cout << "\n=== REGISTRAR RESERVA ===\n";
    string documento = leerTextoObligatorio("Documento del huesped: ");
    if (!existeHuesped(documento)) {
        cout << "El huesped no esta registrado. Vamos a registrarlo.\n";
        string nombre = leerTextoObligatorio("Nombre completo: ");
        string telefono = leerTextoObligatorio("Telefono: ");
        string correo = leerTextoObligatorio("Correo: ");
        registrarHuesped(Huesped(documento, nombre, telefono, correo));
        cout << "Huesped registrado.\n";
    }
    string entrada = leerTextoObligatorio("Fecha de entrada (AAAA-MM-DD): ");
    string salida = leerTextoObligatorio("Fecha de salida (AAAA-MM-DD): ");
    validarFechasNuevas(entrada, salida);
    int personas = leerEntero("Numero de personas: ", 1, 20);
    string tipo = leerTipoFiltro();

    vector<Habitacion> libres = buscarHabitacionesLibres(entrada, salida, personas, tipo);
    if (libres.empty())
        throw invalid_argument("No hay habitaciones libres con esas condiciones.");
    cout << "\nHabitaciones libres:\n";
    for (size_t i = 0; i < libres.size(); i++)
        mostrarHabitacion(libres[i], "Libre en esas fechas");

    string codigoHab = leerTextoObligatorio("Codigo de la habitacion elegida: ");
    int pos = buscarHabitacionPorCodigo(libres, codigoHab);
    if (pos == -1)
        throw invalid_argument("Esa habitacion no esta en la lista de habitaciones libres.");

    vector<Reserva> reservas = cargarReservas();
    Reserva nueva(generarCodigo("R", (int)reservas.size() + 1), documento, codigoHab,
                  entrada, salida, personas, libres[pos].getPrecioPorNoche());
    cout << "\nResumen de la reserva:\n";
    mostrarResumenReserva(nueva);
    if (!confirmar("Guardar la reserva")) {
        cout << "Reserva descartada.\n";
        return;
    }
    reservas.push_back(nueva);
    guardarReservas(reservas);
    cout << "Reserva guardada con codigo " << nueva.getCodigo() << ".\n";
}

static void listarReservas() {
    vector<Reserva> lista = cargarReservas();
    cout << "\n=== RESERVAS (" << lista.size() << ") ===\n";
    for (size_t i = 0; i < lista.size(); i++) mostrarResumenReserva(lista[i]);
}

static void buscarPorCodigoReserva() {
    string codigo = leerTextoObligatorio("Codigo de la reserva: ");
    mostrarDetalleReserva(obtenerReserva(codigo));
}

static void buscarPorDocumento() {
    string documento = leerTextoObligatorio("Documento del huesped: ");
    vector<Reserva> lista = cargarReservas();
    int encontradas = 0;
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].getDocumentoHuesped() == documento) {
            mostrarResumenReserva(lista[i]);
            encontradas++;
        }
    }
    if (encontradas == 0) cout << "Ese documento no tiene reservas.\n";
}

static void buscarPorFecha() {
    string fecha = leerTextoObligatorio("Fecha (AAAA-MM-DD): ");
    if (!Fecha::esValida(fecha)) throw invalid_argument("La fecha no es valida.");
    vector<Reserva> lista = cargarReservas();
    int encontradas = 0;
    for (size_t i = 0; i < lista.size(); i++) {
        if (lista[i].estaActiva() && lista[i].getFechaEntrada() <= fecha &&
            fecha < lista[i].getFechaSalida()) {
            mostrarResumenReserva(lista[i]);
            encontradas++;
        }
    }
    if (encontradas == 0) cout << "No hay reservas activas en esa fecha.\n";
}

static void cambiarFechasMenu() {
    string codigo = leerTextoObligatorio("Codigo de la reserva a modificar: ");
    vector<Reserva> reservas = cargarReservas();
    int pos = buscarReserva(reservas, codigo);
    if (pos == -1) throw invalid_argument("No existe esa reserva.");
    if (reservaTieneFactura(codigo))
        throw invalid_argument("La reserva ya tiene factura y no se puede modificar.");

    string entrada = leerTextoObligatorio("NUEVA fecha de entrada (AAAA-MM-DD): ");
    string salida = leerTextoObligatorio("NUEVA fecha de salida (AAAA-MM-DD): ");
    validarFechasNuevas(entrada, salida);

    string codigoHab = reservas[pos].getCodigoHabitacion();
    vector<Habitacion> habitaciones = cargarHabitaciones();
    int posHab = buscarHabitacionPorCodigo(habitaciones, codigoHab);
    if (posHab == -1 || !habitaciones[posHab].estaOperativa())
        throw invalid_argument("La habitacion esta en mantenimiento o fuera de servicio.");
    if (!habitacionLibre(codigoHab, entrada, salida, reservas, codigo))
        throw invalid_argument("La habitacion ya esta reservada en esas fechas.");

    reservas[pos].cambiarFechas(entrada, salida);
    guardarReservas(reservas);
    cout << "Fechas actualizadas.\n";
}

static void cambiarPersonasMenu() {
    string codigo = leerTextoObligatorio("Codigo de la reserva a modificar: ");
    vector<Reserva> reservas = cargarReservas();
    int pos = buscarReserva(reservas, codigo);
    if (pos == -1) throw invalid_argument("No existe esa reserva.");
    if (reservaTieneFactura(codigo))
        throw invalid_argument("La reserva ya tiene factura y no se puede modificar.");

    int personas = leerEntero("Nuevo numero de personas: ", 1, 20);
    vector<Habitacion> habitaciones = cargarHabitaciones();
    int posHab = buscarHabitacionPorCodigo(habitaciones, reservas[pos].getCodigoHabitacion());
    if (posHab == -1) throw invalid_argument("La habitacion de la reserva ya no existe.");
    if (personas > habitaciones[posHab].getCapacidad())
        throw invalid_argument("La habitacion solo tiene capacidad para " +
                               to_string(habitaciones[posHab].getCapacidad()) + " persona(s).");

    reservas[pos].cambiarPersonas(personas);
    guardarReservas(reservas);
    cout << "Numero de personas actualizado.\n";
}

static void cancelarReservaMenu() {
    string codigo = leerTextoObligatorio("Codigo de la reserva a cancelar: ");
    vector<Reserva> reservas = cargarReservas();
    int pos = buscarReserva(reservas, codigo);
    if (pos == -1) throw invalid_argument("No existe esa reserva.");
    if (reservaTieneFactura(codigo))
        throw invalid_argument("La reserva ya tiene factura y no se puede cancelar.");
    if (!confirmar("Cancelar la reserva " + codigo)) {
        cout << "Operacion cancelada.\n";
        return;
    }
    reservas[pos].cancelar();
    guardarReservas(reservas);
    cout << "Reserva cancelada. Se conserva en el historial y la habitacion queda libre.\n";
}

void menuReservas() {
    int opcion;
    do {
        cout << "\n=== RESERVAS ===\n"
             << "1. Registrar reserva\n"
             << "2. Ver todas las reservas\n"
             << "3. Ver detalle de una reserva (por codigo)\n"
             << "4. Buscar reservas por documento del huesped\n"
             << "5. Buscar reservas por fecha\n"
             << "6. Cambiar fechas de una reserva\n"
             << "7. Cambiar numero de personas\n"
             << "8. Cancelar reserva\n"
             << "9. Consultar disponibilidad por fechas\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: registrarReservaMenu(); break;
                case 2: listarReservas(); break;
                case 3: buscarPorCodigoReserva(); break;
                case 4: buscarPorDocumento(); break;
                case 5: buscarPorFecha(); break;
                case 6: cambiarFechasMenu(); break;
                case 7: cambiarPersonasMenu(); break;
                case 8: cancelarReservaMenu(); break;
                case 9: consultarDisponibilidadMenu(); break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}
