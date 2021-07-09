
/**
 * @brief created by Eng Mohamed Moustafa
 * whatsapp 01021853415
 * This will be a demo from larg project controlling cnc Aluminm cutting machine
 * 
 * here interfacing with hmi touch screen with uart communication 
 * so every touchable area will send me a key code (2byte) and store it in local address assigned on screen driver 
 * which make me able to ask for this address value "this value deleted screen if power runs out"
 * this code will be used to represet (page number , intruction to do)
 * ex: 0x5122 here i will use first byte to indicate page number
 * low byte as a code to call a function or to do something
 * 
 * those codes will be predefined and saved in struct and analysis its instuctions with pointer to function
 * Have main tasks want to do:
 * 1.analysis uart command recieved, all command will be recieved will indicate for(keycode, string, number)
 * 2.
 * 
 * 
 */

/**
 * @brief
 * notes to remeber and to avoid
 * errors codes and popup it in touch screen and make documenation for it
 * how to force display with uart command
 * @return ** void 
 * 
 */

#include <Arduino.h>
//#include <sdcard.h>
#include "SD.h"
//#include <mySD.h>
#include <SPI.h>
#include <CSV_Parser.h>
#include "All_Functions\Allfunctions.h"

#include <SoftwareSerial.h>

#define ESP8266 //ESP32 or ESP8266

#ifdef ESP8266
SoftwareSerial Serial2(D0, D1); //Rx,Tx
//For SD Card
#define SCK D5 //14
#define MISO D6 //12
#define MOSI D7 //13
#define SS D8 //15
#define CS D4 //17
#endif

#ifdef ESP32
SoftwareSerial Serial2(D0, D1); //Rx,Tx
//For SD Card
#define SCK 14
#define MISO 12
#define MOSI 13
#define SS 15
#endif

#define Serial_Debuging

#define _StartComm_Hbyte 0x5A
#define _StartComm_Lbyte 0xA5

//Hardware pins
const int chipSelect = 10; //for sd card ss

void Serial_init(void);
void TouchScreenEvent();
void DGUS_LED_Bright(byte bVal);
void DGUS_Beep(byte bTime);
void DGUS_Go_to_Picture(byte picID);
void DGUS_SendVal(int Address, int Value);

//functions declarations
void print1(void);
void print1(void) { return; }

//defining each command function
Screen Screen_codes[] =
    {
        {lang_Eng, print1},

        {
            material_Alum,
        },
        {
            material_UPVC,
        },
};
char *creatfile_PATH(char *material,
                     char *section,
                     char *company);

void setup()
{
  Serial_init();
  SPI.pins(SCK, MISO, MOSI, SS);
  delay(5000);
#ifdef Serial_Debuging
  Serial.print("Initializing SD card...");
#endif
  // see if the card is present and can be initialized:
  if (!SD.begin(CS))
  {
#ifdef Serial_Debuging
    Serial.println("Card failed, or not present");
#endif
    //while (1)
      ; // don't do anything more:
  }
#ifdef Serial_Debuging
  Serial.println("card initialized.");
#endif
char const *printedPATH = creatfile_PATH("mohame","moustafa","aly");

Serial.print("PATH");
Serial.println(printedPATH);

/*
  int company_rows = company_file.getRowsCount();

  if (section_name && sill_number)
  {
    for (int row = 0; row < company_file.getRowsCount(); row++)
    {
      Serial.print("row = ");
      Serial.print(row, DEC);
      Serial.print(", column_1 = ");
      Serial.print(section_name[row]);
      Serial.print(", column_2 = ");
      Serial.println(sill_number[row], DEC);
    }
  }
  else
  {
    Serial.println("At least 1 of the columns was not found, something went wrong.");
  }

  // output parsed values (allows to check that the file was parsed correctly)
  company_file.print(); // assumes that "Serial.begin()" was called before (otherwise it won't work)
  */
}

void loop()
{

  TouchScreenEvent(); //read sent packed and analysis it (keycode found go to relaed function)(changeglobal "keycode_val" var)
}

void Serial_init(void)
{
#ifdef Serial_Debuging
  Serial.begin(9600); //for serial debug
#endif
  Serial2.begin(115200); //for touch screen (RX2,TX2)(16,17)
  //screen_serial.begin(115200,0x800001c,16,17);//serial2
}

