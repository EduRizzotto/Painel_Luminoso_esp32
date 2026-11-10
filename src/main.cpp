#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include "imagens.h"

int luzCima = 13;
int luzBaixo = 14;

WiFiServer server(80);

unsigned long tempoAnterior1 = 0;
bool estadoRele1 = false;

unsigned long tempoAnterior2 = 0;
int passo2 = 0;

unsigned long tempoAnterior3 = 0;
bool estadoRele3 = false;

unsigned long tempoAnterior4 = 0;
bool estadoRele4 = false;

unsigned long tempoAnterior5 = 0;
bool estadoRele5 = false;

void luzes1(){
    unsigned long agora = millis();
    if (agora - tempoAnterior1 >= 1000){
        tempoAnterior1 = agora;
        estadoRele1 = !estadoRele1;
        digitalWrite(luzBaixo, estadoRele1 ? LOW : HIGH);
        digitalWrite(luzCima, estadoRele1 ? HIGH : LOW);
    }
}
void luzes2() {
    unsigned long agora = millis();

    if (agora - tempoAnterior2 >= 1000) {
        tempoAnterior2 = agora;

        switch (passo2) {
            case 0:
                digitalWrite(luzCima, LOW);   
                digitalWrite(luzBaixo, HIGH); 
                passo2 = 1;                   
                break;

            case 1:
                digitalWrite(luzCima, LOW);   
                digitalWrite(luzBaixo, LOW);  
                passo2 = 2;                   
                break;

            case 2:
                digitalWrite(luzCima, HIGH);  
                digitalWrite(luzBaixo, HIGH); 
                passo2 = 0;                   
                break;
        }
    }
}
void luzes3(){
    unsigned long agora = millis();
    if (agora - tempoAnterior3 >= 500){
        tempoAnterior3 = agora;
        estadoRele3 = !estadoRele3;
        digitalWrite(luzBaixo, estadoRele3 ? LOW : HIGH);
        digitalWrite(luzCima, estadoRele3 ? LOW : HIGH);
    }
}
void luzes4(){
    unsigned long agora = millis();
    if (agora - tempoAnterior3 >= 2000){
        tempoAnterior3 = agora;
        estadoRele3 = !estadoRele3;
        digitalWrite(luzBaixo, estadoRele3 ? LOW : HIGH);
        digitalWrite(luzCima, estadoRele3 ? LOW : HIGH);
    }
}

int passo5 = 0;

void luzes5(){
    const int etapas[][3] = {
        {LOW,  HIGH, 120},
        {HIGH, HIGH, 120},
        {LOW,  HIGH, 120},
        {HIGH, HIGH, 120},

        {LOW,  HIGH, 400},

        {LOW,  LOW,  120},
        {LOW,  HIGH, 120},
        {LOW,  LOW,  120},
        {LOW,  HIGH, 120},

        {LOW,  LOW,  20000},

        {HIGH, LOW,  120},
        {LOW,  LOW,  120},
        {HIGH, LOW,  120},
        {LOW,  LOW,  120},
        {HIGH, LOW,  400}, 

        {HIGH, HIGH, 120},
        {HIGH, LOW,  120},
        {HIGH, HIGH, 120},
        {HIGH, LOW,  120},

        {HIGH, HIGH, 1000}
    };

    unsigned long agora = millis();
    if (agora - tempoAnterior5 >= etapas[passo5][2]){
        tempoAnterior5 = agora;
        passo5 = (passo5 + 1) % 20;
        digitalWrite(luzCima, etapas[passo5][0]);
        digitalWrite(luzBaixo, etapas[passo5][1]);
    }
}

void setup()
{
    Serial.begin(115200);
    SPI.begin();

    pinMode(luzCima, OUTPUT);
    pinMode(luzBaixo, OUTPUT);
    digitalWrite(luzCima, HIGH);
    digitalWrite(luzBaixo, HIGH);

    WiFi.mode(WIFI_STA);
    WiFi.begin("WIFI-NOME", "WIFI-SENHA");
    Serial.print("\nConectando ao Wi-Fi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\n==================================");
    Serial.println("  CONECTADO AO WI-FI COM SUCESSO! ");
    Serial.print("  IP DO ESP32: ");
    Serial.println(WiFi.localIP());
    Serial.print("  Abra no celular: http://");
    Serial.println(WiFi.localIP());
    Serial.println("==================================\n");

    server.begin();
    Serial.println("Servidor web iniciado!\n");
}

