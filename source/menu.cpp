#include "../header/menu.h"
#include "../header/gestionUsuarios.h"
#include "../header/gestionHuespedes.h"
#include "../header/gestionHabitaciones.h"
#include "../header/gestionReservas.h"
#include "../header/gestionServicios.h"
#include "../header/gestionConsumos.h"
#include "../header/gestionFacturas.h"
#include "../header/gestionReportes.h"
#include "../header/utilidades.h"
#include <iostream>
using namespace std;

// Menu del administrador: ve todos los modulos.
static void menuAdministrador(const Usuario& usuario) {
    int opcion;
    do {
        cout << "\n=== MENU PRINCIPAL (ADMINISTRADOR) ===\n"
             << "1. Gestion de usuarios\n"
             << "2. Habitaciones\n"
             << "3. Huespedes\n"
             << "4. Reservas\n"
             << "5. Servicios\n"
             << "6. Consumos de servicios\n"
             << "7. Facturacion\n"
             << "8. Reportes\n"
             << "0. Cerrar sesion\n";
        opcion = leerOpcion();
        switch (opcion) {
            case 1: menuUsuarios(usuario); break;
            case 2: menuHabitaciones(usuario); break;
            case 3: menuHuespedes(); break;
            case 4: menuReservas(); break;
            case 5: menuServicios(usuario); break;
            case 6: menuConsumos(); break;
            case 7: menuFacturacion(); break;
            case 8: menuReportes(usuario); break;
            case 0: cout << "Cerrando sesion...\n"; break;
            default: cout << "Opcion no valida.\n";
        }
    } while (opcion != 0);
}

// Menu del recepcionista: solo ve lo que puede usar, numerado desde 1.
static void menuRecepcionista(const Usuario& usuario) {
    int opcion;
    do {
        cout << "\n=== MENU PRINCIPAL (RECEPCIONISTA) ===\n"
             << "1. Consulta de habitaciones\n"
             << "2. Huespedes\n"
             << "3. Reservas\n"
             << "4. Consumos de servicios\n"
             << "5. Facturacion\n"
             << "0. Cerrar sesion\n";
        opcion = leerOpcion();
        switch (opcion) {
            case 1: menuHabitaciones(usuario); break;
            case 2: menuHuespedes(); break;
            case 3: menuReservas(); break;
            case 4: menuConsumos(); break;
            case 5: menuFacturacion(); break;
            case 0: cout << "Cerrando sesion...\n"; break;
            default: cout << "Opcion no valida.\n";
        }
    } while (opcion != 0);
}

void menuPrincipal(const Usuario& usuario) {
    cout << "\nBienvenido, " << usuario.descripcionRol() << "\n";
    if (usuario.getRol() == "Administrador") menuAdministrador(usuario);
    else menuRecepcionista(usuario);
}