char *creatfile_PATH(char  *material,
                     char *section,
                     char *company)
{
  
  const char* dir_operator = "/";
  const char* extention = ".csv";
  char  CreatedPath[((strlen(material) + strlen(section) + strlen(company)) * sizeof(char))];
  strcpy(CreatedPath, dir_operator);
  strcat(CreatedPath, material);
  strcat(CreatedPath, dir_operator);
  strcat(CreatedPath, section);
  strcat(CreatedPath, dir_operator);
  strcat(CreatedPath, company);
  strcat(CreatedPath, extention);
  return CreatedPath;
};
void getCompany_param(char *material,
                      char *section,
                      char *company)
{ //save company parameter in global variables
  char *companyPATH = creatfile_PATH(material,
                                     section,
                                     company);
 
  company_file.readSDfile(companyPATH); // this wouldn't work if SD.begin wasn't called before
  
  //store in those arrays
  section_name = (char **)company_file["section_name"]; //string
  char *sill_number = (char *)company_file["sill_number"];

  frame_add_W = (int16_t *)company_file["frame_add_W"];
  frame_div_W = (char *)company_file["frame_div_W"];
  frame_add_H = (int16_t *)company_file["frame_add_H"];
  frame_div_H = (char *)company_file["frame_div_H"];

  sill_add_W = (int16_t *)company_file["sill_add_W"];
  sill_div_W = (char *)company_file["sill_div_W"];
  sill_add_H = (int16_t *)company_file["sill_add_H"];
  sill_div_H = (char *)company_file["sill_div_H"];

  net_add_W = (int16_t *)company_file["net_add_W"];
  net_div_W = (char *)company_file["net_div_W"];
  net_add_H = (int16_t *)company_file["net_add_H"];
  net_div_H = (char *)company_file["net_div_H"];

  clamp_add_W = (int16_t *)company_file["clamp_add_W"];
  clamp_div_W = (char *)company_file["clamp_div_W"];
  clamp_add_H = (int16_t *)company_file["clamp_add_H"];
  clamp_div_H = (char *)company_file["clamp_div_H"];

} //
void DGUS_LED_Bright(byte bVal) //Screen backlite set 0-0x40
{
  if (bVal > 0x40)
    bVal = 0x40;
  Serial2.write(_StartComm_Hbyte);
  Serial2.write(_StartComm_Lbyte);
  Serial2.write(0x03);
  Serial2.write(0x80);
  Serial2.write(0x01);
  Serial2.write(bVal);
}
void DGUS_Beep(byte bTime) // Beep generate bTime*10ms
{
  Serial2.write(_StartComm_Hbyte);
  Serial2.write(_StartComm_Lbyte);
  Serial2.write(0x03);
  Serial2.write(0x80);
  Serial2.write(0x02);
  Serial2.write(bTime);
}
void DGUS_Go_to_Picture(byte picID) // Display specific picture
{
  Serial2.write(_StartComm_Hbyte);
  Serial2.write(_StartComm_Lbyte);
  Serial2.write(0x03);
  Serial2.write(0x80); //0x80 write register,0x81 read register
  Serial2.write(0x03);
  Serial2.write(picID);
}
/*
byte picID DGUS_get_current_picID(void) // Know current picture ID 
{ 
  Serial2.write(_StartComm_Hbyte);
  Serial2.write(_StartComm_Lbyte);
  Serial2.write(0x03);
  Serial2.write(0x81);//0x80 write register,0x81 read register
  Serial2.write(0x03);
  Serial2.write(picID);
}
*/
void DGUS_SendVal(int Address, int Value) //Send Value for VP= Address to DGUS
{
  byte bAdrL, bAdrH, bValL, bValH;
  bAdrL = Address & 0xFF;
  bAdrH = (Address >> 8) & 0xFF;
  bValL = Value & 0xFF;
  bValH = (Value >> 8) & 0xFF;

  Serial2.write(0x5A);
  Serial2.write(0xA5);
  Serial2.write(0x05);
  Serial2.write(0x82);
  Serial2.write(bAdrH);
  Serial2.write(bAdrL);
  Serial2.write(bValH);
  Serial2.write(bValL);
}
void TouchScreenEvent()
{
  int iLenght;
  byte iCmd = 0, iData[255];
  int iAdr = 0, iVal = 0;
  if (Serial2.available() > 3)
  {
    if (Serial2.read() == _StartComm_Hbyte)
      if (Serial2.read() == _StartComm_Lbyte)
      {
        iLenght = Serial2.read();

        while (Serial2.available() < iLenght)
          ;               //Wait for all frame !!!! Blocking procedure
        while (iLenght--) //Compile all frame
        {
          iData[iLenght] = Serial2.read();
        }
        iCmd = iData[0];
        iAdr = (iData[1] << 8) + iData[2];
        switch (iCmd)
        {
        case 0x83:
          if (iAdr == Keycode_address)
          {
            iVal = (iData[3] << 8) + iData[4];
            keycode_value = iVal;
            for (checkAllKeyCodes) //call the function of returned keycode
            {
              if (keycode_value == Screen_codes[key].ActionName)
              {
                Screen_codes[key].KeyCodeInst();
                return;
              }
            }
          }
          break;

        default:
          break;
        }
      }
#ifdef Serial_Debuging
    Serial.print("CMD:");
    Serial.println(iCmd);
    Serial.print("VP:");
    Serial.println(iAdr);
    Serial.print("Value:");
    Serial.println(iVal);
#endif
  }
}

/**
 * Errors
 * company files  not found
 * no wifi connection
 * over load
 */