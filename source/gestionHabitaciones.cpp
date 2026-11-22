#include "../header/gestionHabitaciones.h"
#include "../header/gestionReservas.h"
#include "../header/utilidades.h"
#include <iostream>
#include <stdexcept>
using namespace std;

static string rutaHabitaciones() { return rutaArchivo("habitaciones.csv"); }

vector<Habitacion> cargarHabitaciones() {
    vector<string> lineas = leerLineas(rutaHabitaciones());
    vector<Habitacion> lista;
    for (size_t i = 0; i < lineas.size(); i++) {
        try {
            Habitacion h = Habitacion::deserializar(lineas[i]);
            for (size_t j = 0; j < lista.size(); j++)
                if (lista[j].getCodigo() == h.getCodigo())
                    throw invalid_argument("Codigo de habitacion repetido.");
            lista.push_back(h);
        } catch (const exception& e) {
            throw runtime_error("habitaciones.csv, linea " + to_string(i + 1) + ": " + e.what());
        }
    }
    return lista;
}

void guardarHabitaciones(const vector<Habitacion>& lista) {
    vector<string> lineas;
    for (size_t i = 0; i < lista.size(); i++)
        lineas.push_back(lista[i].serializar());
    escribirLineas(rutaHabitaciones(), lineas);
}

void inicializarArchivoHabitaciones() {
    bool vacio = true;
    if (existeArchivo(rutaHabitaciones())) vacio = leerLineas(rutaHabitaciones()).empty();
    if (vacio) {
        vector<Habitacion> lista;
        lista.push_back(Habitacion("101", TipoHabitacion::Sencilla, 1, 100000));
        lista.push_back(Habitacion("102", TipoHabitacion::Sencilla, 1, 100000));
        lista.push_back(Habitacion("103", TipoHabitacion::Sencilla, 1, 105000));
        lista.push_back(Habitacion("104", TipoHabitacion::Sencilla, 1, 105000));
        lista.push_back(Habitacion("105", TipoHabitacion::Doble, 2, 155000));
        lista.push_back(Habitacion("106", TipoHabitacion::Doble, 2, 155000));
        lista.push_back(Habitacion("107", TipoHabitacion::Doble, 2, 165000));
        lista.push_back(Habitacion("108", TipoHabitacion::Doble, 2, 165000));
        lista.push_back(Habitacion("201", TipoHabitacion::Familiar, 4, 250000));
        lista.push_back(Habitacion("202", TipoHabitacion::Familiar, 4, 250000,
                                   EstadoHabitacion::FueraDeServicio));
        lista.push_back(Habitacion("203", TipoHabitacion::Suite, 2, 350000));
        lista.push_back(Habitacion("204", TipoHabitacion::Suite, 2, 350000,
                                   EstadoHabitacion::Mantenimiento));
        guardarHabitaciones(lista);
    } else {
        cargarHabitaciones(); // valida el formato del archivo
    }
}

void mostrarHabitacion(const Habitacion& h, const string& estado) {
    cout << "Habitacion " << h.getCodigo() << " | " << Habitacion::nombreTipo(h.getTipo())
         << " | Capacidad " << h.getCapacidad() << " | $"
         << dinero(h.getPrecioPorNoche()) << "/noche | " << estado << "\n";
}

static int buscarIndice(const vector<Habitacion>& lista, const string& codigo) {
    for (size_t i = 0; i < lista.size(); i++)
        if (lista[i].getCodigo() == codigo) return (int)i;
    return -1;
}

// Cuenta las reservas de una habitacion. Si soloActivas es true, ignora las canceladas.
static int contarReservasDeHabitacion(const string& codigo, bool soloActivas) {
    vector<Reserva> reservas = cargarReservas();
    int cantidad = 0;
    for (size_t i = 0; i < reservas.size(); i++) {
        if (reservas[i].getCodigoHabitacion() != codigo) continue;
        if (soloActivas && !reservas[i].estaActiva()) continue;
        cantidad++;
    }
    return cantidad;
}

static TipoHabitacion elegirTipo() {
    int t = leerEntero("Tipo (1=Sencilla, 2=Doble, 3=Familiar, 4=Suite): ", 1, 4);
    if (t == 1) return TipoHabitacion::Sencilla;
    if (t == 2) return TipoHabitacion::Doble;
    if (t == 3) return TipoHabitacion::Familiar;
    return TipoHabitacion::Suite;
}

static void listarHabitaciones() {
    vector<Habitacion> lista = cargarHabitaciones();
    vector<Reserva> reservas = cargarReservas();
    cout << "\n=== HABITACIONES (" << lista.size() << ") ===\n";
    for (size_t i = 0; i < lista.size(); i++)
        mostrarHabitacion(lista[i], estadoActualHabitacion(lista[i], reservas));
    cout << "Disponible = libre hoy para ocupar. Ocupada = tiene una reserva que incluye hoy.\n";
}

static void buscarHabitacionMenu() {
    string codigo = leerTextoObligatorio("Codigo de la habitacion: ");
    vector<Habitacion> lista = cargarHabitaciones();
    vector<Reserva> reservas = cargarReservas();
    int pos = buscarIndice(lista, codigo);
    if (pos == -1) cout << "No existe esa habitacion.\n";
    else mostrarHabitacion(lista[pos], estadoActualHabitacion(lista[pos], reservas));
}

