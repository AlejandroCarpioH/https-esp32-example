/**
 * Example for the ESP32 HTTP(S) Webserver
 *
 * IMPORTANT NOTE:
 * To run this script, your need to
 *  1) Enter your WiFi SSID and PSK below this comment
 *  2) Make sure to have certificate data available. You will find a
 *     shell script and instructions to do so in the library folder
 *     under extras/
 *
 * This script will install an HTTPS Server on your ESP32 with the following
 * functionalities:
 *  - Show simple page on web server root
 *  - 404 for everything else
 */

// TODO: Configure your WiFi here
#define WIFI_SSID "wifi 5"
#define WIFI_PSK  ""
// #define WIFI_SSID "jesus-hogar"
// #define WIFI_PSK  "luz147223242000022940000"

// Include certificate data (see note above)
#include "cert.h"
#include "private_key.h"

// Binary data for the favicon
#include "favicon.h"

// We will use wifi
#include <WiFi.h>

// Includes for the server
#include <HTTPSServer.hpp>
#include <SSLCert.hpp>
#include <HTTPRequest.hpp>
#include <HTTPResponse.hpp>
#include <ESPmDNS.h>
// The HTTPS Server comes in a separate namespace. For easier use, include it here.

const char *host = "esp32";

using namespace httpsserver;

int port = 23;
int valor = 0;


// const char* example_crt_PEM = 
// "-----BEGIN CERTIFICATE-----\n"
// "MIID/TCCAuWgAwIBAgIUFlcykxLHmfC3R+f3f+jyqpHLjB4wDQYJKoZIhvcNAQEL\n"
// "BQAwgY0xCzAJBgNVBAYTAkNIMREwDwYDVQQIDAhUQVJBUEFDQTEQMA4GA1UEBwwH\n"
// "SVFVSVFVRTEMMAoGA1UECgwDQUxFMREwDwYDVQQLDAhJTkdFTklFUjESMBAGA1UE\n"
// "AwwJQUxFSkFORFJPMSQwIgYJKoZIhvcNAQkBFhVEQVZJRDI0MjQ1NUBHTUFJTC5D\n"
// "T00wHhcNMjQwOTI5MDM1MzQ1WhcNMjUwOTI5MDM1MzQ1WjCBjTELMAkGA1UEBhMC\n"
// "Q0gxETAPBgNVBAgMCFRBUkFQQUNBMRAwDgYDVQQHDAdJUVVJUVVFMQwwCgYDVQQK\n"
// "DANBTEUxETAPBgNVBAsMCElOR0VOSUVSMRIwEAYDVQQDDAlBTEVKQU5EUk8xJDAi\n"
// "BgkqhkiG9w0BCQEWFURBVklEMjQyNDU1QEdNQUlMLkNPTTCCASIwDQYJKoZIhvcN\n"
// "AQEBBQADggEPADCCAQoCggEBAMtPDugOBYSgqR8naAODOpCSevoZN2Xk7PMCUfSF\n"
// "Au8M3lt1OdgCz1YsqehmUCqn1BaRikykr5/19F+hmiRX3VfEeNQ7MVgee4xNrWRg\n"
// "nQXUl5uVLVzUEWqEOCwOUUV6cuZ8Upv+HRuusED86BX0cm0ZikEuL50eeYr1LkCk\n"
// "jKpinNMncs5c0QhIivcCu/I7JPTBHYs3ryvOxC+L/XgGk9/cyupXFGz2BHEv8lji\n"
// "nJjLG0C52dnYGXHYhgQw6TO0uexefxZbCaxFo4dW2lmwobPAoGXJGAaovupmAzE9\n"
// "INfGwQru5CaZuuxKzpQiwFLkx1OnxLEMGSGCSY3hWfc0oAUCAwEAAaNTMFEwHQYD\n"
// "VR0OBBYEFE3eqREc0IWhKW2b8qBjLKjnn5bIMB8GA1UdIwQYMBaAFE3eqREc0IWh\n"
// "KW2b8qBjLKjnn5bIMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEB\n"
// "AC1OCj0lS4meJ6Kh2pogahiig/EqCYnq1XG0tBeRj8XItyxAhrcM8Ro/A4KwxvSc\n"
// "JRCHqHsjc/WEHngaSngxA+hx4X5LyFcjT7TQmQ/Yyn+D0fGIlQOcOC6DZCUlip52\n"
// "DpbpGJhW1P4GlLoxP1pk+R66p7VtmW5kmKdzsf6jUYfXjWAkgDq3V8lOKX34UlcG\n"
// "LW/0BSxGbp8GJBnxnq3drO/i2+I+bfisjwMVAMAa/u/XBqUdBXruLAQknoRSUJF8\n"
// "aSEUwxPCDtGIcv9yRQTcEM6bINv/7dz7lj+yVBSAHpWvcn20HOdXFUO+6Je0xg6y\n"
// "F6JxOd2qi+sJhRHFTax3j00=\n"
// "-----END CERTIFICATE-----";

