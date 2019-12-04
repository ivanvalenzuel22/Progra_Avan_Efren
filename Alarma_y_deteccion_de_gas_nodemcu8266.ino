
#include <ESP8266WiFi.h>        //Biblioteca del Nodemcu 8266
const char* ssid = "Rebaño";      // SSID de la red wifi
const char* password = "contrasena";  // Contraseña de la red wifi
char server[] = "mail.smtp2go.com";   //  SMTP servidor


int sensorPin = A0;                   // Pin de entrada del sensor de gas mq2
boolean canTrigger = true;
int buzzer = 10; 


WiFiClient espClient;
void setup()
{
 // pinMode(LED, OUTPUT);
  pinMode (5, OUTPUT); //LED ROJO
  pinMode (4, OUTPUT); //LED VERDE
  Serial.begin(115200);
  delay(10);
  Serial.println("");
  Serial.println("");
  Serial.print("Connecting To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print("*");
  }
  Serial.println("");
  Serial.println("WiFi Connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 }

void loop()
{
  
  int sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);
  digitalWrite(4, HIGH); //LED VERDE PRENDIDO LECTURA 
  
  if((sensorValue > 1000) && (canTrigger == true))
  {
    Serial.println("Sensor registra un valor alto");
    byte ret = sendEmail();             // aqui es donde se envia el email
    canTrigger = false;
    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);
     tone(buzzer, 1000, 200);
     
  }
  if((sensorValue < 1000) && (canTrigger == false))
  {
    Serial.println("Sin excedentes de gas");
    canTrigger = true;
    noTone(buzzer);
    digitalWrite(4, HIGH);   //LED VERDE SE PRENDE CUANDO 
    digitalWrite(5, LOW);    //LED ROJO SE APAGA
  }
  delay(500);
}

byte sendEmail()
{
  if (espClient.connect(server, 2525) == 1) 
  {
    Serial.println(F("connected"));
  } 
  else 
  {
    Serial.println(F("connection failed"));
    return 0;
  }
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending EHLO"));
  espClient.println("EHLO www.example.com");
  if (!emailResp()) 
    return 0;

  Serial.println(F("Sending auth login"));
  espClient.println("AUTH LOGIN");
  if (!emailResp()) 
    return 0;
  //  
  Serial.println(F("Sending User"));
  // Change this to your base64, ASCII encoded username
  
  espClient.println("aXZhbnZhbGVuenVlbDIyQGdtYWlsLmNvbQ=="); //base64, ASCII usuario encriptado
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending Password"));
  /
  
  espClient.println("TVZ1OFVHSHVmNDNC");//base64, ASCII contraseña encriptada
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending From"));
  // change to sender email address
  espClient.println(F("MAIL From: ivanvalenzuel22@gmail.com"));
  if (!emailResp()) 
    return 0;
  // change to recipient address
  Serial.println(F("Sending To"));
  espClient.println(F("RCPT To: icambambia2@gmail.com"));
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending DATA"));
  espClient.println(F("DATA"));
  if (!emailResp()) 
    return 0;
  Serial.println(F("Sending email"));
  // change to recipient address
  espClient.println(F("To: icambambia2@gmail.com"));
  // change to your address
  espClient.println(F("From: ivanvalenzuel22@gmail.com"));
  espClient.println(F("Subject: Sistema de alarma\r\n"));
  espClient.println(F("Estimado usuario Efren: se ha detectado un nivel alto de gas en su domicilio"));
  //
  espClient.println(F("."));    // this needs to be here
  if (!emailResp()) 
    return 0;
  //
  Serial.println(F("Sending QUIT"));
  espClient.println(F("QUIT"));
  if (!emailResp()) 
    return 0;
  //
  espClient.stop();
  Serial.println(F("disconnected"));
  return 1;
}

byte emailResp()
{
  byte responseCode;
  byte readByte;
  int loopCount = 0;

  while (!espClient.available()) 
  {
    delay(1);
    loopCount++;
    //tiempo de espera de 20 s si no se envia el email
    if (loopCount > 20000) 
    {
      espClient.stop();
      Serial.println(F("\r\nTimeout"));
      return 0;
    }
  }

  responseCode = espClient.peek();
  while (espClient.available())
  {
    readByte = espClient.read();
    Serial.write(readByte);
  }

  if (responseCode >= '4')
  {
    
    return 0;
  }
  return 1;
}
