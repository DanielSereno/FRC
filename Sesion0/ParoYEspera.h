#include <stdio.h>
#include <iostream>
#include <fstream>
#include "linkLayer.h"
#include <stdio_ext.h>
using namespace std;

void MostrarMenuPyS(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel);
void SeleccionMaestro (interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel);
void SeleccionEsclavo(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel);
void SondeoMaestro (interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel);
void SondeoEsclavo(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel);
void Esclavo(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel);
void RecibirTrama(interface *iface, unsigned char *type, unsigned char *&recibido);
void EnviarFicheroPyS (interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel, unsigned char modo);
void RecibirFicheroPyS (interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type);
void MostrarTrama (unsigned char *recibido, unsigned char BCE, bool enviada);
unsigned char CalcularBCE (unsigned char *cadena);