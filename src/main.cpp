#include <Arduino.h>

#define dbg

char aux_str[100];
const char * __APN      = "internet.itelcel.com";
const char * __usrnm    = "webgprs";
const char * __password = "webgprs2002";



unsigned int Counter = 0;
unsigned long datalength, checksum, rLength;
unsigned short topiclength;
unsigned short topiclength2;
unsigned char topic[30];
char str[250];
unsigned char encodedByte;
int X;

unsigned short MQTTProtocolNameLength;
unsigned short MQTTClientIDLength;
unsigned short MQTTUsernameLength;
unsigned short MQTTPasswordLength;
unsigned short MQTTTopicLength;

const char * MQTTHost = "spiraldevnew.sml.mx";
const char * MQTTPort = "1883";
const char * MQTTClientID = "SMLOO1";
const char * MQTTTopic = "AEREADOR";
const char * MQTTTopic2 = "aereador2";
const char * MQTTProtocolName = "MQIsdp";
const char MQTTLVL = 0x03;
const char MQTTFlags = 0xC2;
const unsigned int MQTTKeepAlive = 60;
const char * MQTTUsername = "testiosa";
const char * MQTTPassword = "sml6688154252";
const char MQTTQOS = 0x00;
const char MQTTPacketID = 0x0001;

char sim800lreset = 4;












void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);

  pinMode(sim800lreset, OUTPUT);
  delay(3000);
  MQTTProtocolNameLength = strlen(MQTTProtocolName);



}








void loop()
{
  if (initTCP()) {
    MQTTConnect();
    //readServerResponse("AT+CIPRXGET=2,1024", "OK", "ERROR", 10000);//receive data from n/w

    MQTTsubscribe();
    //readServerResponse("AT+CIPRXGET=2,1024", "OK", "ERROR", 10000);//receive data from n/w



    while (1) {
      MQTTpublish();
      readServerResponse("AT+CIPRXGET=2,1024", "OK", "ERROR", 10000);//receive data from n/w

      Counter++;
      delay(5000);
    }

  }
}






int  MQTTConnect() {

  if (sendATcommand2("AT+CIPSEND", ">", "ERROR", 1000)) {
    Serial2.write(0x10);
    MQTTProtocolNameLength = strlen(MQTTProtocolName);
    MQTTClientIDLength = strlen(MQTTClientID);
    MQTTUsernameLength = strlen(MQTTUsername);
    MQTTPasswordLength = strlen(MQTTPassword);
    datalength = MQTTProtocolNameLength + 2 + 4 + MQTTClientIDLength + 2 + MQTTUsernameLength + 2 + MQTTPasswordLength + 2;
    X = datalength;
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }

      Serial2.write(encodedByte);
    }
    while ( X > 0 );
    Serial2.write(MQTTProtocolNameLength >> 8);
    Serial2.write(MQTTProtocolNameLength & 0xFF);
    Serial2.write(MQTTProtocolName);

    Serial2.write(MQTTLVL); // LVL
    Serial2.write(MQTTFlags); // Flags
    Serial2.write(MQTTKeepAlive >> 8);
    Serial2.write(MQTTKeepAlive & 0xFF);


    Serial2.write(MQTTClientIDLength >> 8);
    Serial2.write(MQTTClientIDLength & 0xFF);
    Serial2.print(MQTTClientID);


    Serial2.write(MQTTUsernameLength >> 8);
    Serial2.write(MQTTUsernameLength & 0xFF);
    Serial2.print(MQTTUsername);


    Serial2.write(MQTTPasswordLength >> 8);
    Serial2.write(MQTTPasswordLength & 0xFF);
    Serial2.print(MQTTPassword);

    Serial2.write(0x1A);
    if (sendATcommand2("", "SEND OK", "SEND FAIL", 5000)) {
      Serial.println(F("CONNECT PACKET SUCCESS"));
      return 1;
    }
    else return 0;
  }
}


int  MQTTpublish() {
  if (sendATcommand2("AT+CIPSEND", ">", "ERROR", 1000)) {

    memset(str, 0, sizeof(str));

    topiclength = sprintf((char*)topic, MQTTTopic);
    //datalength = sprintf((char*)str,"%s%u",topic,Counter);
    datalength = sprintf((char*)str, "%s%s", topic, "Counter$$%%^&&&&**&^%$#");

    delay(1000);
    Serial2.write(0x30);
    X = datalength + 2;
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }
      Serial2.write(encodedByte);
    }
    while ( X > 0 );

    Serial2.write(topiclength >> 8);
    Serial2.write(topiclength & 0xFF);
    Serial2.print(str);
    Serial2.write(0x1A);
    if (sendATcommand2("", "SEND OK", "SEND FAIL", 5000)) {
      Serial.println(F("PUBLISH PACKET SENT"));
      return 1;
    }
    else return 0;
  }
}

