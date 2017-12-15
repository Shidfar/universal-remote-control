#include <WiFi.h>
#include <WifiIPStack.h>
#include <Countdown.h>
#include <MQTTClient.h>
// #include <PubSubClient.h>

#define MQTTCLIENT_QOS2 1

#define RED 29
#define YELLOW 9
#define GREEN 10

#define LEARN_IN 3
#define TEST_IN 11
#define SEND_IN 5

#define STATUS_OUT 27
#define ENABLE_IN 7

#define IR_OUT 30
#define IR_IN 8

//#define FREE_IN 12
//#define FREE_IN 4
//#define FREE_PIN 28

#define MAXPULSE 65000
#define RESOLUTION 13

#define MQTT_HOST "192.168.0.2" //"tractive.asuscomm.com"
#define MQTT_PORT 1883 //1316
#define MQTT_ID "cc3200_01"

#define SUBSCR "/cc3200_01"
#define PUBLSH "/pub/cc3200_01"

WiFiUDP Udp;
#define PORT 13
WifiIPStack ipstack;
MQTT::Client<WifiIPStack, Countdown> client = MQTT::Client<WifiIPStack, Countdown>(ipstack);

char ssid[33] = "";
char pswd[33] = "";

bool enabled;
uint16_t pulses[900][2];
uint8_t currentpulse = 0;

int  REF_TABN[124] = {0};
char REF_TABC[65];

char commandIn[4000];
int commandLen;

// MQTTPacket_connectData data;

void setup()
{
    //Serial.begin(115200);
    Serial.begin(9600);
    pinMode(RED, OUTPUT); pinMode(YELLOW, OUTPUT); pinMode(GREEN, OUTPUT);
    pinMode(LEARN_IN, INPUT); pinMode(TEST_IN, INPUT); pinMode(SEND_IN, INPUT_PULLUP);
    pinMode(ENABLE_IN, INPUT_PULLUP); pinMode(IR_IN, INPUT); pinMode(IR_OUT, OUTPUT);
    pinMode(STATUS_OUT, OUTPUT);
    digitalWrite(RED, LOW); digitalWrite(YELLOW, LOW); digitalWrite(GREEN, LOW);
    digitalWrite(IR_OUT, LOW); digitalWrite(STATUS_OUT, LOW);
    
    while (!_1CONNECT_WIFI())
    {
        _2CONFIGURE();
    }
    Serial.print("\nConnected to: ");
    Serial.println(ssid);
    while(!_3CONNECT_MQTT())
    {
        Serial.println("Connection Failed, Reconnecting");
        delay(500);
    }
    _4INIT_REFERENCES();
    digitalWrite(STATUS_OUT, HIGH);
    send_message("(c) Shidfar H.");
    digitalWrite(RED, HIGH); delay(200); digitalWrite(RED, LOW); delay(200); digitalWrite(RED, LOW); delay(200);
    digitalWrite(YELLOW, HIGH); delay(200); digitalWrite(YELLOW, LOW); delay(200); digitalWrite(YELLOW, LOW); delay(200);
    digitalWrite(GREEN, HIGH); delay(200); digitalWrite(GREEN, LOW); delay(200); digitalWrite(GREEN, LOW); delay(200);
}

void loop()
{
    if (!client.isConnected())
        while(!_3CONNECT_MQTT());
    client.yield(1000);
    if(digitalRead(ENABLE_IN) == LOW)
    {
        enabled = true;
        digitalWrite(STATUS_OUT, LOW);
        delay(1000);
        ButtonsListener();
        digitalWrite(STATUS_OUT, HIGH);
        delay(1000);
    }
}

/** - Loop functions - **/

void ButtonsListener()
{
    Serial.println("Listenning to buttons");
    while(enabled)
    {
        if(digitalRead(ENABLE_IN) == LOW)
            enabled = false;
        if(digitalRead(LEARN_IN) == HIGH)
        {
            Serial.println("Learning");
            digitalWrite(GREEN, HIGH);
            delayMicroseconds(40);
            currentpulse = 0;
            ulearn();
            delayMicroseconds(100);
            digitalWrite(GREEN, LOW);
            Serial.println("Learning Done");
        }
        if(digitalRead(TEST_IN) == HIGH)
        {
            delayMicroseconds(40);
            utest();
            delayMicroseconds(100);
        }
        if(digitalRead(SEND_IN) == LOW)
        {
            delayMicroseconds(100);
            usend();
            delayMicroseconds(300);
        }
        // Serial.println(GPIOA0_BASE, BIN);
        // Serial.println(GPIOA1_BASE, BIN);
        // Serial.println(GPIOA2_BASE, BIN);
        // Serial.println(GPIOA3_BASE, BIN);
        // Serial.println();
        /*
         
         
         */
    }
}