// const char* example_key_PEM = 
// "-----BEGIN PRIVATE KEY-----\n"
// "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDLTw7oDgWEoKkf\n"
// "J2gDgzqQknr6GTdl5OzzAlH0hQLvDN5bdTnYAs9WLKnoZlAqp9QWkYpMpK+f9fRf\n"
// "oZokV91XxHjUOzFYHnuMTa1kYJ0F1JeblS1c1BFqhDgsDlFFenLmfFKb/h0brrBA\n"
// "/OgV9HJtGYpBLi+dHnmK9S5ApIyqYpzTJ3LOXNEISIr3ArvyOyT0wR2LN68rzsQv\n"
// "i/14BpPf3MrqVxRs9gRxL/JY4pyYyxtAudnZ2Blx2IYEMOkztLnsXn8WWwmsRaOH\n"
// "VtpZsKGzwKBlyRgGqL7qZgMxPSDXxsEK7uQmmbrsSs6UIsBS5MdTp8SxDBkhgkmN\n"
// "4Vn3NKAFAgMBAAECggEADtnbTaWqjwXaXMF45lrJZiKhE5BQfOO0UEsA2BL4RCSF\n"
// "QJqHS2lnIlUYYzbID9mbEzv15YPZJV7mxy6LjzCQ5VafeedKAzUU0sqgyhaJmdGt\n"
// "8mo6+k8uZ4C5hrRUCPZ5Gq+FrEnrb+h6jQC8ZlApJKGLORgp17kQaU9gHc43TUHN\n"
// "zv8pPnSsTVLk6A8H3MPzf/RqwdrkMBkUhC9vgDynKTuiNUFtd5LlUCDI6Hgs6LRZ\n"
// "ZHve0sIZ6K6CPgRUMfv9unDnla3FdaqOHkWF4j46ge509NjNfBQp6QV9KgNbMf2z\n"
// "x1MUGOJ7BEgif3fCPlzHRsNqttk6u0V2taN9f5HUmQKBgQD9Dzl0kcSGqb+SoOuP\n"
// "aHwle4lDA5+/MIbw0eo72xSUJvblySnLQsNrtC9qvZcmaqttFFq8WKHsLunxWKIs\n"
// "ttvvHCpMglwp2PRO5sNXSXDEAPfCIaC98FlmwJMGaQVMBlJvHH9S6CzL2Aen4AuZ\n"
// "Z7ZLsRCxMraYH9QietH8JHhAXQKBgQDNq9czIYcEfZEnIThYpx0xqklJ5BWn3t7t\n"
// "df4SUy5gyUJUlAV5j1qJEZhKeH2/lOBR+jKUoKwstimU3orjWPfgdgmMsWotuqNV\n"
// "ThHMk/Cg+12yOfTq56GhVaOm3EUwY7p3H0DLQNPn8ITwC9EB/MmDllxVqx9lXBjB\n"
// "vxs7D50DyQKBgQCa8H1fL/jB8qDG6BwSHAkW8gJis49VgiK8w+nEaCi7ZPf6TSZ3\n"
// "uxLnmNUHZ7lp3hnDmZjMhP/vGfJGwNvawML8LaSAX7iKLrm2rXegs+iFOYp4pBpE\n"
// "8SnrzbC3VgnWOHC0cyBJ5Lm4vTZCxNyVOnjU8z6jH8fIcIRz7/sthC7USQKBgCWB\n"
// "U6H4WDXCbCjVjv/joZg/9FNsK2LMapyk6qsOziNKMDVaNNFhCeJ6+emCpXO9Q1bP\n"
// "qE+gKSWSJm0GVA93VuRwwfcNF061fTl+HhLPW48Pp5trpDT9/mR8SHaSLm/Pj95P\n"
// "LbKi5MGkA6n+jaOuJFVBox7gML58F+uk3ONdSoW5AoGBAJF+sW8gTTkZjNNc9Xz2\n"
// "1034ghjWdiEhs4hi46VWnno5HMivx57g0AK3Cvr0VGbCWOzwsZDWFTSg3o0zq4G/\n"
// "4tbVRsHTbZNLjWQzW1Pa4qlSVH9b3jFoWi1ThAw9rB06dJSl1MiSzG5wrwBcre8p\n"
// "Dkh9sNxQ50BcSLZFoTu3NlVu\n"
// "-----END PRIVATE KEY-----";

// SSLCert cert = SSLCert(
//   example_crt_DER, example_crt_DER_len,
//   example_key_DER, example_key_DER_len
// );
SSLCert cert = SSLCert(example_crt_DER, example_crt_DER_len,example_key_DER, example_key_DER_len);

