template<class T> inline Print &operator <<(Print &obj, T arg) {
  obj.print(arg);
  return obj;
}
#define EEPROM_SIZE 12

#include "CTBot.h"
#include <EEPROM.h>


CTBot miBot;
CTBotInlineKeyboard miTeclado;

#include "token.h"

const int Led=2;
const int Buzzer=18;
const int SensorPin = 4;
int SensorState = 0;

float tiempo = 0;
float espera = 60;

const int DireccionSonido = 0;
boolean Sonido = true;
const int DireccionActivo = 1;
boolean Activo = true;

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println("Iniciando Bot de Telegram");

  EEPROM.begin(EEPROM_SIZE);
  Serial.println("EEPROM Configurada");

  Activo = EEPROM.read(DireccionActivo);
  Serial.print("Alarma: ");
  Serial.println(Activo ? "Activo" : "Apagado");

  Sonido = EEPROM.read(DireccionSonido);
  Serial.print("Sonido: ");
  Serial.println(Sonido ? "Activo" : "Apagado");

  pinMode(Buzzer, OUTPUT);
  pinMode(Led, OUTPUT);
  pinMode(SensorPin, INPUT);

  miBot.wifiConnect(ssid, password);

  miBot.setTelegramToken(token);

  if (miBot.testConnection()) {
    Serial.println("\n Conectado");
  }
  else {
    Serial.println("\n Problemas Auxilio");
  }
  miTeclado.addButton("Alarma", "alarma", CTBotKeyboardButtonQuery);
  miTeclado.addButton("Sonido", "sonido", CTBotKeyboardButtonQuery);
  miTeclado.addButton("Estado", "estado", CTBotKeyboardButtonQuery);
  miTeclado.addRow();


  miBot.sendMessage(IDchat, "En Linea, Sistema Vigilancia:" + nombre);
  tiempo = -espera * 1000;

}

void loop() {
  SistemaAlarma();
  SistemaConfiguracion();
  digitalWrite(Led, Activo);
  Serial.println(SensorState);//to graph 
}

void SistemaConfiguracion() {
  TBMessage msg;

  if (miBot.getNewMessage(msg)) {
    if (msg.sender.id == IDchat) {
      if (msg.messageType == CTBotMessageText) {
        if (msg.text.equalsIgnoreCase("opciones")) {
          PedirEstado();
        }
        else {
          Serial.println("Enviar 'opciones'");
          miBot.sendMessage(msg.sender.id, "prueba 'opciones'");
        }
      } else if (msg.messageType == CTBotMessageQuery) {
        Serial << "Mensaje: " <<  msg.sender.firstName << "\n";
        if (msg.callbackQueryData.equals("alarma")) {
          Activo = !Activo;
          String Mensaje  = "Alarma: ";
          Mensaje += (Activo ? "Activo" : "Apagado");
          Serial.println(Mensaje);
          miBot.endQuery(msg.callbackQueryID, Mensaje);
          EEPROM.put(DireccionActivo, Activo);
          EEPROM.commit();
        } else if (msg.callbackQueryData.equals("sonido")) {
          Sonido = !Sonido;
          String Mensaje  = "Sonido: ";
          Mensaje += (Activo ? "Activo" : "Apagado");
          Serial.println(Mensaje);
          miBot.endQuery(msg.callbackQueryID, Mensaje);
          EEPROM.put(DireccionSonido, Sonido);
          EEPROM.commit();
        } else if (msg.callbackQueryData.equals("estado")) {
          PedirEstado();
        }
      }
    } else {
      Serial << "Hable con: " << msg.sender.firstName << " - " <<  msg.sender.lastName << "\n";
      Serial << "Usuario: " << msg.sender.username << " ID: " << msg.sender.id << "\n";
      miBot.sendMessage(msg.sender.id, "Discula, no te conosco :( ");
    }
  }
}

void PedirEstado() {
  Serial.println("Enviando 'opciones'");
  String Mensaje  = "Estado Actual\n";
  Mensaje += "Alarma: ";
  Mensaje += (Sonido ? "Activo" : "Apagado");
  Mensaje += " - Sonido: ";
  Mensaje += (Sonido ? "Activo" : "Apagado");
  Serial.println(Mensaje);
  miBot.sendMessage(IDchat, Mensaje);
  miBot.sendMessage(IDchat, "Cambiar", miTeclado);
}
void SistemaAlarma() {
  if (Activo) {
    SensorState = digitalRead(SensorPin);
      if (SensorState == HIGH ) {
        Serial.println("Enviando Alerta");
        digitalWrite(Led, LOW);
       digitalWrite(Buzzer, LOW);
        miBot.sendMessage(IDchat, "Alerta Puerta Abierta" + nombre);
        delay(1000);
        digitalWrite(Buzzer, HIGH);
        digitalWrite(Led, LOW);
        tiempo = millis();
      }
      if(SensorState==LOW){
        digitalWrite(Led, LOW);
       digitalWrite(Buzzer, LOW);
      }
  }
}