int modo = 1;
bool manualCima = false;
bool manualBaixo = false;

void loop()
{
    WiFiClient client = server.available();
    if (client)
    {
        String linhaAtual = "";
        String requisicaoCompleta = "";
        unsigned long tempoLimiteWeb = millis();

        while (client.connected() && (millis() - tempoLimiteWeb < 250))
        {
            if (client.available())
            {
                char c = client.read();
                requisicaoCompleta += c;

                if (c == '\n')
                {
                    if (linhaAtual.length() == 0)
                    {
                        if (requisicaoCompleta.indexOf("GET /mudarModo?modo=") >= 0)
                        {
                            int inicio = requisicaoCompleta.indexOf("modo=") + 5;
                            int fim = requisicaoCompleta.indexOf(" ", inicio);

                            String valorTexto = requisicaoCompleta.substring(inicio, fim);
                            modo = valorTexto.toInt();

                            digitalWrite(luzCima, HIGH);
                            digitalWrite(luzBaixo, HIGH);

                            Serial.print("valor recebido: ");
                            Serial.println(modo);

                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/plain");
                            client.println("Connection: close");
                            client.println();
                            client.print("Recebido com sucesso!");
                            break;
                        }
                        else if (requisicaoCompleta.indexOf("GET /manual?") >= 0)
                        {
                            int posCima = requisicaoCompleta.indexOf("c=") + 2;
                            int posBaixo = requisicaoCompleta.indexOf("b=") + 2;

                            manualCima = (requisicaoCompleta.charAt(posCima) == '1');
                            manualBaixo = (requisicaoCompleta.charAt(posBaixo) == '1');
                            modo = 0;

                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/plain");
                            client.println("Connection: close");
                            client.println();
                            client.print("OK");
                            break;
                        }
                        else if (requisicaoCompleta.indexOf("GET /logo.jpg") >= 0)
                        {
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: image/jpeg");
                            client.println("Connection: close");
                            client.println();
                            client.write(logo1_jpg, logo1_jpg_len);
                            break;
                        }
                        else if (requisicaoCompleta.indexOf("GET /image-removebg-preview.png") >= 0)
                        {
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: image/png");
                            client.println("Connection: close");
                            client.println();
                            client.write(logo2_png, logo2_png_len);
                            break;
                        }
                        else if (requisicaoCompleta.indexOf("GET /favicon.ico") >= 0)
                        {
                            client.println("HTTP/1.1 204 No Content");
                            client.println("Connection: close");
                            client.println();
                            break;
                        }
                        else
                        {
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-type: text/html");
                            client.println("Connection: close");
                            client.println();

                            client.println(R"rawhtml(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Luzes Rizzotto&Rizzotto</title>
    <style>

        h1 {
            color: white;
        }

        body {
            margin: 0;
            padding: 20px;
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            font-family: sans-serif;
            background-color: #2d2d2e;
            box-sizing: border-box;
        }

        .container {
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 16px;
            width: 100%;
            max-width: 320px;
        }

        .quadrado {
            border: 1px solid #fff;
            width: 100%;
            aspect-ratio: 1 / 1;
            background-color: #1a1a1a;
            border-radius: 12px;
            box-sizing: border-box;
            cursor: pointer;
            user-select: none;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 12px;
            overflow: hidden;
            transition: background-color 0.2s ease, box-shadow 0.2s ease, filter 0.1s ease, transform 0.1s ease;
        }

        .quadrado img {
            width: 90%;
            height: 90%;
            object-fit: cover;
            transform: scale(1.2);
            pointer-events: none;
            border-radius: 50%;
        }

        .quadrado:active {
            background-color: #383838;
            filter: brightness(1.6);
            transform: scale(0.98);
        }

        .quadrado.aceso {
            background-color: #555554;
            box-shadow: 0 0 30px #a9a7a7;
        }

        .quadrado.aceso:active {
            background-color: #4e4e4e;
            filter: brightness(1.3);
            transform: scale(0.98);
        }

        .retangulo {
            width: 100%;
            height: 110px;
            padding: 12px;
            background-color: #ff0000;
            border-radius: 12px;
            border: 1px solid #fff;
            color: #ffffff;
            font-size: 1.1em;
            font-weight: bold;
            cursor: pointer;
            user-select: none;
            display: flex;
            justify-content: center;
            align-items: center;
            overflow: hidden;
            transition: background-color 0.2s ease, box-shadow 0.2s ease, filter 0.1s ease, transform 0.1s ease;
            box-sizing: border-box;
        }

        .retangulo img {
            max-width: 100%;
            max-height: 100%;
            object-fit: contain;
            pointer-events: none;
        }

        .retangulo:active {
            filter: brightness(1.2);
            transform: scale(0.98);
        }

        .retangulo.aceso {
            background-color: #ff4040;
            box-shadow: 0 0 25px #ff0000;
        }

        .btn-acao {
            width: 100%;
            padding: 14px 20px;
            margin-top: 8px;
            background-color: #2196f3;
            color: #ffffff;
            border: 1px solid #fff;
            border-radius: 12px;
            font-size: 1em;
            font-weight: bold;
            cursor: pointer;
            user-select: none;
            transition: filter 0.15s ease, transform 0.1s ease;
            box-sizing: border-box;
        }

        .btn-acao:hover {
            filter: brightness(1.1);
        }

        .btn-acao:active {
            filter: brightness(1.2);
            transform: scale(0.98);
        }
    </style>
</head>
<body>
    <h1>Rizzotto & Rizzotto</h1>

    <div class="container">
        <div class="quadrado" id="luz">
            <img src="logo.jpg" alt="Logo Rizzotto & Rizzotto">
        </div>
        <button class="retangulo" id="luz2">
            <img src="image-removebg-preview.png" alt="Logo 2">
        </button>
        <button class="btn-acao" id="btn-acao">Modo 1</button>
    </div>
    
    <script>
        const luz1 = document.getElementById('luz');
        const luz2 = document.getElementById('luz2');
        const btnAcao = document.getElementById('btn-acao');

        let modo = 1;
        let ultimoModo = 1;

        [luz1, luz2].forEach(el => {
            el.addEventListener('click', () => {
                el.classList.toggle('aceso');

                if (modo !== 0) {
                    ultimoModo = modo;
                }
                modo = 0;
                btnAcao.innerText = "Modo manual";

                const c = luz1.classList.contains('aceso') ? 1 : 0;
                const b = luz2.classList.contains('aceso') ? 1 : 0;
                fetch('/manual?c=' + c + '&b=' + b);
            });
        });

        btnAcao.addEventListener('click', () => {
            if (modo === 0) {
                modo = ultimoModo;
            } else {
                modo = (modo % 5) + 1;
                ultimoModo = modo;
            }
            btnAcao.innerText = "Modo " + modo;

            luz1.classList.remove('aceso');
            luz2.classList.remove('aceso');

            fetch('/mudarModo?modo=' + modo);
        });
    </script>
</body>
</html>
)rawhtml");
                            break;
                        }
                    }
                    else
                    {
                        linhaAtual = "";
                    }
                }
                else if (c != '\r')
                {
                    linhaAtual += c;
                }
            }
        }
        client.stop();
    }
    switch (modo)
    {
    case 0:
        digitalWrite(luzCima, manualCima ? LOW : HIGH);
        digitalWrite(luzBaixo, manualBaixo ? LOW : HIGH);
        break;
    case 1:
        luzes1();
        break;
    case 2:
        luzes2();
        break;
    case 3:
        luzes3();
        break;
    case 4:
        luzes4();
        break;
    case 5:
        luzes5();
        break;                
    default:
        break;
    }
}
