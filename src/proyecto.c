#include <16f877a.h>                      //Tipo de MicroControlador a utilizar 

#fuses HS,NOPROTECT,NOWDT,NOLVP
#device ADC=10
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

#define TAM_COMANDO 20

void ajustaPWM(int16 ciclo);
int leer_comando(char* comando);
int separar_comando(char* comando, char* porcentaje_pwm);
void pwm(int ciclo);
int esNumero(char* cadena);
void limpiarLCD();
void prenderLeds();
void apagarLeds(); 
void imprimirPrompt();
void temperatura();

int main()
{ 
   char comando[TAM_COMANDO]; // mensaje recibio: pwm=100]
   char porcentaje_pwm[3] = "50";
   int recibio_comando = 0;
   
   // ============ inicializamos LCD ============
   lcd_init(); 

   // ============ inicializamos ADC ============
   // establecemos puerto AN0 como analogico
   setup_adc_ports(AN0);      
   // coloca el reloj interno del micro como fuente de reloj para el mod adc
   setup_adc(ADC_CLOCK_INTERNAL);
   // coloca el puerto AN0 como entrada del modulo adc
   set_adc_channel(0);
   // retardo para que se configure el modulo adc
   delay_us(50);
   
   // ============ inicializamos CCP ============
   // configuramos el modulo ccp en modo pwm
   setup_ccp1(CCP_PWM);
   // configuramos trisc.2 como salida
   set_tris_d(PIN_C2); // salidas
   // configuramos timer 2 con prescaler 1:16, con un periodo de 155 y un 
   // postcaler 1:1
   setup_timer_2(T2_DIV_BY_16,155,1);   
   set_pwm1_duty(0);
   
   int ciclo = 0;
   
   printf("Proyecto Final Microcomputadoras. \r\n\r\n");
   printf("==================================== \r\n");
   printf("====== INTERPRETE DE COMANDOS ====== \r\n");
   printf("==================================== \r\n\r\n");
   printf("1. temperatura\r\n");
   printf("2. pwm=10\r\n");
   printf("3. motor on\r\n");
   printf("4. motor off\r\n");
   printf("5. leds on\r\n");
   printf("6. leds off\r\n\r\n");
   imprimirPrompt();
   while(1)
   { 
      recibio_comando = leer_comando(&comando);            
      
      if(recibio_comando) {
         int comando_correcto = separar_comando(&comando, &porcentaje_pwm);
         limpiarLCD();
         
         switch(comando_correcto) {
            case 1:
               temperatura();
               imprimirPrompt();
               break;
               
            case 2:
               ciclo = atol(porcentaje_pwm);
               pwm(ciclo);
               imprimirPrompt();
               break;
            case 3:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Com motor on"); 
               lcd_gotoxy(1,2);
               printf(lcd_putc, "Motor encendido");
               imprimirPrompt();
               break;
            case 4:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Com motor off"); 
               lcd_gotoxy(1,2);
               printf(lcd_putc, "Motores apagado");
               imprimirPrompt();
               break;
               
            case 5:
               prenderLeds();
               imprimirPrompt();
               break;
               
            case 6:
               apagarLeds();
               imprimirPrompt();
               break;
            
            default:
               lcd_gotoxy(1,1);
               printf(lcd_putc, "Comando no");
               lcd_gotoxy(1,2);
               printf(lcd_putc, "reconocido");
               printf("Comando no reconocido\r\n");
               imprimirPrompt();
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
   // primero separamos el prompt
   char copia_comando[TAM_COMANDO]; 
   strcpy(copia_comando, comando);

   char *comando_sin_prompt = strtok(copia_comando, (void*)">");
   //printf("%s\r\n", comando_sin_prompt);
   
   char delim[] = "=";
   char *token = strtok(comando_sin_prompt, delim);
   
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

void pwm(int ciclo) {
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

void prenderLeds() {
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Com leds on"); 
   lcd_gotoxy(1,2);
   printf(lcd_putc, "Leds encendidos");
   output_b(0xFF);
}

void apagarLeds() {
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Com leds off"); 
   lcd_gotoxy(1,2);
   printf(lcd_putc, "Leds apagados");
   
   output_b(0x00);
}

void temperatura() {
   /*
      1 grado centrigrado = 10 mv
      - Para resolucion de 10 bits con referencia de 5 volts:
      (1 C/ 10mV) (5000 mV / 1023) * lectura ADC
      (500 / 1023) * lectura ADC
   */
   // coloca el puerto AN0 como entrada del modulo adc
   set_adc_channel(0);
   // retardo para que se configure el modulo adc
   delay_us(50);
   
   float temperatura = (float)(500 * read_adc()) / 1023.0;
   
   lcd_gotoxy(1,1);
   printf(lcd_putc, "Com temperatura"); 
   lcd_gotoxy(1,2);
   printf(lcd_putc, "Temp: %f", temperatura);
}

void limpiarLCD() {
   lcd_gotoxy(1,1);
   printf(lcd_putc,"                "); 
   lcd_gotoxy(1,2);
   printf(lcd_putc,"                "); 
}

void imprimirPrompt() {
   printf("Ingresa tu comando: \r\n");
   printf(">");
}
