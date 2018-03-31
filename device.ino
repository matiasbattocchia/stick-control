#include <Tone.h>

#define MILLIS_IN_MIN 60000
#define DIFICULTAD 25 // en milisegundos
#define DRUM A3

Tone tono;
#define BUZZER 11

#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

#define RIGHT  0
#define UP     1
#define DOWN   2
#define LEFT   3
#define SELECT 4
#define NONE   5
#define BOTONES 0

int read_buttons() {
  int adc_key_in = analogRead(BOTONES);
  // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
  // we add approx 50 to those values and check to see if we are close
  if (adc_key_in > 1000) return NONE;
  if (adc_key_in < 50)   return RIGHT;  
  if (adc_key_in < 195)  return UP; 
  if (adc_key_in < 380)  return DOWN; 
  if (adc_key_in < 555)  return LEFT; 
  if (adc_key_in < 790)  return SELECT;
 
  return NONE;
}


bool on = false;
bool apretado = false;
int error = 0;
unsigned long acumulado, hit;
unsigned int bpm, beat, compass, beats, intervalo, atras, adelante;

void setup() {
  bpm = 60;
  intervalo = MILLIS_IN_MIN / bpm;
  
  tono.begin(BUZZER);
  pinMode(DRUM, INPUT);
  digitalWrite(DRUM, INPUT_PULLUP);
 
  lcd.begin(16, 2);
  render();
}

char buffer[9];

void render() {
  lcd.clear();
  sprintf(buffer, "%3u-%u", compass, beat);
  lcd.print(buffer);
  
  lcd.setCursor(8,0);
  sprintf(buffer, "%3u BPM", bpm);
  lcd.print(buffer);
  
  lcd.setCursor(0,1);
  switch (error) {
    case -3: { lcd.print("s*_____f"); break; }
    case -2: { lcd.print("s_*____f"); break; }
    case -1: { lcd.print("s__*___f"); break; }
    case  0: { lcd.print("s_BIEN_f"); break; }
    case  1: { lcd.print("s___*__f"); break; }
    case  2: { lcd.print("s____*_f"); break; }
    case  3: { lcd.print("s_____*f"); break; }
  }
}

unsigned int precision(unsigned int ms) {
  return ms / DIFICULTAD;
}

void loop() {
  if (on) {
    if (millis() > acumulado) {
      beat = beats % 4 + 1;
      compass = beats / 4 + 1;

      if (beat == 1) {
        tono.play(NOTE_A7, 150);
      } else {
        tono.play(NOTE_C7, 150);
      }
      
      acumulado += intervalo;
      beats += 1;
      render();
    }
  } else {
    acumulado = millis();
    beats = 0;
  }

  int button = read_buttons(); // read the buttons
  
/*
  if (digitalRead(DRUM) == LOW && !apretado) {
    hit = acumulado - millis();
    apretado = true;
    render();
  }
*/

/*  TODO
 *  Tempo check, quiet count
 *  Scoring
 *  Dificultad
 *  Duración nota (negra, corchea...)
 *  Tiempos compás
 *  Acentos
 *  Resolución metrónomo
 *  Guardar configuración anterior
 *  Progreso de las lecciones
 *  Múltiples instrumentos/cuerpos
 *  
 *  
 */
 
  if (!apretado) {
    switch (button) {
      case RIGHT: {
        apretado = true;
        hit = millis();
        // acumulado tiene el valor temporal del beat siguiente
        atras = acumulado - hit;
        adelante = hit - (acumulado - intervalo);
        // estimamos la intención del golpe, puede estar dirigido
        // a la marca que acaba de pasar o a la que está por venir
        if (atras < adelante) {
          // el golpe está atrasado
          error = - precision(atras);
        } else {
          // el golpe está adelantado
          error =   precision(adelante);          
        }
        
        render();
        break;
      }
      case LEFT: {
        apretado = true;
        //render();
        break;
      }
      case UP: {
        apretado = true;
        bpm += 1;
        intervalo = MILLIS_IN_MIN / bpm;
        render();
        break;
      }
      case DOWN: {
        apretado = true;
        bpm -= 1;
        intervalo = MILLIS_IN_MIN / bpm;
        render();
        break;
      }
      case SELECT: {
        lcd.print("SELECT");
        apretado = true;
        on = !on;
        render();
        break;
      }
    }
  } else {
    if (button == NONE) {
      apretado = false;
      //beat, compass = 0;
      //render();
    }
  }
}
