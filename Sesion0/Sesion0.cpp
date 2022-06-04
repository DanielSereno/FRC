//============================================================================
// ----------- PRACTICAS DE FUNDAMENTOS DE REDES DE COMUNICACIONES -----------
// ---------------------------- CURSO 2021/22 --------------------------------
// ----------------------------- SESION1.CPP ---------------------------------
//============================================================================

#include <stdio.h>
#include <iostream>
#include "linkLayer.h"
#include "modulos.h"
#include "ParoYEspera.h"
#include <stdio_ext.h>
using namespace std;

int main()
{
   char car = ' ';
   unsigned char mac_src[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   unsigned char mac_dst[6];
   unsigned char type[2] = {0x30, 0x00}; // grupo 5 {48 + nº grupo, modo de funcionamiento} 1:peticion 2:contestacion 0:fncionamiento normal

   interface_t iface;
   pcap_if_t *avail_ifaces = NULL;

   printf("\n----------------------------\n");
   printf("------ SESION 1 - FRC ------\n");
   printf("----------------------------\n");

   mostrarInferfaces(iface, avail_ifaces );

   memcpy(mac_src, iface.MACaddr, 6);
   // selección de número de grupo
   printf("Introduzca el número de grupo: \n");
   int grupo;
   cin >> grupo;

   type[0] = 48 + grupo;

   // abrimos el puerto
   int Puerto = OpenAdapter(&iface);

   /* Control de errores */
   if (Puerto != 0)
   {
      printf("Error al abrir el puerto\n");
   }
   else
   {
      printf("Puerto abierto correctamente\n");
   }

   __fpurge(stdin);

   printf("Seleccione el modo de la estación: \n");
   printf("[1] Modo Maestra \n");
   printf("[2] Modo Esclava \n");

   int sel=0;
   cin >> sel;

   if (sel == 1)
   {
      if (DescubrimientoMaestro(iface, mac_src, mac_dst,type))
      {
         type[1] = 00; //funcionamiento normal
         printf("Selección de modo:\n");
         printf("[F1] - Envío de caracteres interactivo\n");
         printf("[F2] - Envío de un fichero\n");
         printf("[F3] - Protocolo paro y espera\n");
         printf("[F4] - Introducción de errores durante el protocolo\n");
         printf("[ESC] - Salir\n");
         while (car != 27)
         {
            if (kbhit())
               car = getch();

            if (car == 27) // si es una tecla de función
            {
               if (kbhit())
               {
                  car = getch();
                  if (car == 'O') // si es (F1,F2,F3,F4)
                  {
                     car = getch();
                  }
               }
               switch (car)
               {
               case 'P':
                  printf("\nEnvío de caracteres interactivo: \n");
                  EnvioInteractivo(iface, mac_src, mac_dst, type);
                  printf("Selección de modo:\n[F1] - Envío de caracteres interactivo\n[F2] - Envío de un fichero\n[F3] - Protocolo paro y espera\n[F4] - Introducción de errores durante el protocolo\n[ESC] - Salir\n");
                  break;

               case 'Q':
                  printf("\nEnvío de fichero:\n");
                  EnviarFichero(iface, mac_src, mac_dst, type);
                  printf("Selección de modo:\n[F1] - Envío de caracteres interactivo\n[F2] - Envío de un fichero\n[F3] - Protocolo paro y espera\n[F4] - Introducción de errores durante el protocolo\n[ESC] - Salir\n");
                  break;

               case 'R':
                  printf("\nProtocolo paro y espera. Para salir pulse ESC:\n");
                  MostrarMenuPyS(iface, mac_src, mac_dst, type, sel);
                  printf("Selección de modo:\n[F1] - Envío de caracteres interactivo\n[F2] - Envío de un fichero\n[F3] - Protocolo paro y espera\n[F4] - Introducción de errores durante el protocolo\n[ESC] - Salir\n");
                  break;

               case 27:
                  printf("\nSalir \n");
                  break;

               default:
                  break;
               }
            }
         }
      }
   }
   else if (sel == 2)
   {
      if (DescubrimientoEsclavo(iface, mac_src, mac_dst, type))
      {
         type[1] = 00; //funcionamiento normal
         printf("Selección de modo:\n");
         printf("[F1] - Envío de caracteres interactivo\n");
         printf("[F3] - Protocolo paro y espera\n");
         printf("[ESC] - Salir\n");
         while (car != 27)
         {
            RecibirCaracter(iface, type); // siempre tiene que estar recibiendo
            if (kbhit())
            {
               car = getch();
               if (car == 27) // si es tecla de función
               {
                  car = getch();
                  if (car == 'O') // si es (F1,F2,F3,F4)
                  {
                     car = getch();
                  }
               }
               switch (car)
               {
               case 'P':
                  printf("\nEnvío de caracteres interactivo: \n");
                  EnvioInteractivo(iface, mac_src, mac_dst, type);
                  printf("Selección de modo:\n[F1] - Envío de caracteres interactivo\n[ESC] - Salir\n");
                  break;

               case 'R':
                  printf("\nProtocolo paro y espera. Para salir pulse ESC:\n");
                  MostrarMenuPyS(iface, mac_src, mac_dst, type, sel);
                  printf("Selección de modo:\n[F1] - Envío de caracteres interactivo\n[F3] - Protocolo paro y espera\n[ESC] - Salir\n");
                  break;

               case 27:
                  printf("\nSalir \n");
                  break;
               }
            }
         }
      }
   }

   // unsigned char recibido;

   /*
      Bucle utilizado para enviar y recibir caracteres
   */
   // while (car != 27)
   // {
   //    // recibir caracter
   //    recibido = RecibirCaracter(iface);
   //    if (recibido != 0)
   //    {
   //       // imprimir caracter
   //       printf("Recibido: %c\n", recibido);
   //    }
   //    if (kbhit())
   //    {
   //       car = getch();
   //       if (car != 27)
   //          EnviarCaracter(iface, car, iface.MACaddr, mac_dst, type);
   //    }
   // }

   /*
   Maestro:
      ListarInterfaces
      Descubrimiento
      Mientras tecla !=ESC
         if kbhit()
            tecla = getch()
            if tecla = !27

            else
               if kbhit() (comprobar si hay algo en el buffer)
                  tecla = getch()
                  if tecla =='O'
                     tecla = getch()
   */

   /*
   Esclavo:
      ListarInterfaces
      Descubrimiento
      Mientras tecla !=ESC
         RecibirTrama()
         hay que mostrar el numero de caracteres recibidos

   */

   CloseAdapter(&iface);

   return 0;
}
