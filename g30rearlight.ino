#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <WebServer.h>

#define LED_PIN 5    // GPIO pin connected to the NeoPixel ring
#define LED_COUNT 24 // Number of LEDs on the ring

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char *ssid = "NinebotSNSC2.1LightControl";
const char *password = ""; // Open network (no password)

WebServer server(80);

int red = 255, green = 0, blue = 0; // Default to red

void setColor(uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; i < strip.numPixels(); i++)
    {
        strip.setPixelColor(i, strip.Color(r, g, b));
    }
    strip.show();
}

void handleRoot()
{
    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>DsSoft LED Control</title>
      <style>
        body { font-family: Arial; text-align: center; margin-top: 40px; }
        input[type=color] { width: 100px; height: 100px; border: none; }
        button { padding: 12px 24px; font-size: 18px; margin-top: 20px; }
      </style>
    </head>
    <body>
      <h2>Control the G30 LED Ring</h2>
      <form action="/set" method="GET">
        <input type="color" name="color" value="#ff0000">
        <br>
        <button type="submit">Set Color</button>
      </form>
    </body>
    </html>
  )rawliteral";
    server.send(200, "text/html", html);
}

void handleSetColor()
{
    if (server.hasArg("color"))
    {
        String hexColor = server.arg("color");
        long number = strtol(hexColor.c_str() + 1, NULL, 16);
        red = (number >> 16) & 0xFF;
        green = (number >> 8) & 0xFF;
        blue = number & 0xFF;

        setColor(red, green, blue);
    }
    server.sendHeader("Location", "/");
    server.send(303);
}

void setup()
{
    Serial.begin(115200);
    strip.begin();
    strip.setBrightness(255);   // optional: reduce if it's too bright
    setColor(red, green, blue); // default red

    WiFi.softAP(ssid, password);
    Serial.println("Access Point started");
    Serial.print("IP address: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", handleRoot);
    server.on("/set", handleSetColor);
    server.begin();
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();
}