void ulearn()
{
    currentpulse = 0;
    while (true)
    {
        uint16_t highpulse, lowpulse;
        highpulse = lowpulse = 0;
        while (digitalRead(IR_IN) == HIGH)
        {
            //      digitalRead(IR_IN);
            highpulse++;
            delayMicroseconds(RESOLUTION);
            if ((highpulse >= MAXPULSE) && (currentpulse != 0))
            {
                printpulses();
                return;
            }
        }
        pulses[currentpulse][0] = highpulse;
        while (digitalRead(IR_IN) == LOW)
        {
            //      digitalRead(IR_IN);
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
void printpulses(void)
{
    Serial.println("\n\r\n\rReceived: \n\rOFF \tON");
    for (uint8_t i = 0; i < currentpulse; i++)
    {
        Serial.print(pulses[i][0] * RESOLUTION, DEC);
        Serial.print("\t");
        Serial.println(pulses[i][1] * RESOLUTION, DEC);
    }
    //   Serial.println("int IRsignal[] = {");
    //   Serial.println("// ON, OFF ");
    //
    //   for (uint8_t i = 0; i < currentpulse - 1; i++)
    //   {
    //     Serial.print("pulseIR(");
    //     Serial.print(pulses[i][1] * RESOLUTION , DEC);
    //     Serial.print(");");
    //     Serial.println("");
    //     Serial.print("delayMicroseconds(");
    //     Serial.print(pulses[i + 1][0] * RESOLUTION , DEC);
    //     Serial.println(");");
    //   }
    //   Serial.print("pulseIR(");
    //   Serial.print(pulses[currentpulse - 1][1] * RESOLUTION, DEC);
    //   Serial.print(");");
}

void utest()
{
    for (uint8_t i = 0; i < currentpulse - 1; i++)
    {
        pulseIR(pulses[i][1] * RESOLUTION);
        delayMicroseconds(pulses[i + 1][0] * RESOLUTION);
    }
    pulseIR(pulses[currentpulse - 1][1] * RESOLUTION);
    Serial.println("Testing Done");
}
void pulseIR(long microsecs)
{
    noInterrupts();
    while (microsecs > 0)
    {
        digitalWrite(IR_OUT, HIGH);
        digitalWrite(IR_OUT, HIGH);
        delayMicroseconds(12);
        digitalWrite(IR_OUT, LOW);
        digitalWrite(IR_OUT, LOW);
        delayMicroseconds(12);
        microsecs -= 26;
    }
    interrupts();
}

void usend()
{
    
    int msgc = 0;
    char outmsg[68];
    char buff[801];
    int len;
    
    bzero(buff, 801);
    sprintf(buff, "");
    for (uint8_t i = 0; i < currentpulse - 1; i++)
    {
        //pulse IR
        sprintf(buff + strlen(buff), encode(pulses[i][1]));
        sprintf(buff + strlen(buff), " ");
        //delay Microseconds
        sprintf(buff + strlen(buff), encode(pulses[i + 1][0]));
        sprintf(buff + strlen(buff), " ");
    }
    //pulse IR
    sprintf(buff + strlen(buff), encode(pulses[currentpulse - 1][1]));
    //Serial.print(pulses[currentpulse-1][1] * RESOLUTION, DEC);
    // Serial.print("to cloud: ");
    // Serial.println(buff);
    len = strlen(buff);
    
    outmsg[msgc++] = '{';
    outmsg[msgc++] = '[';
    for (int i = 0; i < len; i++)
    {
        if (buff[i] == ' ' && outmsg[msgc - 1] == ' ')
        {
            msgc--;
        }
        if (msgc > 56 && buff[i] == ' ')
        {
            outmsg[msgc++] = ']', outmsg[msgc++] = '\0';
            send_message(outmsg);
            delay(23);
            msgc = 0, outmsg[msgc++] = '[';
            continue;
        }
        outmsg[msgc++] = buff[i];
    }
    outmsg[msgc++] = ']';
    outmsg[msgc++] = '}';
    outmsg[msgc++] = '\0';
    
    delay(23);
    send_message(outmsg);
}

/** - Configuration functions - **/

bool _1CONNECT_WIFI()
{
    if(strcmp(ssid, "") == 0)
        return false;
    Serial.println();
    Serial.println("Connecting to WiFi.");
    WiFi.begin(ssid, pswd);
    int c = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(RED, LOW);
        c++;
        if (c == 112)
            return false;
        delay(100);
        Serial.print(".");
        digitalWrite(RED, HIGH);
        delay(100);
    }
    Serial.println();
    Serial.println("Obtaining IP.");
    c = 0;
    while (WiFi.localIP() == INADDR_NONE)
    {
        digitalWrite(YELLOW, LOW);
        c++;
        if (c == 225)
            return false;
        delay(100);
        Serial.print(".");
        digitalWrite(YELLOW, HIGH);
        delay(100);
    }
    return true;
}

void _2CONFIGURE()
{
    char packetBuffer[255];
    const char _ssid[] = "URC@SH_inc";
    const char _pass[] = "password";
    digitalWrite(RED, HIGH); digitalWrite(YELLOW, HIGH); digitalWrite(GREEN, HIGH);
    delay(100);
    Serial.println("");
    Serial.print("Setting up AP: "); Serial.println(_ssid);
    Serial.print("WPA  Password: "); Serial.println(_pass);
    WiFi.stopNetwork();
    Serial.println("!");
    WiFi.beginNetwork((char*)_ssid, (char*)_pass);
    Serial.println("?");
    while(WiFi.localIP() == INADDR_NONE)
    {
        Serial.print('.');
        digitalWrite(YELLOW, LOW); delay(150);
        digitalWrite(YELLOW, HIGH); delay(150);
    }
    Serial.println();
    for(int i=0; i<3; i++)
    {
        digitalWrite(RED, LOW); digitalWrite(YELLOW, LOW); digitalWrite(GREEN, LOW);
        delay(150);
        digitalWrite(RED, HIGH); digitalWrite(YELLOW, HIGH); digitalWrite(GREEN, HIGH);
        delay(150);
    }
    Serial.println("AP active.");
    delay(150);
    
    Udp.begin(PORT);
    
    unsigned int num_clients = 0;
    int len;
    while(true)
    {
        unsigned int a, i;
        a = WiFi.getTotalDevices();
        if(a != num_clients)
        {
            if(a > num_clients)
            {
                Serial.println("Client connected! All client: ");
                for(i=0; i<a; i++)
                {
                    Serial.print(" Client #"); Serial.print(i); Serial.print(WiFi.deviceIpAddress(i)); Serial.println(WiFi.deviceMacAddress(i));
                }
            }
            else
            {
                Serial.println("Client Disconnected");
            }
            num_clients = a;
        }
        int packetSize = Udp.parsePacket();
        if (packetSize)
        {
            len = Udp.read(packetBuffer, 255);
            if (len > 0) packetBuffer[len] = 0;
            Serial.print("Contents: ");
            Serial.println(packetBuffer);
            
            int len = strlen(packetBuffer);
            int cl = 0;
            int j = 0;
            int i;
            bzero(ssid, 33);
            bzero(pswd, 33);
            
            for(i=0; i<len; i++)
            {
                if(packetBuffer[i] == '"')
                { cl++; continue;}
                if(packetBuffer[i] == '>' || packetBuffer[i] == '<')
                { j = 0; continue; }
                if(cl == 1)
                { ssid[j++] = packetBuffer[i]; continue; }
                if(cl == 3)
                { pswd[j++] = packetBuffer[i]; continue; }
            }
            
            Serial.print("SSID: ");
            Serial.print(ssid);
            Serial.print(" ");
            Serial.println(strlen(ssid));
            Serial.print("PSWD: ");
            Serial.print(pswd);
            Serial.print(" ");
            Serial.println(strlen(pswd));
            Serial.println("Closing Connection");
            delay(500);
            WiFi.stopNetwork();
            delay(3000);
            return;
        }
    }
}

void blink(unsigned int LED, int times)
{
    for(int i = 0; i<times; i++)
    {
        digitalWrite(LED, HIGH);
        delay(60);
        digitalWrite(LED, LOW);
        delay(60);
    }
    
}

bool _3CONNECT_MQTT()
{
    char printbuf[100];
    const char* topic = SUBSCR;
    char hostname[] = MQTT_HOST;
    int port = MQTT_PORT;
    blink(RED, 5); blink(GREEN, 5); blink(YELLOW, 5);
    sprintf(printbuf, "Connecting to %s:%d\n", hostname, port);
    Serial.print(printbuf);
    int rc = ipstack.connect(hostname, port);
    if (rc != 1)
    {
        sprintf(printbuf, "rc from TCP connect is %d\n", rc);
        Serial.print(printbuf);
    }
    
    Serial.println("MQTT connecting");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char*)MQTT_ID;
    rc = client.connect(data);
    if (rc != 0)
    {
        sprintf(printbuf, "rc from MQTT connect is %d\n", rc);
        Serial.print(printbuf);
    }
    Serial.println("MQTT connected");
    
    rc = client.subscribe(topic, MQTT::QOS0, messageArrived);
    if (rc != 0)
    {
        sprintf(printbuf, "rc from MQTT subscribe is %d\n", rc);
        Serial.print(printbuf);
    }
    Serial.println("MQTT subscribed");
    return client.isConnected();
    //  //client.disconnect();
    //  for(int i = 0; i<5; i++)
    //  {
    //    digitalWrite(GREEN, HIGH); digitalWrite(YELLOW, HIGH);
    //    delay(60);
    //    digitalWrite(GREEN, LOW); digitalWrite(YELLOW, LOW);
    //    delay(60);
    //  }
    //  int rc = ipstack.connect(MQTT_HOST, MQTT_PORT);
    //  Serial.println("MQTT connecting");
    //  data = MQTTPacket_connectData_initializer;
    //  data.MQTTVersion = 3;
    //  data.clientID.cstring = (char*)MQTT_ID;
    //  rc = client.connect(data);
    //  Serial.println("MQTT connected");
    //  rc = client.subscribe(SUBSCR, MQTT::QOS0, messageArrived);
    //  Serial.println("MQTT subscribed");
    //  return client.isConnected();
}

void _4INIT_REFERENCES()
{
    int c = 0;
    for (int i = 48; i < 58; i++)
        REF_TABC[c] = char(i), REF_TABN[i] = c++;
    for (int i = 65; i < 91; i++)
        REF_TABC[c] = char(i), REF_TABN[i] = c++;
    for (int i = 97; i < 123; i++)
        REF_TABC[c] = char(i), REF_TABN[i] = c++;
    REF_TABC[c] = char(35), REF_TABN[35] = c++;
    REF_TABC[c] = char(36), REF_TABN[36] = c++;
}
/** - Helpers & Callbacks - **/

void send_message(char *str)
{
    const char* topic = PUBLSH;
    if (!client.isConnected())
        while(!_3CONNECT_MQTT());
    Serial.println(PUBLSH);
    Serial.println(str);
    
    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*)str;
    message.payloadlen = strlen(str) + 1;
    int rc = client.publish(topic, message);
}