int MQTTsubscribe() {

  if (sendATcommand2("AT+CIPSEND", ">", "ERROR", 1000)) {

    memset(str, 0, 250);
    topiclength2 = strlen(MQTTTopic2);
    datalength = 2 + 2 + topiclength2 + 1;
    delay(1000);

    Serial2.write(0x82);
    X = datalength;
    do
    {
      encodedByte = X % 128;
      X = X / 128;
      // if there are more data to encode, set the top bit of this byte
      if ( X > 0 ) {
        encodedByte |= 128;
      }
      Serial2.write(encodedByte);
    }
    while ( X > 0 );
    Serial2.write(MQTTPacketID >> 8);
    Serial2.write(MQTTPacketID & 0xFF);
    Serial2.write(topiclength2 >> 8);
    Serial2.write(topiclength2 & 0xFF);
    Serial2.print(MQTTTopic2);
    Serial2.write(MQTTQOS);

    Serial2.write(0x1A);
    if (sendATcommand2("", "SEND OK", "SEND FAIL", 5000)) {
      Serial.println(F("SUBSCRIBE PACKET SENT"));
      return 1;
    }
    else return 0;
  }

}

boolean resetModem() {
  uint8_t answer = 0;
  digitalWrite(sim800lreset, LOW);
  delay(1000);
  digitalWrite(sim800lreset, HIGH);
  // checks if the module is started
  delay(3000);
  answer = sendATcommand("AT", "OK", 2000);
  if (answer == 1)sendATcommand("AT", "OK", 2000); // turn off the echo
  else if (answer == 0)
  {
    // power on pulse
    digitalWrite(sim800lreset, LOW);
    delay(1000);
    digitalWrite(sim800lreset, HIGH);

    // waits for an answer from the module
    int trials = 0;
    while (answer == 0) {
      trials++;
      // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT", "OK", 2000);
      if (trials == 5) {
        Serial.println(F("Gsm Start Fail"));
        //break;
        return false;
      }
    }
    sendATcommand("ATE0", "OK", 2000);// turn off the echo
  }
  else if (answer == 1)return true;
}



int8_t readServerResponse(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout) {
  unsigned long nowMillis = millis();
  Serial2.println(ATcommand);
  delay(3000);
 
  if (Serial2.available()) {
    while (char(Serial.read()) != 0x24) {
      if ((millis() - nowMillis) > 2000) {
        Serial.println("NO DATA RECEIVED FROM REMOTE");
        break;
      }
    }
    nowMillis=(millis());
    while (Serial2.available()) {
      Serial.print(char(Serial2.read()));
    }
  }

}


int8_t sendATcommand2(char* ATcommand, char* expected_answer1, char* expected_answer2, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  Serial2.flush();
  Serial2.println(ATcommand);    // Send the AT command
  //if(strstr(ATcommand, "AT+CIPSEND")!=NULL) Serial2.write(0x1A);

#ifdef dbg
  Serial.println(ATcommand);    // Send the AT command
#endif

  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    // if there are data in the UART input buffer, reads it and checks for the asnwer
    if (Serial2.available() != 0) {
      response[x] = Serial2.read();
      x++;
      // check if the desired answer 1  is in the response of the module
      if (strstr(response, expected_answer1) != NULL)
      {
        answer = 1;
        while (Serial.available()) {
          response[x] = Serial2.read();
          x++;
        }
      }
      // check if the desired answer 2 is in the response of the module
      else if (strstr(response, expected_answer2) != NULL)
      {
        answer = 2;
        while (Serial.available()) {
          response[x] = Serial2.read();
          x++;
        }
      }

    }
  }
  // Waits for the asnwer with time out
  while ((answer == 0) && ((millis() - previous) < timeout));
#ifdef dbg
  Serial.println(response);
#endif
  return answer;
}
/////////////////////


