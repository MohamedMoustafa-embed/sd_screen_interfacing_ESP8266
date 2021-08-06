/**
 * @file Allfunctions.h
 * @author your name (you@domain.com)
 * @brief will contain all functions needed in the main code to avoid huge code of lines
 * @version 0.1
 * @date 2021-07-04
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <Arduino.h>
#include "HardwareSerial.h"
//#include <sdcard.h>
#include "SD.h"
//#include <mySD.h>
#include <SPI.h>
#include <CSV_Parser.h>
/*
#define checkAllKeyCodes    \
  int key = 0;              \
  key <= enums_max_counter; \
  key++ //for forloop to avoid long writing in fo loop
*/
typedef enum
{
  //pagename_keycode action == keycode (page position + instruction no.)
  lang_Eng = 0x0600,
  lang_Arb,

  main_Back = 0x0700,
  main_settings,
  main_Deduction,
  main_WirelessControl,
  main_Directcut,
  
  sectionType_Hinged = 0x0900,
  sectionType_Sliding,

  material_UPVC = 0x1000,
  material_Alum ,
  
  CompanyList_1 = 0x1200,
  CompanyList_2 ,
  CompanyList_3 ,
  CompanyList_4 ,
  CompanyList_5 ,
  CompanyList_6 ,

  DispCateg_chooseRow = 0x0901,
  

  Measure_SaveOuterLenght = 0x1801,

  enums_max_counter //used to stop iteration

} pagename_keycodeActionName;


typedef struct
{
  pagename_keycodeActionName ActionName;
  void (*KeyCodeInst)(void); //executing intructions related to every code
} Screen;



int keycode_value = 0;
const int Keycode_address = 0X0100;
int keycode_instruction = 0;

//data may be outerlenght or W,L array of "operation list"
float section_outer_lenght_val = 0;
const int section_outer_lenght_addr = 0X1800;
int section_outer_lenght_inst = 0;

//will be sent to screen
float section_width_val = 0;
const int section_width_addr = 0x0500;

float section_inner_val = 0;
const int section_inner_addr = 0x0300;






/**csv file header data>
      *section_name,sill_number,frame_add_W,frame_div_W,frame_add_H,frame_div_H,sill_add_W,sill_div_W,sill_add_H,sill_div_H,net_add_W,net_div_W,net_add_H,net_div_H,clamp_add_W,clamp_div_W,clamp_add_H,clamp_div_H
      *section_name,      "string>s"
      *sill_number,   "char>c"
      "will note exceed "
      "note write it in mm " max 127 mm
      *sill_add_H, "int16_t>d"max +/- 32767 mm 
      sill_div_L, "char>c"max127
      sill_add_W, "int16_t>d"max +/- 32767 mm
      sill_div_W,   "char>c"max127
      format example "scdcdc"
      *frame_add_H,frame_div_L,frame_add_W,frame_div_W,net_add_H,
      *net_div_L,net_add_W,net_div_W,
      *clamp_add_H,clamp_div_L,clamp_add_W,clamp_div_W
*/

/*
  /format/
 		 Acceptable format types are:  
			s - string  (C-like string, not a "String" Arduino object, just a char pointer, terminated by 0)   
			f - float    
			L - int32_t (32-bit signed value, can't be used for values over 2147483647)   
			d - int16_t (16-bit signed value, can't be used for values over 32767)    
			c - char    (8-bit signed value, can't be used for values over 127)   
			x - hex     (stored as int32_t)   
			"-" (dash character) means that value is unused/not-parsed (this way memory won't be allocated for values from that column)
*/


extern char **section_name; //string
extern char *sill_number;

extern int16_t *frame_add_W;
extern char *frame_div_W;
extern int16_t *frame_add_H;
extern char *frame_div_H;

extern int16_t *sill_add_W;
extern char *sill_div_W;
extern int16_t *sill_add_H;
extern char *sill_div_H;

extern int16_t *net_add_W;
extern char *net_div_W;
extern int16_t *net_add_H;
extern char *net_div_H;

extern int16_t *clamp_add_W;
extern char *clamp_div_W;
extern int16_t *clamp_add_H;
extern char *clamp_div_H;

/**
 * char    **section_name   = (char**)cp["section_name"];	//string
char    *sill_number = (char*)cp["sill_number"];

int16_t *frame_add_W = (int16_t*)cp["frame_add_W"];
char    *frame_div_W = (char*)cp["frame_div_W"];
int16_t *frame_add_H = (int16_t*)cp["frame_add_H"];
char    *frame_div_H = (char*)cp["frame_div_H"];

int16_t *sill_add_W = (int16_t*)cp["sill_add_W"];
char    *sill_div_W = (char*)cp["sill_div_W"];
int16_t *sill_add_H = (int16_t*)cp["sill_add_H"];
char    *sill_div_H = (char*)cp["sill_div_H"];

int16_t *net_add_W = (int16_t*)cp["net_add_W"];
char    *net_div_W = (char*)cp["net_div_W"];
int16_t *net_add_H = (int16_t*)cp["net_add_H"];
char    *net_div_H = (char*)cp["net_div_H"];

int16_t *clamp_add_W = (int16_t*)cp["clamp_add_W"];
char    *clamp_div_W = (char*)cp["clamp_div_W"];
int16_t *clamp_add_H = (int16_t*)cp["clamp_add_H"];
char    *clamp_div_H = (char*)cp["clamp_div_H"];
 * 
 */
#if 0
HardwareSerial Debug_Serial(0);
HardwareSerial screen_serial(2) ;

void extern Serial_init(void);
void extern TouchScreenEvent();


void extern DGUS_LED_Bright(byte bVal);
void extern DGUS_Beep(byte bTime);
void extern DGUS_Go_to_Picture(byte picID);
void extern DGUS_SendVal(int Address, int Value);
#endif
