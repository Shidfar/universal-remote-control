#include <SPI.h>
#include <WiFi.h>
#include <WifiIPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
#include <aJSON.h>
#include <SLFS.h>

#define YELLOW 9
#define GREEN 10
#define RED 29

#define EXTLED 27

//#define FREE_PIN 28 //---------

#define ENABLER 7

#define IRLED 30
#define IRRCV 8

#define CLOUDIFY 5
#define LEARN 4
#define TEST 11

//define FREE_PIN 12

#define MAXPULSE 65000
#define RESOLUTION 10

char ssid[] = "controll@unifi";
char pswd[] = "Dushanbe14";

char hostname[] = "192.168.0.10";//"iot.eclipse.org";
int port = 1883;

const char* subs = "/cc3200/";
const char* pubs = "/pub/1/cc3200/";

WifiIPStack ipstack;
MQTT::Client<WifiIPStack, Countdown> client = MQTT::Client<WifiIPStack, Countdown>(ipstack);

uint16_t pulses[100][2];
uint8_t currentpulse = 0;
char printbuf[801];

char* answer;
int REF_TABN[124] = {0};
char REF_TABC[65];

bool enabled;

void initRef()
{
    int c = 0;
    for(int i=48; i<58; i++)
        REF_TABC[c] = char(i), REF_TABN[i] = c++;
    for(int i=65; i<91; i++)
        REF_TABC[c] = char(i), REF_TABN[i] = c++;
    for(int i=97; i<123; i++)
        REF_TABC[c] = char(i), REF_TABN[i] = c++;
    REF_TABC[c] = char(35), REF_TABN[35] = c++;
    REF_TABC[c] = char(36), REF_TABN[36] = c++;
}

void send_codes()
{
  //cc = 0;
  sprintf(printbuf, "");
  for (uint8_t i = 0; i < currentpulse-1; i++)
  {
    //pulse IR
    sprintf(printbuf + strlen(printbuf), encode(pulses[i][1]));
    sprintf(printbuf + strlen(printbuf), " ");
    //delay Microseconds
    sprintf(printbuf + strlen(printbuf), encode(pulses[i+1][0]));
    sprintf(printbuf + strlen(printbuf), " ");
  }
  //pulse IR
  sprintf(printbuf + strlen(printbuf), encode(pulses[currentpulse-1][1]));
  //Serial.print(pulses[currentpulse-1][1] * RESOLUTION, DEC);
  Serial.print("to cloud: ");
  Serial.println(printbuf);
  
  if (!client.isConnected())
    connect_mqtt();
  
  MQTT::Message message;
  message.qos = MQTT::QOS2;
  message.retained = false;
  message.dup = false;
  message.payload = (void*)printbuf;
  message.payloadlen = strlen(printbuf)+1;
  int rc = client.publish(pubs, message);
}

void setup()
{
  Serial.begin(9600);
  SerFlash.begin();
  initRef();
  pinMode(YELLOW, OUTPUT);
  digitalWrite(YELLOW, HIGH);
  pinMode(GREEN, OUTPUT);
  digitalWrite(GREEN, HIGH);
  pinMode(RED, OUTPUT);
  digitalWrite(RED, HIGH);
  pinMode(EXTLED, OUTPUT);
  digitalWrite(EXTLED, HIGH);

  pinMode(IRLED, OUTPUT);
  pinMode(IRRCV, INPUT);
  digitalWrite(IRLED, LOW);
  

  pinMode(LEARN, INPUT_PULLUP);
  pinMode(TEST, INPUT_PULLUP);
  pinMode(CLOUDIFY, INPUT_PULLUP);
  pinMode(ENABLER, INPUT_PULLUP);

  WiFi.begin(ssid, pswd);
  if(!connect_wifi())
  {delay(10000); }
  else
  {
    obtain_ip();
    connect_mqtt();
    if(client.isConnected())
    {
      Serial.println("MQTT is connected.");
    }
  }
}

