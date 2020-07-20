/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

enum DGUSLCD_Screens : uint8_t {
  DGUSLCD_SCREEN_BOOT = 0,
  DGUSLCD_SCREEN_MAIN = 45,
  DGUSLCD_SCREEN_SETTING = 63,
  DGUSLCD_SCREEN_PRINTFILE = 46,
  DGUSLCD_SCREEN_TEMP_CONTROL = 57,
  DGUSLCD_SCREEN_STATUS = 53,
  DGUSLCD_SCREEN_STATUS_PAUSED = 54,
  DGUSLCD_SCREEN_MANUALMOVE = 71,
  DGUSLCD_SCREEN_MANUALEXTRUDE= 42,
  DGUSLCD_SCREEN_FANANDFEEDRATE = 44,
  DGUSLCD_SCREEN_FLOWRATES = 46,
  DGUSLCD_SCREEN_SDFILELIST = 46,
  DGUSLCD_SCREEN_SDPRINTMANIPULATION = 52,
  DGUSLCD_SCREEN_POWER_LOSS = 100,
  DGUSLCD_SCREEN_PREHEAT=120,
  DGUSLCD_SCREEN_UTILITY=110,
  DGUSLCD_SCREEN_FILAMENT_HEATING=146,
  DGUSLCD_SCREEN_FILAMENT_LOADING=148,
  DGUSLCD_SCREEN_FILAMENT_UNLOADING=158,
  DGUSLCD_SCREEN_SDPRINTTUNE = 170,
  DGUSLCD_SCREEN_CONFIRM = 240,
  DGUSLCD_SCREEN_KILL = 250, ///< Kill Screen. Must always be 250 (to be able to display "Error wrong LCD Version")
  DGUSLCD_SCREEN_WAITING = 251,
  DGUSLCD_SCREEN_POPUP = 252,  ///< special target, popup screen will also return this code to say "return to previous screen"
  DGUSLDC_SCREEN_UNUSED = 255
};

// Firmware version on the boot screen.
constexpr uint16_t VP_MARLIN_VERSION = 0x106A;
constexpr uint8_t VP_MARLIN_VERSION_LEN = sizeof(SHORT_BUILD_VERSION);   // there is more space on the display, if needed.
constexpr uint16_t VP_MACHINE_NAME = 0x1060;
constexpr uint8_t VP_MACHINE_NAME_LEN = sizeof(CUSTOM_MACHINE_NAME);   // there is more space on the display, if needed.
constexpr uint16_t VP_PrinterSize = 0x1074;

// Temperature on main screen
constexpr uint16_t VP_T_E0_Is = 0x1036;  // 4 Byte Integer
constexpr uint16_t VP_T_E0_Set = 0x1034; // 2 Byte Integer
constexpr uint16_t VP_T_Bed_Is = 0x103C;  // 4 Byte Integer
constexpr uint16_t VP_T_Bed_Set = 0x103A; // 2 Byte Integer
constexpr uint16_t VP_MAIN_SCREEN = 0x1002;
constexpr uint16_t VP_SETTING_SCREEN = 0x103E;
constexpr uint16_t VP_LEVELING_SCREEN = 0x1040;
constexpr uint16_t VP_SETTEMP_SCREEN = 0x1032;


// Actual Position
constexpr uint16_t VP_MOVE_X = 0x1048;  // 2 Byte Integer
constexpr uint16_t VP_MOVE_Y = 0x104A;
constexpr uint16_t VP_MOVE_Z = 0x104C;
constexpr uint16_t VP_HOME_XY = 0x1046;

constexpr uint16_t VP_PROGRESS = 0x1016;
constexpr uint16_t VP_TIME_HOUR = 0x1010;
constexpr uint16_t VP_TIME_MIN = 0x1012;

// Bed Leveling
constexpr uint16_t VP_BED_AUTO_MEASURE = 0x1044;  // 2 Byte Integer
constexpr uint16_t VP_BED_MEASUTRMENT = 0x1100;

