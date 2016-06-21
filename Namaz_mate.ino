
/* 
 Muslim_testing_12122015_2 Протестировано - OK!
 
 При каждом изменении уровня входного сигнала из "HIGH" в "LOW" (от 1 до 8), поступающего от сенсора на цифровой вход D2,
 с цифровых выходов (D4, D5, D6, D7) циклически поступает сигнал "HIGH" для запитывания соответствующих светодиодов (LED1, LED2, LED3, LED4).
 При нечетном количестве сигналов, поступающих на D2, очередной светодиод сначала переходит в режим мигания, при следующем сигнале - горит постоянно.
 При снижении уровня напряжения питающей батареи с +9В до +7,5В (контролируется анологовым входом A0) загорается светодиод "Low BAT" в режиме мигания
 (цифровой выход D9). При снижении напряжения до 6,5В светодиод "Low BAT" горит постоянно и работа устройства блокируется.
 При включении устройства, при наличии достаточного напряжения питающей батареи, все светодиоды загораются на 1 сек, для индикации включения устройства и работоспособности светодиодов.
 По схеме:
 * вывод + светодиодов (LED1, LED2, LED3, LED4) соединяется с цифровыми выходами D4 - D7
 * вывод - от сенсора соединяется с цифровым входом D2
 * вывод + светодиода "Low BAT" подключен к цифровому выходу D9
 * аналоговый сигнал с делителя опорного напряжения +9В (контроль напряжения +6В и ниже) поступает на аналоговый вход A0
 
 Для программного устранения дребезга контактов используется Bounce2 library 
 
 Создано: 27 января 2016
 Автор: Шарапудин Шарапудинов
*/

#include <Bounce2.h>   // Bounce2 library

#define SENSOR_PIN 2   // цифровой вход от сенсора
#define LED_1 4        // цифровой выход на LED1
#define LED_2 5        // цифровой выход на LED2
#define LED_3 6        // цифровой выход на LED3
#define LED_4 7        // цифровой выход на LED4
#define LED_LOW 9      // цифровой выход на LED "LOW BAT"
#define LOWBAT_PIN A0  // аналоговый вход для мониторинга опорного напряжения

// Переменные:
int ledsMode = 0;                     // текущий режим индикации (LED1, LED2, LED3, LED4) от 1 до 8
int sensorState = 0;                  // текущее состояние сигнала от сенсора
int lastSensorState = 0;              // предыдущее состояние от сенсора
int ledState = LOW;                   // для установки попеременного состояния LED (режим "мигания")
int led = 0;                          // номер LED, переводимого в режим "мигания"
unsigned long previousMillis = 0ul;   // для хранения предыдущего значения времени
unsigned long currentMillis = 0ul;    // для хранения текущего значения времени
unsigned long delayBlink = 777ul;       // временной интервал режима "мигания" (milliseconds)
int mediumBatLevel = 720;             // значение уровня опорного напряжения - переход в режим мигания
int lowBatLevel = 670;                // пограничное значение уровня опорного напряжения
int batLevel = 0;                     // текущее значение уровня опорного напряжения
unsigned long previousMillisLowBat = 0ul;   // для хранения предыдущего значения времени
unsigned long currentMillisLowBat = 0ul;    // для хранения текущего значения времени
unsigned long intervalLowBat = 3777ul;      // временной интервал режима "мигания" LED "LOW BAT"
int ledStateLowBat = LOW;                   // для установки попеременного состояния LED "LOW BAT"
int firstOn = 1;                      // индикатор включения устройства

// создаем экземпляр объекта Bounce
Bounce debouncer = Bounce();

void setup() {
  
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  pinMode(LED_LOW, OUTPUT);
  
  // Установка начального значения LED1, LED2, LED3, LED4, LED "LOW BAT":
  digitalWrite(LED_1, LOW);
  digitalWrite(LED_2, LOW);
  digitalWrite(LED_3, LOW);
  digitalWrite(LED_4, LOW);
  digitalWrite(LED_LOW, LOW);
  
  // Установки для экземпляра объекта Bounce:
  debouncer.attach(SENSOR_PIN);
  debouncer.interval(30); // установка задержки в ms для устранения "дребезга" контактов
  
  //Serial.begin(9600);     // для отладки

}