void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;
    //  Serial.print(message.qos);
    //  Serial.print(message.retained);
    //  Serial.print(message.dup);
    //  Serial.print(message.id);
    //  Serial.print((char*)message.payload);
    
    char line[68];
    bzero(line, 68);
    sprintf(line, "%s", (char *) message.payload);
    Serial.println(line);
    int len = strlen(line);
    bool square = false;
    int curly = 0;
    // char commandIn[805];
    // int commandLen;
    for(int i=0; i<len; i++)
    {
        if(line[i] == '{')
        {
            curly = 1, commandLen = 0;
            continue;
        }
        if(line[i] == '}')
        {
            curly = 2, commandIn[commandLen] = '\0';
            break;
        }
        if(line[i] == '[')
        {
            square = true;
            continue;
        }
        if(line[i] == ']')
        {
            square = false;
            continue;
        }
        if(square)
        {
            if(line[i] == ' ' && commandIn[commandLen-1] == ' ')
                commandLen --;
            commandIn[commandLen++] = line[i];
        }
        else
            break;
    }
    commandIn[commandLen++] = ' ';
    if(curly == 2)
    {
        Serial.println(commandIn);
        fill_pulses(commandIn);
        bzero(commandIn, 805);
    }
    if (!client.isConnected())
        while(!_3CONNECT_MQTT());
}

void fill_pulses(char *str)
{
    currentpulse = 1;
    char ine[4];
    int cc = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] == ' ')
        {
            ine[cc] = '\0';
            pulses[currentpulse / 2][(currentpulse) % 2] = decode(ine, strlen(ine));
            currentpulse++;
            cc = 0;
            continue;
        }
        else
        {
            ine[cc++] = str[i];
        }
    }
    currentpulse /= 2;
    digitalWrite(STATUS_OUT, LOW);
    delay(23);
    utest();
    delay(23);
    digitalWrite(STATUS_OUT, HIGH);
}

char* encode(long x)
{
    int lg = 0;
    long cp = x;
    while (cp)cp /= 64, lg++;
    
    char *r = (char*) malloc(sizeof(char) * lg);
    int i = lg;
    while (i > 0)
    {
        i--;
        r[i] = REF_TABC[(x % 64)];
        x /= 64;
    }
    r[lg] = '\0';
    return r;
}

long decode(char* buf, int len)
{
    long ans = 0, c = 1;
    while (len--)
    {
        ans += (c * REF_TABN[buf[len]]);
        c *= 64;
    }
    return ans;
}
