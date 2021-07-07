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


#define checkAllKeyCodes    \
  int key = 0;       \
  key <= enums_max_counter; \
  key++ //for forloop to avoid long writing in fo loop

typedef enum
{
  //pagename_keycode action == keycode (page position + instruction no.)
  lang_Eng = 0x0200,
  lang_Arb,

  main_back = 0x0300,
  main_deduction,
  main_directcut,
  main_settings,
  main_instruction,

  material_Alum = 0x0500,
  material_UPVC = 0x0501,

  AlumCompanies_1 = 0x0601,
  AlumCompanies_2,
  AlumCompanies_3,
  AlumCompanies_4,
  AlumCompanies_5,
  AlumCompanies_6,

  UPVCCompanies_1 = 0x0701,
  UPVCCompanies_2,
  UPVCCompanies_3,
  UPVCCompanies_4,
  UPVCCompanies_5,
  UPVCCompanies_6,

  enums_max_counter //used to stop iteration
} pagename_keycodeActionName;
typedef struct
{
  pagename_keycodeActionName ActionName;
  void (*KeyCodeInst)(void); //executing intructions related to every code
} Screen;


int keycode_val = 0;
int Keycode_addr = 0X0100;
int keycode_inst = 0;

int data_in_val = 0;
int data_in_addr = 0X0200;
int data_in_inst = 0;



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


