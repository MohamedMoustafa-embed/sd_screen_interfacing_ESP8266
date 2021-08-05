
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

//#define ESP8266 //ESP32 or ESP8266

#ifdef ESP8266
SoftwareSerial Serial2(D0, D1); //Rx,Tx
//For SD Card
#define SCK D5  //14
#define MISO D6 //12
#define MOSI D7 //13
#define SS D8   //15
#define CS D4   //17
#endif

//buttons
#define right_pin 9//s2
#define left_pin 10//s3


#ifdef ESP32
SoftwareSerial Serial2(D0, D1); //Rx,Tx
//For SD Card
#define SCK 14
#define MISO 12
#define MOSI 13
#define SS 15
#endif

#define dispCateg_colums 2
#define dispCateg_rows 5 
#define dispCateg_pages 3
#define dispCateg_startAddress 0x1500

#define dispCateg_secNameLenght 12
#define dispCateg_sillNameLenght 1

#define cutting_ist_rows 3

#define Serial_Debuging

#define _StartComm_Hbyte 0x5A
#define _StartComm_Lbyte 0xA5

char **section_name; //string
char *sill_number;

int16_t *frame_add_W;
char *frame_div_W;
int16_t *frame_add_H;
char *frame_div_H;

int16_t *sill_add_W;
char *sill_div_W;
int16_t *sill_add_H;
char *sill_div_H;

int16_t *net_add_W;
char *net_div_W;
int16_t *net_add_H;
char *net_div_H;

int16_t *clamp_add_W;
char *clamp_div_W;
int16_t *clamp_add_H;
char *clamp_div_H;

int categ_rows ;
int categ_colum;

void Serial_init(void);
void TouchScreenEvent();
void DGUS_LED_Bright(byte bVal);
void DGUS_Beep(byte bTime);
void DGUS_Go_to_Picture(byte picID);
void DGUS_SendVal(int Address, int Value);
void DGUS_SendSrting(int Address, String text);
void fill_dispCateg_table(void);
void display_Section_list(int start_page, int Start_addr);

typedef enum {
Deduction,
DirectCut,
WirelessControl,
}modes;


typedef enum {
  display_categ,//output caase
  operation,//input case
  
}tablePages;


//global varibals needed
int current_page = display_categ ;
bool lang_eng = 1;//1 english, 0 arabic
int machine_mode = Deduction;

bool material_alu = 1;//1 aluminum , 0 UPVC
bool sectionType_hinged = 1; //1 hinged , 0 sliding
String company_no="";//to store company name

//functions declarations
String creatfile_PATH(String material, String section_type, String company);
void getCompany_param(String material, String section_type, String company);
void pass_company_no (String company);
void fill_disp_categ_table();

void change_lang(void){if (lang_eng)lang_eng = 0;else lang_eng=1;}
void mode_deduc(void) { machine_mode=Deduction; }
void mode_direct(void) { machine_mode=DirectCut; }
void mode_wireless(void) { machine_mode=WirelessControl; }

void material_Alum_fn (void) {material_alu = 1;}
void material_UPVC_fn (void){material_alu =0;} 

void sectionType_Hinged_fn (void){sectionType_hinged =1;}
void sectionType_Sliding_fn(void){sectionType_hinged=0;} 

void  CompanyList_1_fn (void) { pass_company_no("01"); }
void  CompanyList_2_fn (void) { pass_company_no("02"); }
void  CompanyList_3_fn (void) { pass_company_no("03"); }
void  CompanyList_4_fn (void) { pass_company_no("04"); }
void  CompanyList_5_fn (void) { pass_company_no("05"); }
void  CompanyList_6_fn (void) { pass_company_no("06"); }

//defining each command function
Screen Screen_codes[] =
    {
        {lang_Eng, change_lang},
        {lang_Arb, change_lang},

        {main_Deduction, mode_deduc},
        {main_Directcut, mode_direct},
        {main_WirelessControl, mode_wireless},

        {material_Alum,material_Alum_fn},
        {material_UPVC,material_UPVC_fn},

        {sectionType_Hinged,sectionType_Hinged_fn},
        {sectionType_Sliding,sectionType_Sliding_fn},

        {CompanyList_1, CompanyList_1_fn},
        {CompanyList_2, CompanyList_2_fn},
        {CompanyList_3, CompanyList_3_fn},
        {CompanyList_4, CompanyList_4_fn},
        {CompanyList_5, CompanyList_5_fn},
        {CompanyList_6, CompanyList_6_fn},
//then display data read from sd card in section and sill table

};


void setup()
{
  Serial_init();
  SPI.pins(SCK, MISO, MOSI, SS);
  pinMode(right_pin,INPUT_PULLUP);
  pinMode(left_pin,INPUT_PULLUP);

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
  }
#ifdef Serial_Debuging
  Serial.println("card initialized.");
