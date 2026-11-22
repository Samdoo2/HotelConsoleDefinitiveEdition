#include <iostream>
#include <string>
#include <exception>
#include <conio.h> // Para ocultar la contrasena (Windows)


// Funciones de gestion de usuarios
#include "header/usuario.h"
#include "header/menu.h"
#include "header/gestionUsuarios.h"
#include "header/gestionHuespedes.h"
#include "header/gestionHabitaciones.h"
#include "header/gestionReservas.h"
#include "header/gestionServicios.h"
#include "header/gestionConsumos.h"
#include "header/gestionFacturas.h"

using namespace std;

// Lee la contrasena mostrando asteriscos.
string leerContrasena() {
    string clave;
    int tecla;
    while ((tecla = _getch()) != 13) {      // 13 = Enter
        if (tecla == 8) {                   // 8 = Retroceso
            if (!clave.empty()) {
                clave.erase(clave.size() - 1);
                cout << "\b \b";
            }
        } else if (tecla >= 32 && tecla <= 126) {
            clave += (char)tecla;
            cout << '*';
        }
    }
    cout << endl;
    return clave;
}



int main() {
    try {
        inicializarArchivoUsuarios();
        inicializarArchivoHabitaciones();
        inicializarArchivoHuespedes();
        inicializarArchivoServicios();
        inicializarArchivoReservas();
        inicializarArchivoConsumos();
        inicializarArchivoFacturas();
    } catch (const exception& e) {
        cout << "Error al preparar los archivos: " << e.what() << endl;
        return 1;
    }

    cout << "--- SISTEMA HOTEL CONSOLE ---" << endl;
    int intentos = 0;
    while (intentos < 3) {
        string nombre;
        cout << "Usuario: ";
        if (!getline(cin, nombre)) return 0;
        cout << "Contrasena: ";
        string clave = leerContrasena();

        Usuario* usuario = 0;
        try {
            usuario = iniciarSesion(nombre, clave);
        } catch (const exception& e) {
            cout << e.what() << "\n" << endl;
            intentos++;
        }
        if (usuario != 0) {
            menuPrincipal(*usuario);
            delete usuario;
            return 0;
        }
    }
    cout << "Demasiados intentos fallidos. Programa terminado." << endl;
    return 1;
}
