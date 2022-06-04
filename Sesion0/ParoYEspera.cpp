#include "ParoYEspera.h"

void MostrarMenuPyS(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel)
{
   type[1] = 0x00; // funcionamiento normal
   if (sel == 1)   // maestro
   {
      int num;
      printf("Seleccione el tipo de operacion: \n");
      printf("[1] Operación Selección.\n");
      printf("[2] Operación Sondeo.\n");
      printf("[3] Salir.\n");
      cin >> num;
      switch (num)
      {
      case 1:
         SeleccionMaestro(iface, mac_src, mac_dst, type, sel);
         break;

      case 2:
         SondeoMaestro(iface, mac_src, mac_dst, type, sel);
         break;

      case 3:
         break;
      }
   }
   else if (sel == 2) // esclavo
   {
      cout << "Estas en modo esclavo" << endl;
      Esclavo(iface, mac_src, mac_dst, type, sel);
   }
}

/*
Maestro:
 ContruirTC(ENQ)
 EnviarTC()
 While !ACK
   RecibirTramaProtocolo
 EnviarFicheroProtocolo
 ConstruirTC(EOT)
 EnviarTC()
 Mientras !ACK
   RecibirTRamaFin
*/

void Esclavo(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel)
{
   bool enc = false;
   unsigned char *p = (unsigned char *)malloc(3 * sizeof(unsigned char));
   unsigned char *recibido = (unsigned char *)malloc(259 * sizeof(unsigned char));
   bool R;

   while (!enc)
   {
      RecibirTrama(&iface, type, recibido);
      if (recibido[0] != 0)
      {
         if (recibido[1] == 5) // si se recibe un enq
         {
            MostrarTrama(recibido, 0, false);
            enc = true;
         }
         if (recibido[0] == 'R')
         {
            R = true;
            p[0] = 'R'; // seleccion
         }
         else
         {
            R = false;
            p[0] = 'T';
         }
         recibido[0] = 0;
      }
   }

   p[1] = 06;  // ACK
   p[2] = '0'; // primer numero de trama
   unsigned char *trama = BuildFrame(mac_src, mac_dst, type, p);
   SendFrame(&iface, trama, 3);
   MostrarTrama(p, 0, true);

   if (R)
   {
      SeleccionEsclavo(iface, mac_src, mac_dst, type, sel);
   }
   else
   {
      SondeoEsclavo(iface, mac_src, mac_dst, type, sel);
   }

   free(recibido);
   free(trama);
   free(p);
}

void SeleccionMaestro(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel)
{
   bool enc = false;
   unsigned char *p = (unsigned char *)malloc(3 * sizeof(unsigned char));
   unsigned char *recibido = (unsigned char *)malloc(259 * sizeof(unsigned char));
   p[0] = 'R'; // seleccion
   p[1] = 05;  // ENQ
   p[2] = '0'; // primer numero de trama
   unsigned char *trama = BuildFrame(mac_src, mac_dst, type, p);
   SendFrame(&iface, trama, 3);

   MostrarTrama(p, 0, true);

   while (!enc)
   {
      RecibirTrama(&iface, type, recibido);
      if (recibido[0] != 0)
      {
         if (recibido[1] == 6) // si se recibe un ack
         {
            MostrarTrama(recibido, 0, false);
            enc = true;
         }
         recibido[0] = 0;
      }
   }

   EnviarFicheroPyS(iface, mac_src, mac_dst, type, sel, 'R');

   enc = false;
   p[0] = 'R'; // seleccion
   p[1] = 04;  // EOT
   p[2] = '0'; // primer numero de trama
   trama = BuildFrame(mac_src, mac_dst, type, p);
   SendFrame(&iface, trama, 3);
   MostrarTrama(p, 0, true);

   while (!enc)
   {
      RecibirTrama(&iface, type, recibido);
      if (recibido[0] != 0)
      {
         if (recibido[1] == 6) // si se recibe un ack
         {
            MostrarTrama(recibido, 0, false);
            enc = true;
         }
         recibido[0] = 0;
      }
   }

   free(recibido);
   free(trama);
   free(p);
}

void SeleccionEsclavo(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel)
{

   RecibirFicheroPyS(iface, mac_src, mac_dst, type);
}