// PIDs
constexpr uint16_t VP_E0_PID_P = 0x3700; // at the moment , 2 byte unsigned int , 0~1638.4
constexpr uint16_t VP_E0_PID_I = 0x3702;
constexpr uint16_t VP_E0_PID_D = 0x3704;
constexpr uint16_t VP_BED_PID_P = 0x3710;
constexpr uint16_t VP_BED_PID_I = 0x3712;
constexpr uint16_t VP_BED_PID_D = 0x3714;

// Status screen
constexpr uint16_t VP_ADJUST_BTN = 0x3714;

// Display Memory layout used (T5UID)
// Except system variables this is arbitrary, just to organize stuff....

// 0x0000 .. 0x0FFF  -- System variables and reserved by the display
// 0x1000 .. 0x1FFF  -- Variables to never change location, regardless of UI Version
// 0x2000 .. 0x2FFF  -- Controls (VPs that will trigger some action)
// 0x3000 .. 0x4FFF  -- Marlin Data to be displayed
// 0x5000 ..         -- SPs (if we want to modify display elements, e.g change color or like) -- currently unused

// As there is plenty of space (at least most displays have >8k RAM), we do not pack them too tight,
// so that we can keep variables nicely together in the address space.

// UI Version always on 0x1000...0x1002 so that the firmware can check this and bail out.
constexpr uint16_t VP_UI_VERSION_MAJOR = 0x1000;  // Major -- incremented when incompatible
constexpr uint16_t VP_UI_VERSION_MINOR = 0x1001;  // Minor -- incremented on new features, but compatible
constexpr uint16_t VP_UI_VERSION_PATCH = 0x1002;  // Patch -- fixed which do not change functionality.
constexpr uint16_t VP_UI_FLAVOUR       = 0x1010;  // lets reserve 16 bytes here to determine if UI is suitable for this Marlin. tbd.

// Storage space for the Killscreen messages. 0x1100 - 0x1200 . Reused for the popup.
constexpr uint16_t VP_MSGSTR1 = 0x1100;
constexpr uint8_t VP_MSGSTR1_LEN = 0x20;  // might be more place for it...
constexpr uint16_t VP_MSGSTR2 = 0x1140;
constexpr uint8_t VP_MSGSTR2_LEN = 0x20;
constexpr uint16_t VP_MSGSTR3 = 0x1180;
constexpr uint8_t VP_MSGSTR3_LEN = 0x20;
constexpr uint16_t VP_MSGSTR4 = 0x11C0;
constexpr uint8_t VP_MSGSTR4_LEN = 0x20;

// Screenchange request for screens that only make sense when printer is idle.
// e.g movement is only allowed if printer is not printing.
// Marlin must confirm by setting the screen manually.
constexpr uint16_t VP_SCREENCHANGE_ASK = 0x2000;
constexpr uint16_t VP_SCREENCHANGE = 0x2001;   // Key-Return button to new menu pressed. Data contains target screen in low byte and info in high byte.
constexpr uint16_t VP_TEMP_ALL_OFF = 0x2002;   // Turn all heaters off. Value arbitrary ;)=
constexpr uint16_t VP_SCREENCHANGE_WHENSD = 0x2003; // "Print" Button touched -- go only there if there is an SD Card.

constexpr uint16_t VP_CONFIRMED = 0x2010; // OK on confirm screen.

// Buttons on the SD-Card File listing.
constexpr uint16_t VP_SD_ScrollEvent = 0x2020; // Data: 0 for "up a directory", numbers are the amount to scroll, e.g -1 one up, 1 one down
constexpr uint16_t VP_SD_FileSelected = 0x2022; // Number of file field selected.
constexpr uint16_t VP_SD_FileSelectConfirm = 0x2024; // (This is a virtual VP and emulated by the Confirm Screen when a file has been confirmed)

constexpr uint16_t VP_SD_ResumePauseAbort = 0x2026; // Resume(Data=0), Pause(Data=1), Abort(Data=2) SD Card prints
constexpr uint16_t VP_SD_AbortPrintConfirmed = 0x2028; // Abort print confirmation (virtual, will be injected by the confirm dialog)
constexpr uint16_t VP_SD_Print_Setting = 0x2040;
constexpr uint16_t VP_SD_Print_LiveAdjustZ = 0x2050; // Data: 0 down, 1 up

