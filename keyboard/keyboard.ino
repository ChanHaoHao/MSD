#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include "DHT.h"

// DHT setting
#define DHTTYPE DHT11   // DHT 11
#define DHTPin 0      // the pin for DHT11 data D3
DHT dht(DHTPin, DHTTYPE); 


// WiFi settings
#define SSID_AP            "ESP8266_WiFi_Car" // AP server name
#define PASSWORD_AP        "12345678"         // AP server password

// motor settings
#define RIGHT_MOTOR_PIN1  4                   // pin 1 of right motor (D2)
#define RIGHT_MOTOR_PIN2  5                   // pin 2 of right motor (D1)
#define LEFT_MOTOR_PIN1   12                  // pin 1 of left motor (D6)
#define LEFT_MOTOR_PIN2   14                  // pin 2 of left motor (D5)
// #define LED_PIN           2                   // built-in LED (D4)

IPAddress local_ip(192, 168, 4, 1);           //IP for AP mode
IPAddress gateway(192, 168, 100, 1);          //IP for AP mode
IPAddress subnet(255, 255, 255, 0);           //IP for AP mode
ESP8266WebServer server(80);

Servo servo;

int car_mode = 0;                             // set car drive mode (0 = stop)
int pos;
int current_pos = 80;
float Temperature = 0;
float Humidity = 0;
int RIGHT_MOTOR_SPEED = 100;                // speed for right motor (0-1023)
int LEFT_MOTOR_SPEED = 100;                // speed for left motor (0-1023)

// initialize
void setup() {
  
  Serial.begin(115200);
  Serial.println("ESP8266 Wifi Car");
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
//  pinMode(LED_PIN, OUTPUT);
  servo.attach(2);
  servo.write(current_pos);
  delay(2);
  servo.detach();
  car_control(); // stop the car

  pinMode(DHTPin, INPUT);
  dht.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP(SSID_AP, PASSWORD_AP);

  // setup web server to handle specific HTTP requests
  server.on("/", HTTP_GET, handle_OnConnect);
  server.on("/forward", HTTP_GET, handle_forward);
  server.on("/backward", HTTP_GET, handle_backward);
  server.on("/left", HTTP_GET, handle_left);
  server.on("/right", HTTP_GET, handle_right);
  server.on("/stop", HTTP_GET, handle_stop);
  server.on("/up", HTTP_GET, handle_arm_up);
  server.on("/down", HTTP_GET, handle_arm_down);
  server.on("/119", HTTP_GET, handle_forward);
  server.on("/115", HTTP_GET, handle_backward);
  server.on("/97", HTTP_GET, handle_left);
  server.on("/100", HTTP_GET, handle_right);
  server.on("/32", HTTP_GET, handle_stop);
  server.on("/117", HTTP_GET, handle_arm_up);
  server.on("/106", HTTP_GET, handle_arm_down);
  server.onNotFound(handle_NotFound);

  //start server
  server.begin();
  Serial.println("ESP8266 car server started.");
//  digitalWrite(LED_PIN, LOW);
}

// handle HTTP requests and control car
void loop() { 
  server.handleClient();
  car_control();
}

