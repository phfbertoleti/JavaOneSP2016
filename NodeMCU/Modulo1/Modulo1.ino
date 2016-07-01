#include <ESP8266WiFi.h> // Importa a Biblioteca ESP8266WiFi
#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
 
// WIFI
const char* SSID = " "; // SSID da Rede
const char* PASSWORD = " "; // Senha da Rede
 
// MQTT
const char* BROKER_MQTT = "iot.eclipse.org"; // IP/URL DO BROKER MQTT
int BROKER_PORT = 1883; // Porta do Broker MQTT

//defines gerais 
#define ID_MQTT  "JavaOneModulo01"     //id mqtt (para identificação de sessão)
#define ENDERECO_MODULO      1         //endereço deste módulo
#define ENDERECO_MODULO_STR  "1"       //endereço deste módulo
#define TOPICO_SUBSCRIBE "MQTTJavaOnePublisher"     //tópico MQTT de escuta
#define TOPICO_PUBLISH   "MQTTJavaOneSubscriber"    //tópico MQTT de envio de informações para Broker

//mapeamento de pinos do NodeMCU
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
char EstadoEntrada1;
char EstadoEntrada2;
char EstadoEntrada3;
 
//Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void EnviaEstadoInputsMQTT(void);
void InitOutputs(void);
void AtuaSaida(String saida, String acao); 
/* 
 *  Implementações das funções
 */
void setup() {
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  pinMode(D5,INPUT);
  pinMode(D6,INPUT);
  pinMode(D7,INPUT);
  
  InitOutputs();
  initSerial();
  initWiFi();
  initMQTT();
}
 
//Função: inicializa comunicação serial
//Parâmetros: nenhum
//Retorno: nenhum
void initSerial() {
  Serial.begin(115200);
}

//Função: inicializa e conecta-se na rede WiFi desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi() 
{
    delay(10);
    Serial.println();
    Serial.print("Conectando-se em: ");
    Serial.println(SSID);
 
    WiFi.begin(SSID, PASSWORD); // Conecta na Rede Wireless
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
  
    Serial.println();
    Serial.print("Conectado na Rede ");
    Serial.print(SSID);
    Serial.println(" | IP ");
    Serial.println(WiFi.localIP());
}
 
//Função: inicializa parâmetros de conexão MQTT(endereço do broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
    MQTT.setCallback(mqtt_callback); //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
 
//Função: função de callback (chamada toda vez que uma informação de um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
    
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
  
    //faz o parse da string
    String Modulo = msg.substring(0, 1);
    String Saida  = msg.substring(1, 2);
    String Acao = msg.substring(2, 3);

    if (Modulo.equals(ENDERECO_MODULO_STR))
       AtuaSaida(Saida,Acao);
   
}

//Função: atua nas saidas
//Parâmetros: saida que deseja atiuar e ação (ligar ou desligar)
//Retorno: nenhum
void AtuaSaida(String saida, String acao)
{
    if (saida.equals("1"))
    {
        if (acao.equals("1"))            
            digitalWrite(D0, HIGH);
        
        if (acao.equals("0"))            
            digitalWrite(D0, LOW);   

        return; 
    }

    if (saida.equals("2"))
    {
        if (acao.equals("1"))            
            digitalWrite(D1, HIGH);
        
        if (acao.equals("0"))            
            digitalWrite(D1, LOW);   
        
        return; 
    }

    if (saida.equals("3"))
    {
        if (acao.equals("1"))
            digitalWrite(D2, HIGH);
        
        if (acao.equals("0"))  
            digitalWrite(D2, LOW);   
        
        return; 
    }
}
 
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else 
        {
            Serial.println("Falha ao Reconectar");
            Serial.println("Tentando se reconectar em 2 segundos");
            delay(2000);
        }
    }
}
 
//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void recconectWiFi() 
{
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
}

//Função: verifica o estado das conexões WiFI e ao broker MQTT. Em caso de desconexão (qualquer uma das duas), a conexão é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
    
     recconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

//Função: envia ao Broker o estado atual de todos os inputs 
//Parâmetros: nenhum
//Retorno: nenhum
void EnviaEstadoInputsMQTT(void)
{
    char payloadChar[6];
    int EnderecoModulo = ENDERECO_MODULO;
    char payload[4];

    EstadoEntrada1 = '0';
    if (digitalRead(D5) == HIGH)
        EstadoEntrada1 = '1'; 
    
    EstadoEntrada2 = '0';
    if (digitalRead(D6) == HIGH)
        EstadoEntrada2 = '1'; 
    
    EstadoEntrada3 = '0';
    if (digitalRead(D7) == HIGH)
        EstadoEntrada3 = '1'; 

    sprintf(payloadChar,"%d%c%c%c", EnderecoModulo,
                                    EstadoEntrada1,
                                    EstadoEntrada2,
                                    EstadoEntrada3);
                                       
    memcpy(payload,payloadChar,4);

    MQTT.publish(TOPICO_PUBLISH, payloadChar);
    Serial.println("- Estado das saidas enviado ao broker!");
    delay(1000);
}

//Função: inicializa todos os outputs em nível lógico baixo
//Parâmetros: nenhum
//Retorno: nenhum
void InitOutputs(void)
{
    digitalWrite(D0, LOW);          
    digitalWrite(D1, LOW);          
    digitalWrite(D2, LOW);              
}
//programa principal
void loop() 
{
    //garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();

    //envia o status de todos os outputs para o Broker no protocolo esperado
    EnviaEstadoInputsMQTT();

    //keep-alive da comunicação com broker MQTT
    MQTT.loop();
    
}
