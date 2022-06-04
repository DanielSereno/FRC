#include "modulos.h"
/*
Modulo para escoger una interfaz.
*/
void mostrarInferfaces(interface_t &iface, pcap_if_t *avail_ifaces)
{
   avail_ifaces = GetAvailAdapters();

   // listado de las interfaces
   printf("Interfaces disponibles: \n");
   int i = 0;
   while (avail_ifaces->next != NULL)
   {
      printf("[%d] %s \n", i, avail_ifaces->name);
      avail_ifaces = avail_ifaces->next;
      i++;
   }

   // selección de interfaz
   printf("Seleccione interfaz: ");
   int sel;
   cin >> sel;
   while (sel < 0 || sel > i)
   { // control de errores
      printf("selección incorrecta, escoja un número válido \n");
      cin >> sel;
   }

   /*
   Se accede a la interfaz seleccionada, para ello se coloca el puntero al principio de la lista
   y se va avanzando en la lista
   */
   avail_ifaces = GetAvailAdapters();
   for (i = 0; i < sel; i++)
   {
      avail_ifaces = avail_ifaces->next;
   }

   // mostrar el nombre y la mac origen
   printf("Interfaz elegida: %s \n", avail_ifaces->name);
   setDeviceName(&iface, avail_ifaces->name);
   GetMACAdapter(&iface);
   printf("La Mac origen es: ");
   for (i = 0; i < 6; i++)
   {
      printf("%X", iface.MACaddr[i]);
      if (i != 5)
      {
         printf(":");
      }
   }
   printf("\n");
}

/*
   Módulo que dada una interfaz y un tipo recibe un carácter en forma de trama y lo devuelve.
*/
unsigned char *RecibirCaracter(interface_t &interface, unsigned char *type)
{
   int cont = 0;
   int i = 14; // a partir de la posicion 14 empiezan los datos
   unsigned char *p;
   // recibir trama
   apacket_t trama = ReceiveFrame(&interface);
   // Quedarse con el campo datos
   if (trama.packet != nullptr)
   {
      if (trama.packet[12] == type[0] && trama.packet[13] == type[1])
      {
         // contar el numero de caracteres e imprimirlos
         while (trama.packet[i] != '\0') // mientras no se encuentre el fin de cadena
         {
            printf("%c", trama.packet[i]); // imprimir caracter
            i++;
            cont++;
         }
         printf("\n");
         printf("Número de caracteres recibidos: %d\n", cont);
      }
   }

   return 0;
}

/*
   Módulo que dado una interfaz, mac origen y destino y unos datos, construye y envía una trama con los datos a la mac destino
*/
void EnviarCaracter(interface_t &interface, unsigned char datos, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type)
{
   // reservar memoria
   unsigned char *car = (unsigned char *)malloc(1 * sizeof(char));
   // Almacenar datos
   car[0] = datos;
   // Construir trama
   unsigned char *trama = BuildFrame(mac_origen, mac_destino, type, car);
   // Enviar trama
   SendFrame(&interface, trama, 1);
   // Liberar memoria de datos
   free(car);
   // Liberar memoria de trama
   free(trama);
}

/*
   Maestro:
   ConstruirTrama
   EnviarTrama
   Mientras !Recibido
      RecibirTrama
      If ( TG && TC)
         obtenerMacOrigen
*/