void loop()
{
  
  client.yield(1000);
  if(digitalRead(ENABLER) == LOW)
  {
    enabled = true;
    digitalWrite(EXTLED, LOW);
    delay(1000);
    TreeButtons();
    digitalWrite(EXTLED, HIGH);
    delay(1000);
  }
}
void TreeButtons()
{
  while(enabled)
  {
    if(digitalRead(LEARN) == LOW)
    {
      Serial.println("Learning");
      digitalWrite(GREEN, HIGH);
      delayMicroseconds(23);
      currentpulse=0;
      read_ir();
      delayMicroseconds(23);
      digitalWrite(GREEN, LOW);
      Serial.println("Done");
    }
    if(digitalRead(TEST) == LOW)
    {
      Serial.println("Sending IR");
      digitalWrite(RED, HIGH);
      delayMicroseconds(23);
      test_ir();
      delayMicroseconds(23);
      digitalWrite(RED, LOW);
      Serial.println("Done");
    }
    if(digitalRead(CLOUDIFY) == LOW)
    {
      Serial.println("Sending Message");
      delayMicroseconds(23);
      send_codes();
      delayMicroseconds(23);
      Serial.println("Done");
    }
    if(digitalRead(ENABLER) == LOW)
    {
      enabled = false;
      delay(1000);
    }
  }
}
void printpulses(void)
{
  Serial.println("\n\r\n\rReceived: \n\rOFF \tON");
  for (uint8_t i = 0; i < currentpulse; i++)
  {
    Serial.print(pulses[i][0] * RESOLUTION, DEC);
    Serial.print("\t");
    Serial.println(pulses[i][1] * RESOLUTION, DEC);
  }
  Serial.println("int IRsignal[] = {");
  Serial.println("// ON, OFF ");
  
  for (uint8_t i = 0; i < currentpulse-1; i++)
  {
    Serial.print("pulseIR(");
    Serial.print(pulses[i][1] * RESOLUTION , DEC);
    Serial.print(");");
    Serial.println("");
    Serial.print("delayMicroseconds(");
    Serial.print(pulses[i+1][0] * RESOLUTION , DEC);
    Serial.println(");");
  }
  Serial.print("pulseIR(");
  Serial.print(pulses[currentpulse-1][1] * RESOLUTION, DEC);
  Serial.print(");");
}
void test_ir()
{
  for (uint8_t i = 0; i < currentpulse-1; i++)
  {
    pulseIR(pulses[i][1] * RESOLUTION);
    delayMicroseconds(pulses[i+1][0] * RESOLUTION);
  }
  pulseIR(pulses[currentpulse-1][1] * RESOLUTION);
  Serial.println("Sending Done");
}
void read_ir()
{
  while(true)
  {
    uint16_t highpulse, lowpulse;
    highpulse = lowpulse = 0;
    while(digitalRead(IRRCV) == HIGH)
    {
       highpulse++;
       delayMicroseconds(RESOLUTION);
       if ((highpulse >= MAXPULSE) && (currentpulse != 0))
       {
         printpulses();
         return;
       }
    }
    pulses[currentpulse][0] = highpulse;
    while (digitalRead(IRRCV) == LOW)
    {
       lowpulse++;
       delayMicroseconds(RESOLUTION);
       if ((lowpulse >= MAXPULSE)  && (currentpulse != 0))
       {
         printpulses();
         return;
       }
    }
    pulses[currentpulse][1] = lowpulse;
    currentpulse++;
  }
}
void pulseIR(long microsecs)
{
  noInterrupts();
  while (microsecs > 0)
  {
     digitalWrite(IRLED, HIGH);
     delayMicroseconds(10);
     digitalWrite(IRLED, LOW);
     delayMicroseconds(10);
     microsecs -= 26;
  }
  interrupts();
}
bool connect_wifi()
{
  Serial.println("Connecting to WiFi.");
  int c = 0;
  while(WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(RED, LOW);
    c++;
    if(c == 451)
      return false;
    delay(100);
    Serial.print(".");
    digitalWrite(RED, HIGH);
    delay(100);
  }
  return true;
}
void obtain_ip()
{
  Serial.println("Obtaining IP.");
  int c = 0;
  while (WiFi.localIP() == INADDR_NONE)
  {
    digitalWrite(YELLOW, LOW);
    Serial.print(".");
    delay(100);
    digitalWrite(YELLOW, HIGH);
    delay(100);
  }
}
void blink(int PIN, int N)
{
  while(N--)
  {
    digitalWrite(PIN, HIGH);
    delay(200);
    digitalWrite(PIN, LOW);
    delay(200);
  }
}
void connect_mqtt()
{
  int rc = ipstack.connect(hostname, port);
  if (rc != 1)
  {blink(GREEN, 2);}
 
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
  data.MQTTVersion = 3;
  data.clientID.cstring = (char*) "cc3200_01";
  rc = client.connect(data);
  if (rc != 0)
  {blink(GREEN, 3);}
  rc = client.subscribe(subs, MQTT::QOS2, messageArrived);
  if (rc != 0)
  {blink(GREEN, 4);}
  else
  {
    blink(RED, 6);
    blink(YELLOW, 4);
    blink(GREEN, 2);
    digitalWrite(RED, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, LOW);
  }
}
void messageArrived(MQTT::MessageData& md)
{
  char cloudbuf[801];
  MQTT::Message &message = md.message;
  sprintf(cloudbuf, "%s", (char*)message.payload);
  Serial.println(cloudbuf);
  /*currentpulse = 1;
  char ine[4];
  int cc = 0;
  for(int i=0; i<strlen(cloudbuf); i++)
  {
   if(cloudbuf[i] == ' ')
   {
       ine[cc] = '\0';
       pulses[currentpulse/2][(currentpulse)%2] = decode(ine, strlen(ine));
       currentpulse++;
       cc = 0;
       continue;
   }
   else
   {
       ine[cc++] = cloudbuf[i];
   }
  }
  currentpulse/=2;
  digitalWrite(EXTLED, LOW);
  delayMicroseconds(23);
  test_ir();
  delayMicroseconds(23);
  digitalWrite(EXTLED, HIGH);*/
}
char* encode(long x)
{
    int lg = 0;
    long cp = x;
    while(cp)cp/=64, lg++;

    char *r = (char*) malloc(sizeof(char)*lg);
    int i = lg;
    while(i>0)
    {
        i--;
        r[i] = REF_TABC[(x%64)];
        x/=64;
    }
    r[lg] = '\0';
    return r;
}
long decode(char* buf, int len)
{
    long ans = 0, c = 1;
    while(len--)
    {
        ans += (c*REF_TABN[buf[len]]);
        c*=64;
    }
    return ans;
}