void SondeoMaestro(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel)
{
   bool enc = false;
   unsigned char *p = (unsigned char *)malloc(3 * sizeof(unsigned char));
   unsigned char *recibido = (unsigned char *)malloc(259 * sizeof(unsigned char));

   p[0] = 'T'; // sondeo
   p[1] = 05;  // ENQ
   p[2] = '0'; // primer numero de trama
   unsigned char *trama = BuildFrame(mac_src, mac_dst, type, p);
   SendFrame(&iface, trama, 3);
   MostrarTrama(p, 0, true);

   while (!enc)
   {
      RecibirTrama(&iface, type, recibido);
      if (recibido[0] != 0)
      {
         if (recibido[1] == 6) // si se recibe un ack
         {
            MostrarTrama(recibido, 0, false);
            enc = true;
         }
         recibido[0] = 0;
      }
   }
   RecibirFicheroPyS(iface, mac_src, mac_dst, type);
   free(recibido);
   free(trama);
   free(p);
}

void SondeoEsclavo(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel)
{
   bool enc = false;
   unsigned char *p = (unsigned char *)malloc(3 * sizeof(unsigned char));
   unsigned char *recibido = (unsigned char *)malloc(259 * sizeof(unsigned char));
   int cont = 0;

   EnviarFicheroPyS(iface, mac_src, mac_dst, type, sel, 'T');

   enc = false;
   p[0] = 'T'; // sondeo
   p[1] = 04;  // EOT
   p[2] = '0'; // primer numero de trama
   unsigned char *trama = BuildFrame(mac_src, mac_dst, type, p);
   SendFrame(&iface, trama, 3);
   MostrarTrama(p, 0, true);

   while (!enc)
   {
      RecibirTrama(&iface, type, recibido);
      if (recibido[0] != 0)
      {
         MostrarTrama(recibido, 0, false);
         if (recibido[1] == 6) // si se recibe un ack
         {
            enc = true;
         }
         else
         {
            cont++;
            if (cont % 2 == 0)
            {
               p[2] = '0';
            }
            else
            {
               p[2] = '1';
            }
            trama = BuildFrame(mac_src, mac_dst, type, p);
            SendFrame(&iface, trama, 3);
            MostrarTrama(p, 0, true);
         }
         recibido[0] = 0;
      }
   }

   free(recibido);
   free(trama);
   free(p);
}

void MostrarTrama(unsigned char *recibido, unsigned char BCE, bool enviada)
{
   if (enviada)
   {
      cout << "E  ";
   }
   else
   {
      cout << "R  ";
   }
   cout << recibido[0] << "   ";
   if (recibido[1] == 2)
   {
      cout << "STX   ";
      cout << recibido[2] << "   ";
      cout << int(recibido[recibido[3] + 4]) << "   ";
      if (!enviada)
      {
         cout << int(BCE);
      }
   }
   else if (recibido[1] == 5)
   {
      cout << "ENQ   ";
      cout << recibido[2] << "   ";
   }
   else if (recibido[1] == 4)
   {
      cout << "EOT   ";
      cout << recibido[2] << "   ";
   }
   else if (recibido[1] == 6)
   {
      cout << "ACK   ";
      cout << recibido[2] << "   ";
   }
   else if (recibido[1] == 21)
   {
      cout << "NACK   ";
      cout << recibido[2] << "   ";
   }
   cout << endl;
}

unsigned char CalcularBCE(unsigned char *cadena)
{
   int i = 5;
   unsigned char BCE;
   BCE = cadena[4];

   while (i < cadena[3] + 4)
   {
      BCE = BCE ^ cadena[i];
      i++;
   }

   if (BCE == 0 || BCE == 255)
      BCE = 1;
   return BCE;
}

void RecibirFicheroPyS(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type)
{
   ofstream fSalida;
   unsigned char BCE;
   bool enc = false;
   int cont1 = 0;
   int cont2 = 0;
   unsigned char *p = (unsigned char *)malloc(3 * sizeof(unsigned char));
   unsigned char *recibido = (unsigned char *)malloc(259 * sizeof(unsigned char));
   int sel = 0;
   fSalida.open("Rprotoc.txt", ios::out);

   while (!enc && sel != 1)
   {
      RecibirTrama(&iface, type, recibido);
      if (recibido[0] != 0)
      {
         if (recibido[1] == 4 && recibido[0] == 'R') // si se recibe un eot
         {
            MostrarTrama(recibido, 0, false);
            p[0] = 'R'; // seleccion
            p[1] = 06;  // ACK
            p[2] = '0'; // primer numero de trama
            unsigned char *trama = BuildFrame(mac_src, mac_dst, type, p);
            SendFrame(&iface, trama, 3);
            MostrarTrama(p, 0, false);
            enc = true;
         }
         else if (recibido[1] == 4 && recibido[0] == 'T')
         {
            MostrarTrama(recibido, 0, false);
            p[0] = 'T'; // sondeo

            printf("Acepta el cierre de la comunicacion: \n");
            printf("[1] Si \n");
            printf("[2] No \n");

            cin >> sel;

            if (sel == 1)
            {
               p[1] = 06;
               enc = true;
            }
            else
            {
               p[1] = 21;
            }
            p[2] = recibido[2];

            unsigned char *trama = BuildFrame(mac_src, mac_dst, type, p);
            SendFrame(&iface, trama, 3);
            MostrarTrama(p, 0, true);

            cont2++;
         }
         else if (recibido[1] == 2) // si se recibe un STX
         {
            BCE = CalcularBCE(recibido);
            if (BCE == recibido[recibido[3] + 4])
            {
               p[1] = 06; // ACK
               fSalida.write((const char *)recibido + 4, recibido[3]);
            }
            else
            {
               p[1] = 21; // NACK
            }
            if (recibido[0] == 'R')
            {
               p[0] = 'R'; // seleccion
            }
            else
            {
               p[0] = 'T'; // sondeo
            }
            p[2] = recibido[2];
            MostrarTrama(recibido, BCE, false);
            unsigned char *trama = BuildFrame(mac_src, mac_dst, type, p);
            SendFrame(&iface, trama, 3);
            MostrarTrama(p, 0, true);

            cont1++;
            free(trama);
         }
         recibido[0] = 0;
      }
   }
   free(recibido);
   free(p);
}

