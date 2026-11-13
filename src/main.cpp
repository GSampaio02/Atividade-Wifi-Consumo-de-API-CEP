#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

// Conexao WIFI
void conexaoWifi();
void reconexaoWiFi();

const char *SSID = "SENAI EDUCACIONAL";
const char *SENHA = "info@134";

const unsigned long tempoConexao = 10000;
const unsigned long tempoReconexao = 5000;

// Consome API CEP
const String enderecoAPI = "https://brasilapi.com.br/api/cep/v2/";
String consultaApi(String cep);

bool cepInvalido;
bool MostrarCep = false;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(15000);

  conexaoWifi();

  // String cep = "09510200";

  Serial.println("Por favor digite um cep: ");
}

void loop()
{
  reconexaoWiFi();

  if (Serial.available())
  {
    String mensagem = Serial.readStringUntil('\n');
    mensagem.trim();
    if (mensagem.length() > 0)
    {
      if (!cepInvalido)
      {
        if (mensagem.length() == 8)
        {
          MostrarCep = true;
        }
        else
        {
          Serial.println("CEP Invalido, por favor digite um CEP valido");
        }
      }
      ///////////////////////////////////////////////////////////////////////
      if (MostrarCep)
      {
        String respostaAPiCep = consultaApi(mensagem);

        if (respostaAPiCep != "")
        {
          // Cria o objeto JSON
          JsonDocument doc;

          DeserializationError erro = deserializeJson(doc, respostaAPiCep);

          if (!erro)
          {
            String rua = doc["street"];
            String estado = doc["state"];
            String local = doc["neighborhood"];
            String cep = doc["cep"];
            String formatoHora = doc["timezoneName"];

            Serial.println();
            Serial.println("DADOS DO ENDERECO");
            Serial.println("--------------------------");
            Serial.println("CEP:      " + cep);
            Serial.println("RUA:      " + rua);
            Serial.println("ESTADO:      " + estado);
            Serial.println("LOCAL:      " + local);
            Serial.println("FORMATO HORA:      " + formatoHora);
            MostrarCep = false;
          }
        }
      }
    }
  }
}

void conexaoWifi()
{
  Serial.printf("Conectando ao WIFI: %s", SSID);

  WiFi.begin(SSID, SENHA);

  unsigned long tempoWifi = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - tempoWifi < tempoConexao)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("Wifi conectado com sucesso");
    Serial.print("Endereço IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("Falha ao se conectar");
  }
}

void reconexaoWiFi()
{
  unsigned long tempoAtual = millis();
  unsigned long tempoUltimaConexao = 0;

  if (tempoAtual - tempoUltimaConexao > tempoReconexao)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println("Conexao Wifi perdida");
      conexaoWifi();
    }
    tempoUltimaConexao = tempoAtual;
  }
}

String consultaApi(String cep)
{
  HTTPClient http; // Cria o objetivo http

  http.begin(enderecoAPI + cep); // Inicializa o http utilizando o endereço + o codigo da função
  int codigoHTTP = http.GET();   // Codigo da pagina http
  String resposta = "";          // Retorna o testo

  delay(3000);

  if (codigoHTTP == 200)
  {
    resposta = http.getString();
    cepInvalido = false;
  }
  else if (codigoHTTP == 400)
  {
    cepInvalido = true;
    Serial.println("Deu erro. Codigo: " + String(codigoHTTP));
  }
  else
  {
    Serial.println("Deu erro. Codigo: " + String(codigoHTTP));
  }

  http.end();
  return resposta; // Retorna pra fora da função oque foi recebido
}