#endif

}

void loop()
{
  //TouchScreenEvent(); //read sent packed and analysis it (keycode found go to relaed function)(changeglobal "keycode_val" var)
}



void Serial_init(void)
{
#ifdef Serial_Debuging
  Serial.begin(9600); //for serial debug
#endif
  Serial2.begin(115200); //esp32 serial2 pins (RX2,TX2)(16,17)
}

String creatfile_PATH(String material, String section_type, String company)
{
#if 0
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
#endif

  String CreatedPath_S = "/" + material + "/" + section_type + "/" + company + ".csv";

  return CreatedPath_S;
};

void getCompany_param(String material, String section_type, String company)
{ //save company parameter in global variables
  String companyPATH_S = creatfile_PATH(material, section_type, company);
  char companyPATH[50];
  companyPATH_S.toCharArray(companyPATH, 50);

  CSV_Parser company_file(/*format*/ "scdcdcdcdcdcdcdcdc", /*has_header*/ true, /*delimiter*/ ',');
  company_file.readSDfile(companyPATH); // this wouldn't work if SD.begin wasn't called before

  //store in those arrays
  section_name = (char **)company_file["section_name"]; //string
  sill_number = (char *)company_file["sill_number"];

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

  categ_rows = company_file.getRowsCount();
  categ_colum = company_file.getColumnsCount();
  
#ifdef Serial_Debuging
  company_file.print();
#endif
}
void pass_company_no (String company){
  if(material_alu&sectionType_hinged) getCompany_param("ALUMINUM","Hinged",company);
  else if(material_alu&!sectionType_hinged)getCompany_param("ALUMINUM","Sliding",company);
  else if(!material_alu&sectionType_hinged)getCompany_param("UPVC","Hinged",company);
  else getCompany_param("UPVC","Sliding",company);

}
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
  //its defined in screen software that number consist of (3 int numbers + 1 decimal number ex: 999.9 ,screen need to recieve it as 9999 and it will divide it)
  byte bAdrL, bAdrH, bValL, bValH;
  bAdrL = Address & 0xFF;
  bAdrH = (Address >> 8) & 0xFF;
  Value *= 10; //for shifting decimal operator
  bValL = (int)Value & 0xFF;
  bValH = ((int)Value >> 8) & 0xFF;

  Serial2.write(_StartComm_Hbyte);
  Serial2.write(_StartComm_Lbyte);
  Serial2.write(0x05); //number of next bytes
  Serial2.write(0x82); //write command
  Serial2.write(bAdrH);
  Serial2.write(bAdrL);
  Serial2.write(bValH);
  Serial2.write(bValL);
}

void DGUS_SendSrting(int Address, String text)
{
  byte bAdrL, bAdrH;
  bAdrL = Address & 0xFF;
  bAdrH = (Address >> 8) & 0xFF;
  Serial2.write(_StartComm_Hbyte);
  Serial2.write(_StartComm_Lbyte);
  Serial2.write(((byte)text.length() & 0xFF)); //number of next bytes
  Serial2.write(0x82);                         //write command
  Serial2.write(bAdrH);
  Serial2.write(bAdrL);
  Serial2.print(text);
}

void TouchScreenEvent()
{
  int iLenght;
  byte iCmd = 0, iData[255];
  int iAdr = 0, iVal = 0;
  if (Serial2.available() > 3)
  {
    Serial.println("start");
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
          else if (iAdr == section_outer_lenght_addr)
          {
            iVal = (iData[3] << 8) + iData[4];
            section_outer_lenght_val = iVal;
#ifdef Serial_Debuging
            Serial.println(section_outer_lenght_val, DEC); //for serial debug
#endif
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
    Serial.println(iVal, DEC);
#endif
  }
}
#if 0
void display_Section_page(int start_page, int Start_addr)
{
  for (int i = ((sections_list_rows * start_page) - sections_list_rows); i <= ((start_page * sections_list_rows) - 1); i++)
  {
    //get data to display from global array      
    DGUS_SendSrting(Start_addr, section_name[i]);
    DGUS_SendVal(Start_addr + 1, sill_number[i]);
    Start_addr += section_list_secLenght;
  }
}
#endif

void fill_dispCateg_table(void)
{
  for (int row_no = 0; row_no <= categ_rows || row_no <= dispCateg_rows ; row_no++)
  {
    //get data to display from global array
    DGUS_SendSrting(dispCateg_startAddress + (row_no*(dispCateg_secNameLenght + dispCateg_sillNameLenght))  , section_name[row_no]);
    DGUS_SendVal(dispCateg_startAddress + (row_no*(2*dispCateg_secNameLenght + dispCateg_sillNameLenght)), sill_number[row_no]);
  }
}

void Cutting_list_Handling()
{
}


/**
 * Errors
 * company files  not found
 * no wifi connection
 * over load
 */