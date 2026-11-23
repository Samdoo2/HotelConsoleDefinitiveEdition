#pragma once
#include <string>
using namespace std;

enum class TipoHabitacion { Sencilla, Doble, Suite, Familiar, Otra };
enum class EstadoHabitacion { Disponible, Mantenimiento, FueraDeServicio };
enum class EstadoReserva { Pendiente, Confirmada, Cancelada };
enum class EstadoPago { Pendiente, Pagada };


TipoHabitacion convertirTipoHabitacion(string texto);
EstadoHabitacion convertirEstadoHabitacion(string texto);
EstadoReserva convertirEstadoReserva(string texto);
EstadoPago convertirEstadoPago(string texto);