// Controls for movement (we can't use the incremental / decremental feature of the display at this feature works only with 16 bit values
// (which would limit us to 655.35mm, which is likely not a problem for common setups, but i don't want to rule out hangprinters support)
// A word about the coding: The VP will be per axis and the return code will be an signed 16 bit value in 0.01 mm resolution, telling us
// the relative travel amount t he user wants to do. So eg. if the display sends us VP=2100 with value 100, the user wants us to move X by +1 mm.

constexpr uint16_t VP_MOVE_E0 = 0x2110;
constexpr uint16_t VP_MOVE_E1 = 0x2112;
//constexpr uint16_t VP_MOVE_E2 = 0x2114;
//constexpr uint16_t VP_MOVE_E3 = 0x2116;
//constexpr uint16_t VP_MOVE_E4 = 0x2118;
//constexpr uint16_t VP_MOVE_E5 = 0x211A;
constexpr uint16_t VP_HOME_ALL = 0x2120;
constexpr uint16_t VP_MOTOR_LOCK_UNLOK = 0x2130;

// Power loss recovery
constexpr uint16_t VP_POWER_LOSS_RECOVERY = 0x2180;

// Fan Control Buttons , switch between "off" and "on"
constexpr uint16_t VP_FAN0_CONTROL = 0x2200;
constexpr uint16_t VP_FAN1_CONTROL = 0x2202;
//constexpr uint16_t VP_FAN2_CONTROL = 0x2204;
//constexpr uint16_t VP_FAN3_CONTROL = 0x2206;

// Heater Control Buttons , triged between "cool down" and "heat PLA" state
constexpr uint16_t VP_E0_CONTROL = 0x2210;
constexpr uint16_t VP_E1_CONTROL = 0x2212;
//constexpr uint16_t VP_E2_CONTROL = 0x2214;
//constexpr uint16_t VP_E3_CONTROL = 0x2216;
//constexpr uint16_t VP_E4_CONTROL = 0x2218;
//constexpr uint16_t VP_E5_CONTROL = 0x221A;
constexpr uint16_t VP_BED_CONTROL = 0x221C;

// Preheat
constexpr uint16_t VP_E0_BED_PREHEAT = 0x2220;
constexpr uint16_t VP_E1_BED_CONTROL = 0x2222;
//constexpr uint16_t VP_E2_BED_CONTROL = 0x2224;
//constexpr uint16_t VP_E3_BED_CONTROL = 0x2226;
//constexpr uint16_t VP_E4_BED_CONTROL = 0x2228;
//constexpr uint16_t VP_E5_BED_CONTROL = 0x222A;

// Filament load and unload
constexpr uint16_t VP_E0_FILAMENT_LOAD_UNLOAD = 0x2300;
constexpr uint16_t VP_E1_FILAMENT_LOAD_UNLOAD = 0x2302;

// Settings store , reset
constexpr uint16_t VP_SETTINGS = 0x2400;

// PID autotune
constexpr uint16_t VP_PID_AUTOTUNE_E0 = 0x2410;
//constexpr uint16_t VP_PID_AUTOTUNE_E1 = 0x2412;
//constexpr uint16_t VP_PID_AUTOTUNE_E2 = 0x2414;
//constexpr uint16_t VP_PID_AUTOTUNE_E3 = 0x2416;
//constexpr uint16_t VP_PID_AUTOTUNE_E4 = 0x2418;
//constexpr uint16_t VP_PID_AUTOTUNE_E5 = 0x241A;
constexpr uint16_t VP_PID_AUTOTUNE_BED = 0x2420;

// Place for status messages.
constexpr uint16_t VP_M117 = 0x3020;
constexpr uint8_t VP_M117_LEN = 0x20;

constexpr uint16_t VP_Flowrate_E0 = 0x3090; // 2 Byte Integer
constexpr uint16_t VP_Flowrate_E1 = 0x3092; // 2 Byte Integer

constexpr uint16_t VP_Fan0_Percentage = 0x3100;  // 2 Byte Integer (0..100)
constexpr uint16_t VP_Fan1_Percentage = 0x33A2;  // 2 Byte Integer (0..100)