bool DescubrimientoMaestro(interface_t &iface, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type)
{
   type[1] = 0x01; // maestro envia trama de descubrimiento
   apacket_t esclavo;
   unsigned char macEsclavo[6];
   bool enc = false;
   // mac de broadcast
   unsigned char broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

   // ConstruirTrama
   unsigned char *trama = BuildHeader(mac_origen, broadcast, type);
   // enviar trama
   SendFrame(&iface, trama, 0);
   printf("Esperando que se una la estación esclava \n");

   while (!enc)
   {
      esclavo = ReceiveFrame(&iface);
      if (esclavo.packet != nullptr)
      {
         // si es una trama de nuestro grupo y si es una trama del esclavo
         if (esclavo.packet[12] == type[0] && esclavo.packet[13] == 2)
         {
            enc = true;
         }
      }
   }
   // obetenemos la mac del esclavo
   memcpy(macEsclavo, esclavo.packet + 6, 6);

   memcpy(mac_destino, macEsclavo, 6);
   // imprimir mac
   printf("Estación encontrada. La MAC es: ");
   for (int i = 0; i < 6; i++)
   {
      printf("%X", macEsclavo[i]);
      if (i != 5)
         printf(":");
   }
   printf("\n");
   free(trama);
   return enc;
   type[1] = 00;
}

/*
   Esclavo:
   Mientras !Recibido
   if (TG y TP)
      ObtenerMacOrigen
   ConstruirTrama
   EnviarTrama
*/

bool DescubrimientoEsclavo(interface_t &iface, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type)
{
   type[1] = 0x02; // esclavo envia trama de descubrimiento al maestro
   apacket_t maestro;
   bool enc = false;
   unsigned char tipo[2];
   unsigned char macMaestro[6];

   printf("Esperando que se una la estación maestra \n");
   while (!enc)
   {
      maestro = ReceiveFrame(&iface);
      if (maestro.packet != nullptr)
      {
         // si trama de nuestro gurpo y si trama de maestro
         if (maestro.packet[12] == type[0] && maestro.packet[13] == 1)
         {
            enc = true;
         }
      }
   }
   // obtenemos la mac
   memcpy(macMaestro, maestro.packet + 6, 6);

   memcpy(mac_destino, macMaestro, 6);
   // imprimir mac
   printf("Estación encontrada. La mac es: ");
   for (int i = 0; i < 6; i++)
   {
      printf("%X", macMaestro[i]);
      if (i != 5)
         printf(":");
   }
   printf("\n");
   // construir y enviar trama
   unsigned char *trama = BuildHeader(mac_origen, macMaestro, type);
   SendFrame(&iface, trama, 0);
   free(trama);
   type[1] = 00; // modo de funcionamiento normal
   return enc;
}

/*
Envio de fichero:
    char cadena[255]
    AbrirFichero
    Mientras !FinFichero
        if f.gcount() >0
        f.read(cadena, 254)
        cadena[f.gcount()]='\0'
        construirTrama
        EnviarTrama
*/

void EnviarFichero(interface &iface, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type)
{
   __fpurge(stdin);
   unsigned char *cadena = (unsigned char *)malloc(255 * sizeof(char));
   ifstream entrada;     // flujo de entrada
   unsigned char *trama; // trama que se envia
   try
   {
      /* code */
      entrada.open("envio.txt", ios::in); // apertura del fichero
   }
   catch (const std::exception &e)
   {
      std::cerr << "Error al abrir el fichero" << '\n';
   }

   while (!entrada.eof())
   {
      entrada.read((char *)cadena, 254); // casting a char *s
      cadena[entrada.gcount()] = '\0';   // fin de cadena
      if (entrada.gcount() > 0)
      {
         trama = BuildFrame(mac_origen, mac_destino, type, cadena);
         SendFrame(&iface, trama, entrada.gcount());
      }
   }
   printf("Fichero enviado correctamente.\n");

   free(trama);
   free(cadena);
   entrada.close();
}

void EnvioInteractivo(interface &iface, unsigned char *mac_origen, unsigned char *mac_destino, unsigned char *type)
{
   __fpurge(stdin);
   unsigned char car;
   car = ' ';
   unsigned char *recibido;
   printf("Pulse los caracteres a enviar: \n");
   while (car != 27)
   {
      recibido = RecibirCaracter(iface, type);
      if (recibido != 0)
      {
         // printf("Caracter recibido: %c\n", recibido);
      }
      if (kbhit())
      {
         car = getch();
         EnviarCaracter(iface, car, mac_origen, mac_destino, type);
      }
   }
}

