#ifndef _4_clienteSNMP_LCD
#define _4_clienteSNMP_LCD
// standard maximum SNMP UDP packet size = 484. 256 will be ok for my porpuses. Allows 32 bytes for community, 16 for oid and 64 for value from GetResponse
//#define UDP_TX_PACKET_MAX_SIZE 256
//interfaces in remote device
#define INTERFACES 24
//interface used by Arduino on remote device
#define SELFINTERFACE 22

#define DEBUG 0
//MAX BUFFER SIZES
#define MAX_COMMUNITY_SIZE 32
#define MAX_OID_SIZE 16
#define MAX_VALUE_SIZE 132

#define DELAY 300 // delay for buttons
//MODES. CHANGE TO 7 TO HIDE TEST MODE
#define MODES 9

typedef struct UDP_struct {
  byte mac[6];
  IPAddress local_IP;
  IPAddress remote_IP;
  unsigned int localPort; //  local port to listen on
} UDP_struct;

typedef struct response_struct {
  int error;
  unsigned long value;
} response_struct;


typedef struct SNMP_struct {
  int snmp_length;
  byte snmpVersion;
  char community[MAX_COMMUNITY_SIZE];
  byte community_length;
  unsigned long requestID;
  byte requestID_length;
  byte error;
  byte errorIndex;
  byte pduType;
  int pdu_length;
  int OID[MAX_OID_SIZE];
  byte OIDLength;
  byte valueType;
  unsigned long valueInt;
  char valueChar[MAX_VALUE_SIZE];
  byte varBindList_length;
  byte varBind_length;
}SNMP_struct;

// define some values used by the LCD panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define retraso 200
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#endif