static void agregarHabitacionMenu() {
    string codigo = leerTextoObligatorio("Codigo de la habitacion: ");
    vector<Habitacion> lista = cargarHabitaciones();
    if (buscarIndice(lista, codigo) != -1)
        throw invalid_argument("Ese codigo de habitacion ya existe.");
    TipoHabitacion tipo = elegirTipo();
    int capacidad = leerEntero("Capacidad (1-20): ", 1, 20);
    double precio = leerDecimal("Precio por noche: ", 0);
    lista.push_back(Habitacion(codigo, tipo, capacidad, precio));
    guardarHabitaciones(lista);
    cout << "Habitacion registrada.\n";
}

static void actualizarHabitacionMenu() {
    string codigo = leerTextoObligatorio("Codigo de la habitacion: ");
    vector<Habitacion> lista = cargarHabitaciones();
    int pos = buscarIndice(lista, codigo);
    if (pos == -1) throw invalid_argument("No existe esa habitacion.");
    TipoHabitacion tipo = elegirTipo();
    int capacidad = leerEntero("Nueva capacidad (1-20): ", 1, 20);
    double precio = leerDecimal("Nuevo precio por noche: ", 0);

    // Si baja la capacidad, no puede dejar a una reserva activa con mas personas de las que caben.
    vector<Reserva> reservas = cargarReservas();
    for (size_t i = 0; i < reservas.size(); i++) {
        if (reservas[i].getCodigoHabitacion() == codigo && reservas[i].estaActiva() &&
            reservas[i].getPersonas() > capacidad)
            throw invalid_argument("La reserva " + reservas[i].getCodigo() +
                                   " tiene mas personas que la nueva capacidad.");
    }
    lista[pos].actualizarDatos(tipo, capacidad, precio);
    guardarHabitaciones(lista);
    cout << "Habitacion actualizada.\n";
}

static void cambiarEstadoMenu() {
    string codigo = leerTextoObligatorio("Codigo de la habitacion: ");
    vector<Habitacion> lista = cargarHabitaciones();
    int pos = buscarIndice(lista, codigo);
    if (pos == -1) throw invalid_argument("No existe esa habitacion.");
    int e = leerEntero("Estado (1=Disponible, 2=Mantenimiento, 3=Fuera de servicio): ", 1, 3);
    if (e != 1 && contarReservasDeHabitacion(codigo, true) > 0)
        throw invalid_argument("La habitacion tiene reservas activas. Cancelalas antes de bloquearla.");
    if (e == 1) lista[pos].cambiarEstado(EstadoHabitacion::Disponible);
    else if (e == 2) lista[pos].cambiarEstado(EstadoHabitacion::Mantenimiento);
    else lista[pos].cambiarEstado(EstadoHabitacion::FueraDeServicio);
    guardarHabitaciones(lista);
    cout << "Estado actualizado.\n";
}

static void eliminarHabitacionMenu() {
    string codigo = leerTextoObligatorio("Codigo de la habitacion a eliminar: ");
    vector<Habitacion> lista = cargarHabitaciones();
    int pos = buscarIndice(lista, codigo);
    if (pos == -1) throw invalid_argument("No existe esa habitacion.");
    if (contarReservasDeHabitacion(codigo, false) > 0)
        throw invalid_argument("Tiene historial de reservas. Mejor ponla fuera de servicio.");
    if (confirmar("Eliminar la habitacion " + codigo)) {
        lista.erase(lista.begin() + pos);
        guardarHabitaciones(lista);
        cout << "Habitacion eliminada.\n";
    }
}

// Menu del administrador: ve todas las opciones.
static void menuHabitacionesAdmin() {
    int opcion;
    do {
        cout << "\n=== GESTION DE HABITACIONES ===\n"
             << "1. Ver todas las habitaciones\n"
             << "2. Buscar por codigo\n"
             << "3. Agregar habitacion\n"
             << "4. Actualizar tipo, capacidad y precio\n"
             << "5. Cambiar estado (mantenimiento, fuera de servicio)\n"
             << "6. Eliminar habitacion\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: listarHabitaciones(); break;
                case 2: buscarHabitacionMenu(); break;
                case 3: agregarHabitacionMenu(); break;
                case 4: actualizarHabitacionMenu(); break;
                case 5: cambiarEstadoMenu(); break;
                case 6: eliminarHabitacionMenu(); break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}

// Menu del recepcionista: solo consulta. No ve las opciones del administrador.
static void menuHabitacionesRecepcion() {
    int opcion;
    do {
        cout << "\n=== CONSULTA DE HABITACIONES ===\n"
             << "1. Ver todas las habitaciones\n"
             << "2. Buscar por codigo\n"
             << "3. Consultar disponibilidad por fechas\n"
             << "0. Volver\n";
        opcion = leerOpcion();
        try {
            switch (opcion) {
                case 1: listarHabitaciones(); break;
                case 2: buscarHabitacionMenu(); break;
                case 3: consultarDisponibilidadMenu(); break;
                case 0: break;
                default: cout << "Opcion no valida.\n";
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    } while (opcion != 0);
}

void menuHabitaciones(const Usuario& usuario) {
    if (usuario.tienePermiso("habitaciones_admin")) menuHabitacionesAdmin();
    else if (usuario.tienePermiso("habitaciones_consultar")) menuHabitacionesRecepcion();
    else cout << "Acceso denegado para este rol.\n";
}
