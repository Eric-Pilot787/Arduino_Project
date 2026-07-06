#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 2     // what pin we're connected to
#define TEMP_LIMIT  30.0
#define HUM_LIMIT   60.0
#define LED_TEMP  8
#define LED_HUM   9

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() 
{
    // Initialize Serial port
    Serial.begin(9600); 

    // Set LED pins as output
    pinMode(LED_TEMP, OUTPUT);
    pinMode(LED_HUM, OUTPUT);

    // Initialize DHT & OLED
    dht.begin();
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.println(F("SSD1306 allocation failed"));
      for(;;); // Don't proceed, loop forever
    }
    // Clear the buffer
    display.clearDisplay();

    Serial.println("Lab 7 start!");
}

void loop() 
{
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    char buf[17];
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(t) || isnan(h)) 
    {
        Serial.println("Failed to read from DHT");
    } 
    else 
    {
        // Print Temperature & Humidity data on Serial port
        Serial.print("Humidity: "); 
        Serial.print(h);
        Serial.print(" %\t");
        Serial.print("Temperature: "); 
        Serial.print(t);
        Serial.println(" *C");

        // Print Temperature & Humidity data on OLED
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.print("Temp: ");
        if(t<10.0) display.print(" ");
        display.print(t, 1);
        display.println(" C");
        display.print("Hum : ");
        if(h<10.0) display.print(" ");
        display.print(h, 1);
        display.println(" \%");
        
        // if value > limit, LED on; else LED off
        if(t >= TEMP_LIMIT)
          digitalWrite(LED_TEMP, HIGH);
        else
          digitalWrite(LED_TEMP, LOW);
        if(h >= HUM_LIMIT)
          digitalWrite(LED_HUM, HIGH);
        else
          digitalWrite(LED_HUM, LOW);
    }
    delay(2000);
}
