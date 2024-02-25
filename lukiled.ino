#include <SPI.h>
#include <Ethernet.h>
#include <Adafruit_NeoPixel.h>
#include <power.h>

const int neopixelPin = 0; // Change this to your actual pin
const int numPixels = 8;    // Change this to match the number of LEDs in your strip

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, neopixelPin, NEO_GRB + NEO_KHZ800);

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(10, 0, 0, 99);

EthernetServer server(80);

// Eine Variable um den HTTP Request zu speichern
String header;

void setup() {
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(4);
  strip.begin();
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  // start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}


void color(int ired, int igreen, int iblue) {

  for (int i = 0; i < numPixels; i++) {
    strip.setPixelColor(i, strip.Color(ired, igreen, iblue));
  }
  strip.show();
}

void loop() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;
        if (c == '\n' && request.endsWith("\r\n\r\n")) {
          break;
        }
      }
    }

    Serial.println("Request:");
    Serial.println(request);

    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;

    int redIndex = request.indexOf("red=");
    int greenIndex = request.indexOf("green=");
    int blueIndex = request.indexOf("blue=");

    if (redIndex != -1) {redValue = request.substring(redIndex + 4).toInt();}
    if (greenIndex != -1) {greenValue = request.substring(greenIndex + 6).toInt();}
    if (blueIndex != -1) {blueValue = request.substring(blueIndex + 5).toInt();    }

    Serial.println("RGB Values:");
    Serial.println("Red:"+String(redValue));
    Serial.println("Green: "+String(greenValue));
    Serial.println("Blue: "+String(blueValue));
    color(redValue, greenValue, blueValue);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE html>");
    client.println("<html lang=\"en\">");
    client.println("<head>");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    client.println("<title>WS2812B Color Picker</title>");
    client.println("<style>");
    client.println("body {");
    client.println("  font-family: Arial, sans-serif;");
    client.println("  text-align: center;");
    client.println("  margin-top: 50px;");
    client.println("}");
    client.println("input[type=\"color\"] {");
    client.println("  width: 50px;");
    client.println("  height: 50px;");
    client.println("  padding: 0;");
    client.println("  border: none;");
    client.println("  margin-bottom: 20px;");
    client.println("}");
    client.println("button {");
    client.println("  font-size: 16px;");
    client.println("  padding: 10px 20px;");
    client.println("  background-color: #4CAF50;");
    client.println("  color: white;");
    client.println("  border: none;");
    client.println("  border-radius: 4px;");
    client.println("  cursor: pointer;");
    client.println("}");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h2>WS2812B Color Picker</h2>");
    client.println("<input type=\"color\" id=\"colorPicker\" value=\"#ff0000\">");
    client.println("<br>");
    client.println("<button onclick=\"sendColor()\">Set Color</button>");
    client.println("<script>");
    client.println("function sendColor() {");
    client.println("  var colorPicker = document.getElementById(\"colorPicker\");");
    client.println("  var selectedColor = colorPicker.value;");
    client.println("  var red = parseInt(selectedColor.substring(1, 3), 16);");
    client.println("  var green = parseInt(selectedColor.substring(3, 5), 16);");
    client.println("  var blue = parseInt(selectedColor.substring(5, 7), 16);");
    client.println("  var url = \"red=\" + red + \"&green=\" + green + \"&blue=\" + blue;");
    client.println("  var xhr = new XMLHttpRequest();");
    client.println("  xhr.open(\"GET\", url, true);");
    client.println("  xhr.send();");
    client.println("}");
    client.println("</script>");
    client.println("</body>");
    client.println("</html>");

    client.stop();
    Serial.println("Client disconnected");

    }
    // give the web browser time to receive the data
    delay(1);

}

