#include <16f877a.h>                      //Tipo de MicroControlador a utilizar 

#fuses HS,NOPROTECT,NOWDT,NOLVP
#device ADC=8
#use delay(clock=20000000)                //Frec. de Osc. 20Mhz 

// establecemos comunicacion serial rx y tx 
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define LCD_RS_PIN      PIN_D0
#define LCD_RW_PIN      PIN_D1
#define LCD_ENABLE_PIN  PIN_D2
#define LCD_DATA4       PIN_D4
#define LCD_DATA5       PIN_D5
#define LCD_DATA6       PIN_D6
#define LCD_DATA7       PIN_D7    

#include <lcd.c>


#define BOTON_CAMBIO PIN_B0
#define TAM_COMANDO 20


void ajustaPWM(int16 ciclo);
int leer_comando(char* comando);
int separar_comando(char* comando, char* porcentaje_pwm);
void comando_pwm(int ciclo);
int esNumero(char* cadena);
void limpiarLCD();

int main()
{ 
   char comando[TAM_COMANDO]; // mensaje recibio: pwm=100]
   char porcentaje_pwm[3] = "50";
   int recibio_comando = 0;
   
   // inicializamos LCD
   lcd_init(); 

   // int valorAD
   // lectura de porcentaje duty cicle mediante puerto analogico AN0
   // establecemos puerto AN0 como analogico
   // setup_adc_ports(AN0);      

   // coloca el reloj interno del micro como fuente de reloj para el mod adc
   // setup_adc(ADC_CLOCK_INTERNAL);
   
   // coloca el puerto AN0 como entrada del modulo adc
   // set_adc_channel(0);
   
   // retardo para que se configure el modulo adc
   // delay_us(50);
   
   // configuramos el modulo ccp en modo pwm
   setup_ccp1(CCP_PWM);
   
   // configuramos trisc.2 como salida
   set_tris_d(PIN_C2); // salidas
   
   // configuramos timer 2 con prescaler 1:16, con un periodo de 155 y un 
   // postcaler 1:1
   setup_timer_2(T2_DIV_BY_16,155,1);   
   set_pwm1_duty(0);
   
   int ciclo = 0;
   
   printf("Para cambiar un control presione el boton. \n\n");
   printf("Cuando utilice el modo terminal, usar el comando: 'pwm=80'\n");
   while(1)
   { 
      recibio_comando = leer_comando(&comando);            
      
      if(recibio_comando) {
         int comando_correcto = separar_comando(&comando, &porcentaje_pwm);
         limpiarLCD();
         
         switch(comando_correcto) {
            case 1:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Com temperatura"); 
               lcd_gotoxy(1,2);
               printf(lcd_putc, "Temperatura:");
               break;
               
            case 2:
               ciclo = atol(porcentaje_pwm);
        
               comando_pwm(ciclo);
               break;
            case 3:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Com motor on"); 
               lcd_gotoxy(1,2);
               printf(lcd_putc, "Motor encendido");
               break;
            case 4:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Com motor off"); 
               lcd_gotoxy(1,2);
               printf(lcd_putc, "Motores apagado"); 
               break;
               
            case 5:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Com leds on"); 
               lcd_gotoxy(1,2);
               printf(lcd_putc, "Leds encendidos"); 
               break;
               
            case 6:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Com leds off"); 
               lcd_gotoxy(1,2);
               printf(lcd_putc, "Leds apagados"); 
               break;
            
            default:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Comando no");
               lcd_gotoxy(1,2);
               printf(lcd_putc, "reconocido");
         }
      }
      
      delay_us(10);
   }                              
   
   return 0;
}


int leer_comando(char* comando) {
   // se revisa si se han recibido datos desde el puerto serial
   if(kbhit()) {
      gets(comando);
      return 1;
   }
   
   return 0; // no recibio ningun comando
}

// el comando viene como "pwm=100"
int separar_comando(char* comando, char* porcentaje_pwm) {
   char delim[] = "=";
   char copia_comando[TAM_COMANDO]; 
   strcpy(copia_comando, comando); // Crea una copia para evitar modificar la cadena original
   char *token = strtok(copia_comando, delim);
   
   char *numero = strtok(0,delim);
   
   if(token){
      if (strcmp(token, (void*) "temperatura") == 0) {
         return 1; 
      }
         
      if (strcmp(token, (void*) "pwm") == 0) {
         if(numero){
            if(esNumero(numero)) {
               strcpy(porcentaje_pwm, numero);
               return 2;
            }
         }             
      }
        
      if (strcmp(token, (void*) "motor on") == 0) {
         return 3; 
      }
        
      if (strcmp(token, (void*) "motor off") == 0) {
         return 4; 
      }
        
      if (strcmp(token, (void*) "leds on") == 0) {
         return 5; 
      }
        
      if (strcmp(token, (void*) "leds off") == 0) {
         return 6; 
      }
   }
   
   
   return 0;
}

int esNumero(char *cadena) {
    // Iterar sobre cada caracter de la cadena
    while (*cadena != '\0') {
        // Verificar si el caracter no es un dígito
        if (!isdigit(*cadena)) {
            return 0;  // No es un número
        }
        cadena++;
    }
    return 1;  // Todos los caracteres son dígitos, es un número
}

void comando_pwm(int ciclo) {
   lcd_gotoxy(1,1);
   printf(lcd_putc,"Com pwm"); 
   lcd_gotoxy(1,2);
   printf(lcd_putc,"Senial pwm: %d", ciclo);
   
   if(ciclo >= 0 && ciclo <= 100) {
      set_pwm1_duty((int16)(ciclo * 624 / 100));
   } else {
      set_pwm1_duty(0);
   }
}

void limpiarLCD() {
   lcd_gotoxy(1,1);
   printf(lcd_putc,"                "); 
   lcd_gotoxy(1,2);
   printf(lcd_putc,"                "); 
}