// Create an SSL certificate object from the files included above
// SSLCert cert = SSLCert(
//   (unsigned char*)example_crt_PEM, strlen(example_crt_PEM),
//   (unsigned char*)example_key_PEM, strlen(example_key_PEM)
// );

// Create an SSL-enabled server that uses the certificate
// The contstructor takes some more parameters, but we go for default values here.
HTTPSServer secureServer = HTTPSServer(&cert);


// Declare some handler functions for the various URLs on the server
// The signature is always the same for those functions. They get two parameters,
// which are pointers to the request data (read request body, headers, ...) and
// to the response data (write response, set status code, ...)
void handleRoot(HTTPRequest * req, HTTPResponse * res);
void handleFavicon(HTTPRequest * req, HTTPResponse * res);
void handle404(HTTPRequest * req, HTTPResponse * res);

void setup() {
  // For logging
  Serial.begin(115200);
 pinMode(port,INPUT);
  // Connect to WiFi
  Serial.println("Setting up WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("Connected. IP=");
  Serial.println(WiFi.localIP());
if (MDNS.begin(host)) {
    Serial.println("mDNS responder started");
  }
   MDNS.addService("https", "tcp", 443);
    Serial.printf("Open https://%s.local in your browser\n", host);
  // For every resource available on the server, we need to create a ResourceNode
  // The ResourceNode links URL and HTTP method to a handler function
  ResourceNode * nodeRoot    = new ResourceNode("/", "GET", &handleRoot);
  ResourceNode * getRut = new ResourceNode("/getrut","GET",&getrut);
  // ResourceNode * nodeFavicon = new ResourceNode("/favicon.ico", "GET", &handleFavicon);
  ResourceNode * node404     = new ResourceNode("", "GET", &handle404);

  // Add the root node to the server
  secureServer.registerNode(nodeRoot);
  secureServer.registerNode(getRut);
  // Add the favicon
  // secureServer.registerNode(nodeFavicon);

  // Add the 404 not found node to the server.
  // The path is ignored for the default node.
  secureServer.setDefaultNode(node404);

  Serial.println("Starting server...");
  secureServer.start();
  if (secureServer.isRunning()) {
    Serial.println("Server ready.");
  }
}

void loop() {
  // This call will let the server do its work
  secureServer.loop();

  // Other code would go here...
  delay(10);
}
void getrut(HTTPRequest * req, HTTPResponse * res){
  res->setHeader("Access-Control-Allow-Methods", "*");
  res->setHeader("Access-Control-Allow-Origin", "*");
  // res->setHeader("Content-Type", "text/plain");
  res->setHeader("Content-Type", "text/plain");

  res->print("14722324-2");

}
void handleRoot(HTTPRequest * req, HTTPResponse * res) {
  // Status code is 200 OK by default.
  // We want to deliver a simple HTML page, so we send a corresponding content type:
  ResourceParameters * val = req->getParams();
  
  // Serial.print(val->getPathParameter("rut"));
  res->setHeader("Access-Control-Allow-Methods", "*");
  res->setHeader("Access-Control-Allow-Origin", "*");
  // res->setHeader("Content-Type", "text/plain");
  res->setHeader("Content-Type", "application/json");
  while(valor < 1){
  valor = digitalRead(port);
  Serial.println(valor);
  }
  Serial.println(valor);
  res->print("{ \"valor\":\"hola clinte\"}");
  valor = 0;
  
  // The response implements the Print interface, so you can use it just like
  // you would write to Serial etc.
  // res->println("<!DOCTYPE html>");
  // res->println("<html>");
  // res->println("<head><title>Hello World!</title></head>");
  // res->println("<body>");
  // res->println("<h1>Hello World!</h1>");
  // res->print("<p>Your server is running for ");
  // // A bit of dynamic data: Show the uptime
  // res->print((int)(millis()/1000), DEC);
  // res->println(" seconds.</p>");
  // res->println("</body>");
  // res->println("</html>");
}

void handleFavicon(HTTPRequest * req, HTTPResponse * res) {
  // Set Content-Type
  res->setHeader("Content-Type", "image/vnd.microsoft.icon");
  // Write data from header file
  // res->write(FAVICON_DATA, FAVICON_LENGTH);
}

void handle404(HTTPRequest * req, HTTPResponse * res) {
  // Discard request body, if we received any
  // We do this, as this is the default node and may also server POST/PUT requests
  req->discardRequestBody();

  // Set the response status
  res->setStatusCode(404);
  res->setStatusText("Not Found");

  // Set content type of the response
  
  res->setHeader("Content-Type", "text/html");

  // Write a tiny HTTP page
  res->println("<!DOCTYPE html>");
  res->println("<html>");
  res->println("<head><title>Not Found</title></head>");
  res->println("<body><h1>404 Not Found</h1><p>The requested resource was not found on this server.</p></body>");
  res->println("</html>");
}
