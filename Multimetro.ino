#include <LiquidCrystal.h>

#define rs 8
#define e 9
#define d4 10
#define d5 11
#define d6 12
#define d7 13

LiquidCrystal lcd(rs,e,d4,d5,d6,d7);
#define Vpin A0
#define BTN 2
#define tr 3

bool toogle = true;
bool firstON = true;

float val0Corrected = 0;
int cont=0;
float SND1=0,SND2=0;
float volt1,volt2;

float voltaje;
float corriente;

float R1=975000; //medidas por multimetro
float R2=218000;


/////////////////////////////////////////////////////////////////
//                                                             //
//            ISR (Interrupt Service Routine)                  //   
//                                                             //
/////////////////////////////////////////////////////////////////

void interrupcion(){
      cont++;
      if(cont>=2){
        cont=0;
      }
      toogle=!toogle;
      digitalWrite(tr,toogle);
      lcd.clear();
      delay(120);
}

/////////////////////////////////////////////////////////////////
//                                                             //
//                    SETUP                                    //
//                                                             //
/////////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(9600);
  lcd.begin(16,2);

  attachInterrupt(digitalPinToInterrupt(2),interrupcion,FALLING); //Activa la interrupcion del pin 2
                                                                  //Indica que la interrupcion es en flanco de bajada (por pull-up)
                                                                  //Identifica la funcion "interrupion" como el vector de interrupcion
  
  pinMode(BTN,INPUT_PULLUP);               //Se establece como entrada con pull-up el pin del boton
  pinMode(tr,OUTPUT);
  digitalWrite(tr,LOW);
} 

/////////////////////////////////////////////////////////////////
//                                                             //
//                      Loop principal                         //
//                                                             //
/////////////////////////////////////////////////////////////////

void loop() {
  
  if(firstON==true)
  {
    firstRoutine();
    
    firstON=false;
  }

  if(cont==1)
  {
    lcd.setCursor(0,0);
    lcd.print("Amperimetro");
    SND1 = readV(A0);
    delay(10);
    SND2 = readV(A1);
    delay(10);

    volt1=(SND1*(R1+R2))/(R2);
    volt2=(SND2*(R1+R2))/(R2);

    voltaje=volt1-volt2;
    corriente=(voltaje*1000)/100;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Amperimetro");
    lcd.setCursor(0,1);
    lcd.print(corriente,3);
    lcd.print("mA");

    Serial.println("Modo Amperimetro");
    Serial.print("I = ");
    Serial.print(corriente,3);
    Serial.println(" mA");
    delay(500);

  }
  else
  {
    lcd.setCursor(0,0);
    lcd.print("Voltimetro");
    SND1 = readV(A0);
    delay(10);
    SND2 = readV(A1);
    delay(10);

    volt1=(SND1*(R1+R2))/(R2);
    volt2=(SND2*(R1+R2))/(R2);

    voltaje=volt1-volt2;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Voltimetro");
    lcd.setCursor(0,1);
    lcd.print(voltaje,3);
    lcd.print("V");

    Serial.println("Modo Voltimetro");
    Serial.print("V = ");
    Serial.print(voltaje,3);
    Serial.println(" V");

    delay(500);
  }
}

/////////////////////////////////////////////////////////////////
//                                                             //
//                          Funciones                          //
//                                                             //
/////////////////////////////////////////////////////////////////

long readVcc() {
  long result;
    // lee 1.1V de referencia comparado con AVcc
  
    // En caso de arduino leonardo/seeduino GPRS
#if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#else //En caso de arduino uno
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
#endif
  delay(2); // espera a que Vref se estabilice
  ADCSRA |= _BV(ADSC); // convierte
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // calcula Vcc (en mV); 1126400 = 1.1*1024*1000
  return result;
}


float readV(float Vpin){
    float V=0, prom=0, val0=0,val0Corrected = 0;
    for(int i = 0; i < 64 ; i++){
    V = analogRead(Vpin);
    prom = V + prom;
  }
  prom = prom/64;
  val0 = prom * 5 / 1023.0;            //4.76 es el voltaje medido
  float supply = readVcc() / 1000.0;
  val0Corrected = supply / 5 * val0;   //4.76 es el voltaje medido
  return val0Corrected;
}

void firstRoutine(){
    Serial.println("Bienvenido a Voltimetro/Amperimetro");
    Serial.println("Para cambiar el modo presiona el boton");
    Serial.println("Medir corriente en serie y voltaje en paralelo");

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Bienvenido al");
    lcd.setCursor(0,1);
    lcd.print("multimetro");
    delay(1000);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Para cambiar el");
    lcd.setCursor(0,1);
    lcd.print("modo");
    delay(1000);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Presionar el");
    lcd.setCursor(0,1);
    lcd.print("boton");
    delay(1000);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Medir corriente");
    lcd.setCursor(0,1);
    lcd.print("en serie");
    delay(1000);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Medir voltaje");
    lcd.setCursor(0,1);
    lcd.print("en paralelo");
    delay(1000);

    cont=0;
}
