#include <stdio.h>
#include <iostream>
#include <fstream>
#include "linkLayer.h"
#include <stdio_ext.h>
using namespace std;

void mostrarInferfaces(interface_t &iface, pcap_if_t *avail_ifaces);
unsigned char *RecibirCaracter(interface_t &interface, unsigned char *tipo);
void EnviarCaracter(interface_t &interface, unsigned char datos, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *tipo);
bool DescubrimientoMaestro(interface_t &iface, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type);
bool DescubrimientoEsclavo(interface_t &iface, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type);
void EnvioInteractivo(interface &iface, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type);
void EnviarFichero(interface &iface, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type);
