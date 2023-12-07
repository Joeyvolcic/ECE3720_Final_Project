#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */

#define MAX_BUFFER_SIZE 100

//Initialize serial communication
void Serial0_begin(unsigned int baud_rate);
void GPS_begin(unsigned int baud_rate);

//Serial Communication functions
void SerCom0(unsigned char data);
void SerCom0_string(unsigned char *data);
unsigned char SerCom1_Receive(void);
void SerCom1_Send(unsigned char *data);
unsigned char SerCom1(void);
unsigned char* NMEA_sentacne(void);


//Data processing
unsigned int findComma(unsigned char *data, int pos);
unsigned char fix(unsigned char *data);
unsigned char* lat(unsigned char *data);
unsigned char* lon(unsigned char *data);



//General purpose functions
void MSDelay(unsigned int);

//Global variables
unsigned char sentence[100];
unsigned char latitude[50];
unsigned int longatude[50];

unsigned char* NMEA;
unsigned char receivedChar;
size_t charCount = 0;

unsigned char sample[] = "$GPGGA,202410.000,4042.6000,N,07400.4858,W,1,4,3.14,276.7,M,-34.2,M,,*63";
unsigned char *samplePointer = sample;

void main(void) {

  //Initializing Serial Communication
  Serial0_begin(9600);
  GPS_begin(9600);
  
  SerCom0_string((unsigned char*)"PMTK_SET_NMEA_OUTPUT_RMCGGA\r\n");
  SerCom0_string((unsigned char*)"PMTK_SET_NMEA_UPDATE_1HZ\r\n");
  
  // Sets up the communication so that it outputs RMGGA
  SerCom1_Send((unsigned char*)"$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"); 
  //Sets the update rate to 1hz
  SerCom1_Send((unsigned char*)"$PMTK220,1000"); 
  
  
  while(1){  
    NMEA = NMEA_sentacne();
    if(NMEA[5] == 'A'){
      SerCom0_string(NMEA);
      SerCom0(fix(NMEA));
      SerCom0_string((unsigned char*)"\r\n");
      if (fix(NMEA) == 'T'){
        //Displays Data on the screen
      }

    }
  }
}


unsigned char* lat(unsigned char *data){
  //returns the latitude in DDDD.DDDDD //
  int i;
  unsigned int pos_start = findComma(data, 3);
  unsigned int pos_end = findComma(data, 4);
  float number;
  float digit_multiplier = 10.0;
  
  
  for(i = pos_start + 2; i < pos_end; i++){
    if (data[i] != '.'){
      number = number + ((data[i] - 48) * digit_multiplier);
      digit_multiplier = digit_multiplier / 10;
    }           
  } 
  
  number = number / 60.0; 
  
  for(i = pos_start + 2; i < pos_end; i++){
    if (data[i] != '.'){
      latitude[(i - pos_start)] = (int)(number/10) + 48;
      number = number - (int)(number/10);
      number = number * 10;
    } else {
      latitude[(i - pos_start)] = ('.');
    }
  }   
  return latitude;  
}

unsigned char* lon(unsigned char *data){
  //returns the longitude in DDDD.DDDDD //
  int i;
  unsigned int pos_start = findComma(data, 5);
  unsigned int pos_end = findComma(data, 6);
  float number;
  float digit_multiplier = 10.0;
  
  for(i = pos_start + 2; i < pos_end; i++){
    if (data[i] != '.'){
      number = number + ((data[i] - 48) * digit_multiplier);
      digit_multiplier = digit_multiplier / 10;
    }    
         
  } //mm.mm -> dd.dd
  
  number = number / 60.0; // converted to dd.ddddd
  
  for(i = pos_start + 2; i < pos_end; i++){
    if (data[i] != '.'){
      longatude[(i - pos_start)] = (int)(number/10) + 48;
      number = number - (int)(number/10);
      number = number * 10;
    } else {
      longatude[(i - pos_start)] = ('.');
    }
  }   
  return latitude;  
}


unsigned char fix(unsigned char *data){
// Retruns T if the GPS has found a fix //
  unsigned int fixPos = findComma(data, 7);
  if (fixPos  != 101){  
    if ((data[fixPos + 1] + data[fixPos + 2]) != '0' ){
         return 'T';
       } else {
         return 'F'; 
    }
    return 'I';
  }
}

unsigned int findComma(unsigned char *data, int pos){
// finds the location of a comma seperator //
// returns a multi digit number needs to be split to print //
   int commaCount = 0;
   int position = 0; 
 
   while (data[position] != '\0') {
          if (data[position] == ',') {
              commaCount++;
              if (commaCount == pos) {
                  // Found the nth comma
                  return position;
              }
          }
          position++;
      }
      return 101; //out of range
}

unsigned char* NMEA_sentacne(void){    
// Captures NMEA sentances as an array //   
   do {
        while(!(SCI1SR1 & 0x20));
        receivedChar = SCI1DRL;
        sentence[charCount] = receivedChar;
        charCount++;

        // Check if the buffer is full to avoid buffer overflow
        if (charCount >= MAX_BUFFER_SIZE - 1) {
            break;
        }

    } while (receivedChar != '\n');

    // Null-terminate the string
    sentence[charCount] = '\0';
    charCount = 0;
    
    return sentence;
}

void SerCom0(unsigned char data){
// sends a char over serial port 0 //
        while(!(SCI0SR1 & 0x80));
        SCI0DRL = data;
  
}

void SerCom0_string(unsigned char *data){
// send a string of chars over serial port 0 //
    while (!(SCI0SR1 & 0x80));
    while (*data != '\0') {
        while (!(SCI0SR1 & 0x80));
        SCI0DRL = *data;
        data++;
    }
}

unsigned char SerCom1_Receive(){
// returns the recieved data char from serial communication //
        while(!(SCI1SR1 & 0x20));
        return SCI1DRL; 
}

void SerCom1_Send(unsigned char *data) {
// send a string of chars over serial port 1 //
    while (!(SCI1SR1 & 0x80));
    while (*data != '\0') {
        while (!(SCI1SR1 & 0x80));
        SCI1DRL = *data;
        data++;
    }
    
    //while (!(SCI1SR1 & 0x80));
    //SCI1DRL = 13;  // Carriage return
    
    //while (!(SCI1SR1 & 0x80));
    //SCI1DRL = 10;  // Line feed  
}

void MSDelay(unsigned int t){
// 1ms delay for t //
   unsigned int i,j;
   j = 0;
   for(i=0;i<t;i++){
   TC6 = TCNT + 187;
   while (!(TFLG1 & TFLG1_C6F_MASK));
   }
}

void Serial0_begin(unsigned int baud_rate){
// sets up the serial communication registers //
    int baud = (int)(250000 / baud_rate);
    SCI0BDH = (baud >> 8) & 0xFF; 
    SCI0BDL = baud & 0xFF; ;    
    SCI0CR1 = 0;
    SCI0CR2 = 0x0C;   
}

void GPS_begin(unsigned int baud_rate) {
//sets up gps serial communication register //
    int baud = (int)(250000 / baud_rate);
    SCI1BDH = (baud >> 8) & 0xFF; 
    SCI1BDL = baud & 0xFF;         
    SCI1CR1 = 0;
    SCI1CR2 = 0x0C;
     
}