constexpr uint16_t VP_Feedrate_Percentage = 0x3102; // 2 Byte Integer (0..100)
constexpr uint16_t VP_PrintProgress_Percentage = 0x3104; // 2 Byte Integer (0..100)

constexpr uint16_t VP_PrintTime = 0x3106;
constexpr uint16_t VP_PrintTime_LEN = 10;

constexpr uint16_t VP_PrintAccTime = 0x3160;
constexpr uint16_t VP_PrintAccTime_LEN = 32;

constexpr uint16_t VP_PrintsTotal = 0x3180;
constexpr uint16_t VP_PrintsTotal_LEN = 16;


constexpr uint16_t VP_EPos = 0x3120;  // 4 Byte Fixed point number; format xxx.yy

// SDCard File Listing
constexpr uint16_t VP_SD_FileName_LEN = 32; // LEN is shared for all entries.
constexpr uint16_t DGUS_SD_FILESPERSCREEN = 5; // FIXME move that info to the display and read it from there.
constexpr uint16_t VP_SD_FileName0 = 0x3200;
constexpr uint16_t VP_SD_FileName1 = 0x3220;
constexpr uint16_t VP_SD_FileName2 = 0x3240;
constexpr uint16_t VP_SD_FileName3 = 0x3260;
constexpr uint16_t VP_SD_FileName4 = 0x3280;

constexpr uint16_t VP_SD_Print_ProbeOffsetZ = 0x32A0; //
constexpr uint16_t VP_SD_Print_Filename = 0x32C0; //

// Fan status
constexpr uint16_t VP_FAN0_STATUS = 0x3300;
constexpr uint16_t VP_FAN1_STATUS = 0x3302;
//constexpr uint16_t VP_FAN2_STATUS = 0x3304;
//constexpr uint16_t VP_FAN3_STATUS = 0x3306;

// Heater status
constexpr uint16_t VP_E0_STATUS = 0x3310;
//constexpr uint16_t VP_E1_STATUS = 0x3312;
//constexpr uint16_t VP_E2_STATUS = 0x3314;
//constexpr uint16_t VP_E3_STATUS = 0x3316;
//constexpr uint16_t VP_E4_STATUS = 0x3318;
//constexpr uint16_t VP_E5_STATUS = 0x331A;
constexpr uint16_t VP_BED_STATUS = 0x331C;

constexpr uint16_t VP_MOVE_OPTION = 0x3400;

// Step per mm
constexpr uint16_t VP_X_STEP_PER_MM = 0x3600; // at the moment , 2 byte unsigned int , 0~1638.4
//constexpr uint16_t VP_X2_STEP_PER_MM = 0x3602;
constexpr uint16_t VP_Y_STEP_PER_MM = 0x3604;
//constexpr uint16_t VP_Y2_STEP_PER_MM = 0x3606;
constexpr uint16_t VP_Z_STEP_PER_MM = 0x3608;
//constexpr uint16_t VP_Z2_STEP_PER_MM = 0x360A;
constexpr uint16_t VP_E0_STEP_PER_MM = 0x3610;
//constexpr uint16_t VP_E1_STEP_PER_MM = 0x3612;
//constexpr uint16_t VP_E2_STEP_PER_MM = 0x3614;
//constexpr uint16_t VP_E3_STEP_PER_MM = 0x3616;
//constexpr uint16_t VP_E4_STEP_PER_MM = 0x3618;
//constexpr uint16_t VP_E5_STEP_PER_MM = 0x361A;

// Wating screen status
constexpr uint16_t VP_WAITING_STATUS = 0x3800;

// SPs for certain variables...
// located at 0x5000 and up
// Not used yet!
// This can be used e.g to make controls / data display invisible
constexpr uint16_t SP_T_E0_Is = 0x5000;
constexpr uint16_t SP_T_E0_Set = 0x5010;
constexpr uint16_t SP_T_E1_Is = 0x5020;
constexpr uint16_t SP_T_Bed_Is = 0x5030;
constexpr uint16_t SP_T_Bed_Set = 0x5040;