void loop() {
  
  if (!firstOn) {
	debouncer.update();  // обновляем экземпляр объекта Bounce
  batLevel = analogRead(LOWBAT_PIN); // считываем значение опорного напряжения
  //Serial.println(batLevel);        // для отладки
  if (batLevel >= lowBatLevel) {
    //Serial.println("Battery >= lowBatLevel");    // для отладки
    //Serial.print(batLevel);                      // для отладки
    if ((batLevel >= lowBatLevel) && (batLevel <= mediumBatLevel)) {
      //Serial.println("Battery LOW Blinking ");    // для отладки
	    // Serial.print(batLevel);                     // для отладки
      currentMillisLowBat = millis();  // считываем текущее значение времени
      if (currentMillisLowBat - previousMillisLowBat > intervalLowBat) {
        previousMillisLowBat = currentMillisLowBat;  // сохранить новое значение для отсчета времени  
        // меняем состояние LED "LOW BAT" на инверсное (режим мигания):
        if (ledStateLowBat == LOW) {
			    //Serial.println("ledStateLowBat HIGH");    // для отладки
			    ledStateLowBat = HIGH;
        }
        else {
          //Serial.println("ledStateLowBat LOW");    // для отладки
		      ledStateLowBat = LOW;
        }
		    digitalWrite(LED_LOW, ledStateLowBat);
      }
    }
    else {
      digitalWrite(LED_LOW, LOW);
      //Serial.println("LED_LOW LOW");    // для отладки
    }
    sensorState = !debouncer.read(); // считываем инвертированное текущее значение сигнала от сенсора
    //Serial.print("sensorstate = ");  // для отладки
    //Serial.println(sensorState);     // для отладки
    if (sensorState && !lastSensorState) { // если текущее значение сигнала от сенсора изменилось
      if (sensorState){
        ledsMode++;         // увеличить текущее значение режима индикации (LED1, LED2, LED3, LED4)
        //Serial.print("ledsMode = "); // для отладки
        //Serial.println(ledsMode);    // для отладки
        if (ledsMode > 8) {
          //ledsMode = 1;  // для отладки
          while(1) {
            ;
          }
        }
        switch (ledsMode) {
          case 1:
            led = LED_1;
            digitalWrite(LED_1, HIGH);
            digitalWrite(LED_2, LOW);
            digitalWrite(LED_3, LOW);
            digitalWrite(LED_4, LOW);
            delay(777);
            break;
          case 2:
            led = 0;
            digitalWrite(LED_1, HIGH);
            digitalWrite(LED_2, LOW);
            digitalWrite(LED_3, LOW);
            digitalWrite(LED_4, LOW);
            delay(777);
            break;
          case 3:
            led = LED_2;
            digitalWrite(LED_1, HIGH);
            digitalWrite(LED_2, HIGH);
            digitalWrite(LED_3, LOW);
            digitalWrite(LED_4, LOW);
            delay(777);
            break;
          case 4:
            led = 0;
            digitalWrite(LED_1, HIGH);
            digitalWrite(LED_2, HIGH);
            digitalWrite(LED_3, LOW);
            digitalWrite(LED_4, LOW);
            delay(777);
            break;
          case 5:
            led = LED_3;
            digitalWrite(LED_1, HIGH);
            digitalWrite(LED_2, HIGH);
            digitalWrite(LED_3, HIGH);
            digitalWrite(LED_4, LOW);
            delay(777);
            break;
          case 6:
            led = 0;
            digitalWrite(LED_1, HIGH);
            digitalWrite(LED_2, HIGH);
            digitalWrite(LED_3, HIGH);
            digitalWrite(LED_4, LOW);
            delay(777);
            break;
          case 7:
            led = LED_4;
            digitalWrite(LED_1, HIGH);
            digitalWrite(LED_2, HIGH);
            digitalWrite(LED_3, HIGH);
            digitalWrite(LED_4, HIGH);
            delay(777);
            break;
          case 8:
            led = 0;
            digitalWrite(LED_1, HIGH);
            digitalWrite(LED_2, HIGH);
            digitalWrite(LED_3, HIGH);
            digitalWrite(LED_4, HIGH);
            delay(777);
        }
      }
    }
    if (led) {
      currentMillis = millis();  // считываем текущее значение времени
      if (currentMillis - previousMillis > delayBlink) {
        previousMillis = currentMillis;  // сохранить новое значение для отсчета времени  
        // меняем состояние LED на инверсное (режим мигания):
        if (ledState == LOW) {
          ledState = HIGH;
        }
        else {
          ledState = LOW;
        }
        digitalWrite(led, ledState);
      }  
    }  
    lastSensorState = sensorState;
  }
  else {
    digitalWrite(LED_LOW, HIGH);
    //Serial.println("Battery < lowBatLevel");    // для отладки
    //Serial.print(batLevel);                     // для отладки
    while (1) {
      ;
    } 
  }  
  }
  else{
	firstOn = 0;
	digitalWrite(LED_1, HIGH);
	digitalWrite(LED_2, HIGH);
	digitalWrite(LED_3, HIGH);
	digitalWrite(LED_4, HIGH);
	digitalWrite(LED_LOW, HIGH);
	delay(777);
	digitalWrite(LED_1, LOW);
	digitalWrite(LED_2, LOW);
	digitalWrite(LED_3, LOW);
	digitalWrite(LED_4, LOW);
	digitalWrite(LED_LOW, LOW);
  }
}