int8_t sendATcommand(char* ATcommand, char* expected_answer, unsigned int timeout) {

  uint8_t x = 0,  answer = 0;
  char response[500];
  unsigned long previous;
  char* str;
  uint8_t index = 0;

  memset(response, '\0', 100);    // Initialize the string

  delay(100);

  while ( Serial2.available() > 0) Serial2.read();   // Clean the input buffer

  Serial2.println(ATcommand);    // Send the AT command
#ifdef dbg
  Serial.println(ATcommand);    // Send the AT command
#endif


  x = 0;
  previous = millis();

  // this loop waits for the answer
  do {
    if (Serial2.available() != 0) {
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = Serial2.read();
      //Serial2.print(response[x]);
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL)
      {
        answer = 1;

      }
    }
  }
  // Waits for the asnwer with time out
  while ((answer == 0) && ((millis() - previous) < timeout));

#ifdef dbg
  Serial.println(response);    // Send the AT command
#endif
  return answer;
}





//void updateTime() {
//  sendATcommand("AT+CCLK?", "OK", 2000);
//}









int initTCP() {

  resetModem();
  sendATcommand2("ATE0", "OK", "ERROR", 2000);
  sendATcommand2("ATE0", "OK", "ERROR", 2000);

  delay(2000);


  Serial.println(F("Connecting to the network..."));

  while ( sendATcommand2("AT+CREG?", "+CREG: 0,1", "+CREG: 0,5", 1000) == 0 );
  delay(2000);


  if (sendATcommand2("AT+CIPMUX=0", "OK", "ERROR", 1000) == 1)      // Selects Single-connection mode

  {
    if (sendATcommand2("AT+CIPRXGET=2", "OK", "ERROR", 1000) == 1) { //RECEIVE DATA manually FROM THE REMOTE SERVER
      int8_t answer = 0;

      delay(1000);

      if (!(sendATcommand2("AT+CIPMODE=0", "OK", "ERROR", 1000) ))return 0;//srt non transparent mode for data sending
      delay(500);
      if (!(sendATcommand2("AT+CIPSRIP=0", "OK", "ERROR", 1000) ))return 0;//Do not show the prompt during receiving data from server
      delay(500);
      while (sendATcommand("AT+CGATT?", "+CGATT: 1", 5000) == 0 );
      delay(1000);

      // Waits for status IP INITIAL
      while (sendATcommand("AT+CIPSTATUS", "INITIAL", 5000) == 0 );
      delay(1000);

      snprintf(aux_str, sizeof(aux_str), "AT+CSTT=\"%s\",\"%s\",\"%s\"", __APN, __usrnm, __password);

      // Sets the APN, user name and password
      if (sendATcommand2(aux_str, "OK",  "ERROR", 30000) == 1)
      {

        // Waits for status IP START
        if (sendATcommand("AT+CIPSTATUS", "START", 500)  == 0 )
          delay(3000);

        // Brings Up Wireless Connection
        if (sendATcommand2("AT+CIICR", "OK", "ERROR", 30000) == 1)
        {

          // Waits for status IP GPRSACT
          while (sendATcommand("AT+CIPSTATUS", "GPRSACT", 500)  == 0 );
          delay(3000);

          // Gets Local IP Address
          if (sendATcommand2("AT+CIFSR", ".", "ERROR", 10000) == 1)
          {

            // Waits for status IP STATUS
            while (sendATcommand("AT+CIPSTATUS", "IP STATUS", 500)  == 0 );
            //delay(5000);
            delay(5000);

            Serial.println(F("Opening TCP"));
            snprintf(aux_str, sizeof(aux_str), "AT+CIPSTART=\"TCP\",\"%s\",\"%s\"", MQTTHost, MQTTPort);

            // Opens a TCP socket
            if (sendATcommand2(aux_str, "OK\r\n\r\nCONNECT", "CONNECT FAIL", 30000) == 1)
            {

              // Serial.println(F("Connected"));
              return 1;

            }

            else
            {
              Serial.println(F("Error opening the connection"));
              Serial.println(F("UNABLE TO CONNECT TO SERVER "));
              return 0;

            }

          }
          else
          {
            Serial.println(F("ERROR GETTING IP ADDRESS "));
            return 0;

          }
        }
        else
        {
          Serial.println(F("ERROR BRINGING UP WIRELESS CONNECTION"));
          return 0;
        }
      }
      else {
        Serial.println(F("Error setting the APN"));
        return 0;
      }


    }
    else
    {
      Serial.println(F("Error setting CIPRXGET"));
      return 0;
    }


  }
}