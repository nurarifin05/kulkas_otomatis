#include <PubSubClient.h>
#include <Wire.h>
#include <WiFi.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#define Password_Length 8
LiquidCrystal_I2C lcd(0x27, 16, 2);  

#define ssid "SUDO" //wifine boba
#define password "saibudin" //password e boba
#define mqtt_server "202.154.58.25"
#define mqtt_username "agung"
#define mqtt_password "skripsi"
#define clientID "espClient"

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);
long lastMsg = 100;

const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
    {'1','2','3', 'A'},
    {'4','5','6', 'B'},
    {'7','8','9', 'C'},
    {'*','0','#', 'D'}
};
String pad;
int pin_relay = 27;
int benar = 0;
char Data[Password_Length]; 
char Master[Password_Length] = "1472589"; 
//char Master[Password_Length] = {'1', '4','7','2','5','8','9'}; 
byte data_count = 0, master_count = 0;
byte rowPins[ROWS] = {19, 18, 5, 17};
byte colPins[COLS] = {16, 4, 2, 15};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int y_satu = 33;
int y_dua = 25;
int t_satu = 12;
int t_dua = 13;


void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient_distance_sensor")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);  

  pinMode(y_satu, INPUT_PULLUP);
  pinMode(y_dua, INPUT_PULLUP);
  pinMode(t_satu, INPUT_PULLUP);
  pinMode(t_dua, INPUT_PULLUP);
  pinMode(pin_relay, OUTPUT);
  digitalWrite(pin_relay, LOW);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to");
  lcd.setCursor(0, 1);
  lcd.print("Smart Kulkas");
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("A : Status");
  lcd.setCursor(0, 1);
  lcd.print("B : Buka");
//  keypad.addEventListener(keypadEvent);
}

void loop() {

    if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  char key = keypad.getKey();
  if(key){
    switch(key){
      case 'A':
        Serial.println("Lihat Status");
        lcd.clear();
        bacaSensor();
        delay(5000);
        break;
      case 'B':
        Serial.println("Buka kunci");
        delay(1000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enter Password:");
        benar = 0;
        while(data_count < Password_Length - 1){
          char key = keypad.getKey();
          if(key){
            Serial.print("Pass: ");
            Serial.println(key);
            pad = pad + String(key);
            Data[data_count] = key; 
            lcd.setCursor(data_count,1); 
            lcd.print(Data[data_count]); 
            data_count++;   
          }
        }
        Serial.println(pad);
            clearData();
            if(pad == "1472589"){
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Password benar");
              lcd.setCursor(0, 1);
              lcd.print("Pintu terbuka");
              digitalWrite(pin_relay, HIGH);
              delay(5000);
              digitalWrite(pin_relay, HIGH);
              delay(5000);
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Pintu tertutup");
              lcd.setCursor(0, 1);
              lcd.print("Kembali");
              delay(2000);
            }else{
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Password salah");
              delay(2000);
            } 
          delay(5000);          
          loop();
        
        break;
      default:
        Serial.print("Key: ");
        Serial.println(key);
        break;
    }
  }

    long now = millis();
  if (now - lastMsg > 100) {
    lastMsg = now; 
  }
}
void clearData(){
  while(data_count !=0){
    Data[data_count--] = 0; 
  }
  return;
}

void bacaSensor(){
  int ys = digitalRead(y_satu);
  int yd = digitalRead(y_dua);
  int ts = digitalRead(t_satu);
  int td = digitalRead(t_dua);

  client.publish("agung/yakult1", String(ys).c_str(), true);
  client.publish("agung/yakult2", String(yd).c_str(), true);
  client.publish("agung/telor1", String(ts).c_str(), true);
  client.publish("agung/telor2", String(td).c_str(), true);


  if(ys){
    Serial.print("Y1 : 0");
    lcd.setCursor(0, 0);
    lcd.print("Y1 : 0");
  }else{
    Serial.print("Y1 : 1");
    lcd.setCursor(0, 0);
    lcd.print("Y1 : 1");
  }
  if(yd){
    Serial.print("   Y2 : 0");
    lcd.setCursor(10, 0);
    lcd.print("Y2 : 0");
  }else{
    Serial.print("   Y2 : 1");
    lcd.setCursor(10, 0);
    lcd.print("Y2 : 1");
  }
  if(ts){
    Serial.print("   T1 : 0");
    lcd.setCursor(0, 1);
    lcd.print("T1 : 0");
  }else{
    Serial.print("   T1 : 1");
    lcd.setCursor(0, 1);
    lcd.print("T1 : 1");
  }
  if(td){
    Serial.println("   T2 : 0");
    lcd.setCursor(10, 1);
    lcd.print("T2 : 0");
  }else{
    Serial.println("   T2 : 1");
    lcd.setCursor(10, 1);
    lcd.print("T2 : 1");
  }
  
}