// HTTP request: on connect
void handle_OnConnect() {
  car_mode = 0;
  Serial.println("Client connected");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: stop car
void handle_stop() {
  car_mode = 0;
  Serial.println("Stopped");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: go forward
void handle_forward() {
  car_mode = 1;
  Serial.println("Go forward...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: go backward
void handle_backward() {
  car_mode = 2;
  Serial.println("Go backward...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: turn left
void handle_left() {
  car_mode = 3;
  Serial.println("Turn left...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: turn right
void handle_right() {
  car_mode = 4;
  Serial.println("Turn right...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: other
void handle_NotFound() {
  car_mode = 0;
  Serial.println("Page error");
  server.send(404, "text/plain", "Not found");
}

// HTTP request: arm up
void handle_arm_up() {
  car_mode = 5;
  Serial.println("arm up...");
  server.send(200, "text/html", SendHTML());
}

// HTTP request: arm down
void handle_arm_down() {
  car_mode = 6;
  Serial.println("arm down...");
  server.send(200, "text/html", SendHTML());
}

// control car movement
void car_control() {
  switch (car_mode) {
    case 0: // stop car
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
      digitalWrite(LEFT_MOTOR_PIN1, LOW);
      digitalWrite(LEFT_MOTOR_PIN2, LOW);
      break;
    case 1: // go forward
      analogWrite(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_SPEED);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
      analogWrite(LEFT_MOTOR_PIN1, LEFT_MOTOR_SPEED);
      digitalWrite(LEFT_MOTOR_PIN2, LOW);
      break;
    case 2: // go backward
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      analogWrite(RIGHT_MOTOR_PIN2, 98);
      digitalWrite(LEFT_MOTOR_PIN1, LOW);
      analogWrite(LEFT_MOTOR_PIN2, 100);
      break;
    case 3: // turn left
      analogWrite(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_SPEED/1.3);
      digitalWrite(RIGHT_MOTOR_PIN2, LOW);
      digitalWrite(LEFT_MOTOR_PIN1, LOW);
      analogWrite(LEFT_MOTOR_PIN2, LEFT_MOTOR_SPEED/1.3);
      break;
    case 4: // turn right
      digitalWrite(RIGHT_MOTOR_PIN1, LOW);
      analogWrite(RIGHT_MOTOR_PIN2, RIGHT_MOTOR_SPEED/1.3);
      analogWrite(LEFT_MOTOR_PIN1, LEFT_MOTOR_SPEED/1.3);
      digitalWrite(LEFT_MOTOR_PIN2, LOW);
      break;
    case 5:
      pos = 180;
      servo.attach(2);
      while (current_pos!=pos) {
        if (current_pos > pos) {
          current_pos-=1;
          servo.write(current_pos);
          delay(10);
        }
        else {
          current_pos+=1;
          servo.write(current_pos);
          delay(10);
        }
      }
      Serial.println("done");
      servo.detach();
      car_mode = 0;
      break;
    case 6:
      pos = 80;
      servo.attach(2);
      while (current_pos!=pos) {
        if (current_pos > pos) {
          current_pos-=1;
          servo.write(current_pos);
          delay(10);
        }
        else {
          current_pos+=1;
          servo.write(current_pos);
          delay(10);
        }
      }
      Serial.println("done");
      servo.detach();
      car_mode = 0;
      break;
  }
  
  Temperature = dht.readTemperature(); // Gets the values of the temperature
  Humidity = dht.readHumidity(); // Gets the values of the humidity 
}

// output HTML web page for user
String SendHTML() {
  String html = "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";
  html += "<title>ESP8266 WiFi Car</title>";
  html += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<style>body {background-color: Moccasin;} h1 {color: SaddleBrown;} h2 {color: Olive;}</style>";
  html += "</head>";
  html += "<body>";
  html += "<div align=\"center\">";
  html += "<h1>ESP8266 WiFi Car</h1>";
  html += "<h2>Press \"stop\" after the server has been reset</h2>";
  html += "<br>\n";
  html +="<p>Press a key on the keyboard in the input field.</p>";
  html +="<input type=\"text\" size=\"40\" onkeypress=\"myFunction(event)\">";
  html +="<p id=\"demo\"></p>";
  html +="<script>function myFunction(event) {let unicode = event.which;window.location.href= unicode;document.getElementById(\"demo\").innerHTML = \"The pressed key was: \" + unicode;}</script>";
  html += "<form method=\"GET\">";
  html += "<input type=\"button\" value=\"Go forward\" onclick=\"window.location.href='/forward'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Go backward\" onclick=\"window.location.href='/backward'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Turn left\" onclick=\"window.location.href='/left'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Turn right\" onclick=\"window.location.href='/right'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Car stop\" onclick=\"window.location.href='/stop'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Arm up\" onclick=\"window.location.href='/up'\">";
  html += "<br><br>\n";
  html += "<input type=\"button\" value=\"Arm down\" onclick=\"window.location.href='/down'\">";
  html += "</form>\n";
  html += "<p>Temperature: ";
  html += (int)Temperature;
  html += " degrees</p>";
  html += "<p>Humidity: ";
  html += (int)Humidity;
  html += "%</p>";
  html += "</div>\n";
  html += "</body>\n";
  html += "</html>\n";
  return html;
}