void RecibirTrama(interface *iface, unsigned char *type, unsigned char *&recibido)
{
   apacket_t trama = ReceiveFrame(iface);
   if (trama.packet != nullptr)
   {
      if (trama.packet[12] == type[0] && trama.packet[13] == type[1])
      {
         recibido[0] = trama.packet[14];
         recibido[1] = trama.packet[15];
         recibido[2] = trama.packet[16];
         if (recibido[1] == 2)
         {
            recibido[3] = trama.packet[17];
            int i = 0;
            while (i < trama.packet[17])
            {
               recibido[i + 4] = trama.packet[i + 18];
               i++;
            }
            recibido[i + 4] = trama.packet[i + 18];
         }
      }
   }
}

void EnviarFicheroPyS(interface &iface, unsigned char *mac_src, unsigned char *mac_dst, unsigned char *type, int sel, unsigned char modo)
{
   __fpurge(stdin);
   unsigned char *cadena = (unsigned char *)malloc(259 * sizeof(char));
   ifstream entrada;     // flujo de entrada
   unsigned char *trama; // trama que se envia
   unsigned char BCE;
   unsigned char *recibido = (unsigned char *)malloc(259 * sizeof(unsigned char));
   int cont = 0;
   int i;
   int numErrores = 0;
   bool enc;
   recibido[0] = 0;
   char car = ' ';
   char aux = ' ';

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
            if (car == 'S')
            {
               numErrores++;
               cout << "INTRODUCIENDO ERROR" << endl;
            }
         }
      }
      if (modo == 'R')
      {
         cadena[0] = 'R';
      }
      else
      {
         cadena[0] = 'T';
      }
      cadena[1] = 2;

      if (cont % 2 == 0)
      {
         cadena[2] = '0';
      }
      else
      {
         cadena[2] = '1';
      }

      entrada.read((char *)cadena + 4, 254);
      cadena[3] = entrada.gcount();

      if (entrada.gcount() > 0)
      {

         cadena[entrada.gcount() + 4] = CalcularBCE(cadena);
         if (numErrores > 0)
         {
            aux = cadena[4];
            cadena[4] = 'ç';
            numErrores--;
         }

         trama = BuildFrame(mac_src, mac_dst, type, cadena);
         SendFrame(&iface, trama, cadena[3] + 5);
         MostrarTrama(cadena, 0, true);
      }

      cont++;

      enc = false;
      while (!enc)
      {
         if (car == 27) // si es una tecla de función
         {
            if (kbhit())
            {
               car = getch();
               if (car == 'O') // si es (F1,F2,F3,F4)
               {
                  car = getch();
               }
               if (car == 'S')
               {
                  numErrores++;
                  cout << "INTRODUCIENDO ERROR" << endl;
               }
            }
         }
         RecibirTrama(&iface, type, recibido);
         if (recibido[0] != 0)
         {
            if (recibido[1] == 6) // si se recibe un ack
            {
               MostrarTrama(recibido, 0, false);
               enc = true;
            }
            else if (recibido[1] == 21)
            {
               MostrarTrama(recibido, 0, false);

               cadena[4] = aux;
               trama = BuildFrame(mac_src, mac_dst, type, cadena);
               SendFrame(&iface, trama, cadena[3] + 5);
               MostrarTrama(cadena, 0, true);
               //cont++;
            }
            recibido[0] = 0;
         }
      }
   }
   //cout << numErrores << endl;

   free(recibido);
   free(trama);
   free(cadena);
   entrada.close();
}