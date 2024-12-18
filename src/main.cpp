#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>

const char* ssid = "CASA";
const char* password = "sonyevelma";

AsyncWebServer server(80);

const String validUser = "Arcruz";
const String validPassword = "4978";

String checkCredentials(String username, String password) {
    if (username == validUser && password == validPassword) 
    {
        return "Login bem-sucedido!";
    } 
    else 
    {
        return "Usuário ou senha incorretos!";
    }
}

void initSPIFFS() {
    if (!SPIFFS.begin(true)) {
        Serial.println("error SPIFFS");
        return;
  }
    Serial.println("success SPIFFS");
}

void setup() 
{
    Serial.begin(115200);
    
    initSPIFFS();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao Wi-Fi...");
    }

    Serial.println("Conectado ao Wi-Fi!");
    Serial.println("IP: " + WiFi.localIP().toString());


    server.on("/login", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = "<!DOCTYPE html><html lang='pt-BR'>";
        html += "<head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
        html += "<title>ESP32 WEB SERVER</title>";
        html += "<style>";
        html += "body { background-color: rgb(214, 212, 212); font-family: Arial, sans-serif; margin: 0; height: 100vh; display: flex; justify-content: center; align-items: center; }";
        html += ".container { display: flex; flex-direction: column; justify-content: center; align-items: center; background-color: rgb(49, 49, 49); padding: 20px; border-radius: 10px; box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.2); color: white; width: 200px; }";
        html += ".inputContainer { margin-bottom: 15px; width: 100%; text-align: center; }";
        html += ".inputContainer label { display: block; margin-bottom: 5px; font-size: 14px; }";
        html += ".inputContainer input { width: 90%; padding: 10px; font-size: 16px; border: 1px solid #ccc; border-radius: 5px; display: block; margin: 0 auto; }";
        html += ".inputContainer input:focus { outline: none; }";
        html += "button { padding: 10px 20px; font-size: 16px; border: none; background-color: #2ac444; color: white; border-radius: 5px; cursor: pointer; }";
        html += "button:hover { background-color: #137a32; }";
        html += "</style></head><body>";
        html += "<div class='container'><div class='inputContainer'>";
        html += "<label for='userInput'>User</label><input id='userInput' name='userInput' type='text' placeholder=''></div>";
        html += "<div class='inputContainer'><label for='passwordInput'>Password</label><input id='passwordInput' name='passwordInput' type='password' placeholder=''></div>";
        html += "<button id='submitBtn'>Login</button></div>";
        html += "<script>";
        html += "const submitBtn = document.getElementById('submitBtn');";
        html += "submitBtn.addEventListener('click', async (event) => {";
        html += "event.preventDefault();";
        html += "const userInput = document.getElementById('userInput').value;";
        html += "const passwordInput = document.getElementById('passwordInput').value;";
        html += "try {";
        html += "const response = await fetch('/login', {";
        html += "method: 'POST',";
        html += "headers: {'Content-Type': 'application/x-www-form-urlencoded',},";
        html += "body: `userInput=${encodeURIComponent(userInput)}&passwordInput=${encodeURIComponent(passwordInput)}`";
        html += "});";
        html += "const result = await response.text();";
        html += "alert(result);";
        html += "} catch (error) { console.error('Erro ao fazer a requisição:', error); }";
        html += "});";
        html += "</script></body></html>";
        request->send(200, "text/html", html);
    });

    server.on("/login", HTTP_POST, [](AsyncWebServerRequest *request)
    {
        String userInput = request->arg("userInput");
        String passwordInput = request->arg("passwordInput");

        Serial.print("Usuário: ");
        Serial.println(userInput);
        Serial.print("Senha: ");
        Serial.println(passwordInput);

        if (userInput == validUser && passwordInput == validPassword) {
            AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Login bem-sucedido!");

            // cria cookie de acesso
            String cookie = "access_token=valid_user; Max-Age=3600; Path=/; HttpOnly";
            response->addHeader("Set-Cookie", cookie);

            request->send(response);
        } else {
            request->send(401, "text/plain", "Credenciais inválidas.");
        }
    });



   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(SPIFFS, "/index.html");
    });

    server.serveStatic("/", SPIFFS, "/");


    server.begin();
}

void loop() 
{

}
