/* *
 * This is the main Webserver route and response handler.
 * This file contains all of the functions and calls necessary to receive, react, and respond to Web requests.
 * The AsyncTCP package will run the server on Core 1 by default, conveniently leaving Core 0 for our loop.
 */

#include <WiFi.h>
//#include <FS.h>
//#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

bool btWS_Fellover = true;

// Replace with your network credentials
const char *ssid = "RaB1Alpha2";
const char *password = "1234567#";

// Since we are the "access point" on a network with no way out (no external internet access), there are no real "Gateway" or "DNS" services provided,.
IPAddress local_ip(192,168,128,1);
IPAddress gateway(192,168,128,1);
IPAddress subnet(255,255,255,0);

AsyncWebServer server(80);

#include "MainPage.h"

unsigned char ucButtonState;
unsigned char ucWorkingButtonState;

int iConvertData;

unsigned long ulWS_EncoderRightCount;
unsigned long ulWS_EncoderLeftCount;

double dWS_PIDInput;
String buttonState = "0";

String EncoderRight;
String EncoderLeft;
String DistanceMeas;
String SensorData;

void setupRoutes()
{

//  These path handler functions no longer exist, but are kept for reference.
//  server.on("/", handle_OnConnect);
//  server.on("/led1on", handle_led1on);
//  server.on("/led1off", handle_led1off);
//  server.on("/led2on", handle_led2on);
//  server.on("/led2off", handle_led2off);
//  server.onNotFound(handle_NotFound);

  server.on("/RaB1", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(200, "text/html", MAIN_page);
  });

  //Our Main Page, http://<ip>/
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(200, "text/html", MAIN_page);
  });

  //Our Event Handler for when any button on the page is pressed, http://<ip>/setButtonPressed?StateButton=<int:button>
  server.on("/setPressedButton", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    AsyncWebParameter* p;
     
    if(request->hasParam("StateButton"))
    {
      p = request->getParam("StateButton");
       // Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      String t_state = p->value().c_str();//server->arg("StateButton"); //Refer  xhttp.open("GET", "setButton?StateButton="+buttonPressed, true);
      
      buttonState = p->value().c_str();
      ucButtonState = buttonState.toInt();
      
      request->send(200, "text/plain", buttonState); //Send web page
    }else{
      request->send(400, "text/plain", "Missing Button State.");
    }
  });

  //A page that returns general diagnostic information, http://<ip>/readSensorData
  server.on("/readSensorData", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    iConvertData = ulWS_EncoderLeftCount;//analogRead(A0);
    SensorData = String(btWS_Fellover) + ";hi       " + String(ulWS_EncoderLeftCount) + ";     " + String(ulWS_EncoderRightCount) + ";     " + String(dWS_PIDInput);
      
    // Serial.println(SensorData);
    request->send(200, "text/plain", SensorData); //Send ADC value only to client ajax request
  });
  
//  server.on("/readADC", handleADC);

//  server.on("/readEncoderLeft", HTTP_GET, [](AsyncWebServerRequest *request){
//      iConvertData = ulEncoderLeftCount;//analogRead(A0);
//      EncoderLeft = String(iConvertData);
//      
//    
//    request->send(200, "text/plane", EncoderLeft); //Send ADC value only to client ajax request
//   
//    });

//   server.on("/readEncoderRight", HTTP_GET, [](AsyncWebServerRequest *request){
//      iConvertData = ulEncoderRightCount;//analogRead(A0);
//     
//      EncoderRight = String(iConvertData);
//    
//    request->send(200, "text/plane", EncoderRight); //Send ADC value only to client ajax request
//   
//    });

//    server.on("/readDistanceMeas", HTTP_GET, [](AsyncWebServerRequest *request){
//     
//      iConvertData = dPIDInput;
//      DistanceMeas = String(iConvertData);
//    
//    request->send(200, "text/plane", DistanceMeas); //Send ADC value only to client ajax request
//   
//    });
}

int loopWEBServerButtonresponce(void)
{
  switch(ucButtonState)
  {
    case 0:
    default:
    {     
      Serial.println("Stop");
      ucWorkingButtonState = 0;
      ucButtonState = 9;
      break;
    }
    case 1:
    {      
      Serial.println("Forward");
      ucButtonState = 9;
      ucWorkingButtonState = 1;
      break;
    }
    case 2:
    {
      Serial.println("Left");
      ucButtonState = 9;
      ucWorkingButtonState = 2;
      break;
    }
    case 3:
    {
      Serial.println("Right");
      ucButtonState = 9;
      ucWorkingButtonState = 3;
      break;
    }
    case 4:
    {   
      Serial.println("Reverse");
      ucButtonState = 9;
      ucWorkingButtonState = 4;
      break;
    }
    case 5:  //toggle servo
    {
      ucButtonState = 9;
      ucWorkingButtonState = 9;
      break;
    }
    case 9:
    {        
      ucButtonState = 9;
      ucWorkingButtonState = 9;
      break;
    }
  }//end of case statment

  return ucWorkingButtonState;
}//end of loopwebServerButtonResponse


void setupWebServer(void)
{
  Serial.println(F("Configuring access point..."));

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
//  delay(100); - this delay *seems* unnecessary, will be confirmed in future testing

  IPAddress myIP = WiFi.softAPIP();
  
  Serial.print(F("SSID:"));Serial.println(ssid);
  Serial.print(F(" PWD:"));Serial.println(password);
  Serial.print(F("SITE: http://"));Serial.print(myIP);Serial.println(F("/"));

  // Setup the possible webserver routes.
  setupRoutes();
  
  server.begin();
  Serial.println(F("HTTP server started"));
  Serial.println();
}
