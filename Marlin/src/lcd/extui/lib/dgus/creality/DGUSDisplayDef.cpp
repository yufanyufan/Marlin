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

/* DGUS implementation written by coldtobi in 2019 for Marlin */

#include "../../../../../inc/MarlinConfigPre.h"

#if ENABLED(DGUS_LCD_UI_CREALITY)

#include "../DGUSDisplayDef.h"
#include "../DGUSDisplay.h"
#include "../DGUSScreenHandler.h"

#include "../../../../../module/temperature.h"
#include "../../../../../module/motion.h"
#include "../../../../../module/planner.h"

#include "../../../../ultralcd.h"
#include "../../../ui_api.h"

#if ENABLED(DGUS_UI_MOVE_DIS_OPTION)
  uint16_t distanceToMove = 10;
#endif

using namespace ExtUI;

const uint16_t VPList_Boot[] PROGMEM = {
  VP_MACHINE_NAME,
  VP_MARLIN_VERSION,
  VP_PrinterSize,
  0x0000
};

const uint16_t VPList_Main[] PROGMEM = {
  /* VP_M117, for completeness, but it cannot be auto-uploaded. */
  VP_T_E0_Is,
  VP_T_E0_Set,
  VP_T_Bed_Is,
  VP_T_Bed_Set,
  0x0000
};

const uint16_t VPList_Status[] PROGMEM = {
  VP_T_E0_Is,
  VP_T_E0_Set,
  VP_T_Bed_Is,
  VP_T_Bed_Set,
  VP_MOVE_X,
  VP_MOVE_Y,
  VP_MOVE_Z,
  VP_PROGRESS,
  0x0000
};

const uint16_t VPList_Status2[] PROGMEM = {
  /* VP_M117, for completeness, but it cannot be auto-uploaded */
  #if HOTENDS >= 1
    VP_Flowrate_E0,
  #endif
  #if HOTENDS >= 2
    VP_Flowrate_E1,
  #endif
  VP_PrintProgress_Percentage,
  VP_PrintTime,
  0x0000
};

const uint16_t VPList_FanAndFeedrate[] PROGMEM = {
  VP_Feedrate_Percentage, VP_Fan0_Percentage,
  0x0000
};

const uint16_t VPList_SD_FlowRates[] PROGMEM = {
  VP_Flowrate_E0, VP_Flowrate_E1,
  0x0000
};

const uint16_t VPList_SDFileList[] PROGMEM = {
  VP_SD_FileName0, VP_SD_FileName1, VP_SD_FileName2, VP_SD_FileName3, VP_SD_FileName4,
  0x0000
};

const uint16_t VPList_SD_PrintManipulation[] PROGMEM = {
  VP_PrintProgress_Percentage, VP_PrintTime,
  0x0000
};

const struct VPMapping VPMap[] PROGMEM = {
  { DGUSLCD_SCREEN_BOOT, VPList_Boot },
  { DGUSLCD_SCREEN_MAIN, VPList_Main },
  { DGUSLCD_SCREEN_STATUS, VPList_Status },
  { DGUSLCD_SCREEN_STATUS_PAUSED, VPList_Status },
  { DGUSLCD_SCREEN_FANANDFEEDRATE, VPList_FanAndFeedrate },
  { DGUSLCD_SCREEN_FLOWRATES, VPList_SD_FlowRates },
  { DGUSLCD_SCREEN_SDPRINTMANIPULATION, VPList_SD_PrintManipulation },
  { DGUSLCD_SCREEN_SDFILELIST, VPList_SDFileList },
  { 0 , nullptr } // List is terminated with an nullptr as table entry.
};

const char MarlinVersion[] PROGMEM = SHORT_BUILD_VERSION;
const char PrinterSize[] PROGMEM = "350 x 350 x 400";
const char MachineName[] PROGMEM = CUSTOM_MACHINE_NAME;

// Helper to define a DGUS_VP_Variable for common use cases.
#define VPHELPER(VPADR, VPADRVAR, RXFPTR, TXFPTR ) { .VP=VPADR, .memadr=VPADRVAR, .size=sizeof(VPADRVAR), \
  .set_by_display_handler = RXFPTR, .send_to_display_handler = TXFPTR }

// Helper to define a DGUS_VP_Variable when the sizeo of the var cannot be determined automaticalyl (eg. a string)
#define VPHELPER_STR(VPADR, VPADRVAR, STRLEN, RXFPTR, TXFPTR ) { .VP=VPADR, .memadr=VPADRVAR, .size=STRLEN, \
  .set_by_display_handler = RXFPTR, .send_to_display_handler = TXFPTR }

void MainScreenControl(DGUS_VP_Variable &var, void *val_ptr) {
  unsigned char* bytes = (unsigned char*)val_ptr;
  switch(bytes[1]) {
    case 1:
      // dgusdisplay.RequestScreen(DGUSLCD_SCREEN_PRINTFILE);
      break;
    case 3:
      dgusdisplay.RequestScreen(DGUSLCD_SCREEN_TEMP_CONTROL);
    }
}

void SettingScreenControl(DGUS_VP_Variable &var, void *val_ptr) {
  unsigned char* bytes = (unsigned char*)val_ptr;
  uint8_t abl_probe_index = 0;
  switch(bytes[1]) {
    case 1:
      for(uint8_t outer = 1; outer < 6 ; outer++)
      {
        for (uint8_t inner = 1; inner < 6; inner++)
        {
          xy_uint8_t point = {inner, outer};
          dgusdisplay.WriteVariable(VP_BED_MEASUTRMENT + (abl_probe_index * 2), 
                                    (int16_t)(ExtUI::getMeshPoint(point) * 1000));
          ++abl_probe_index;
        }
      }
      break;
    case 3:
      dgusdisplay.WriteVariable(VP_MOVE_X, (uint16_t)(ExtUI::getAxisPosition_mm(ExtUI::X) * 10));
      dgusdisplay.WriteVariable(VP_MOVE_Y, (uint16_t)(ExtUI::getAxisPosition_mm(ExtUI::Y) * 10));
      dgusdisplay.RequestScreen(DGUSLCD_SCREEN_MANUALMOVE);
      break;
    }  
}

void SetTempScreenControl(DGUS_VP_Variable &var, void *val_ptr) {
  unsigned char* bytes = (unsigned char*)val_ptr;
  switch(bytes[1]) {
    case 0:
      dgusdisplay.RequestScreen(DGUSLCD_SCREEN_TEMP_CONTROL);
      break;
    }
}

void LevelingScreenControl(DGUS_VP_Variable &var, void *val_ptr) {
  unsigned char* bytes = (unsigned char*)val_ptr;
  switch(bytes[1]) {
    case 1:
      dgusdisplay.RequestScreen(DGUSLCD_SCREEN_SETTING);
      break;
    }
}

void MoveX(DGUS_VP_Variable &var, void *val_ptr) {
  unsigned char* bytes = (unsigned char*)val_ptr;
  ExtUI::setAxisPosition_mm(((bytes[0] << 8) + bytes[1]) / 10., ExtUI::X);
}

void MoveY(DGUS_VP_Variable &var, void *val_ptr) {
  unsigned char* bytes = (unsigned char*)val_ptr;
  ExtUI::setAxisPosition_mm(((bytes[0] << 8) + bytes[1]) / 10., ExtUI::Y);
}

void MoveZ(DGUS_VP_Variable &var, void *val_ptr) {
  unsigned char* bytes = (unsigned char*)val_ptr;
  ExtUI::setAxisPosition_mm(((bytes[0] << 8) + bytes[1]) / 10., ExtUI::Z);
}

void HomeXY(DGUS_VP_Variable &var, void *val_ptr) {
  ExtUI::injectCommands_P((PSTR("G28 XY")));
  dgusdisplay.WriteVariable(VP_MOVE_X, (uint16_t)X_HOME_POS * 10);
  dgusdisplay.WriteVariable(VP_MOVE_Y, (uint16_t)Y_HOME_POS * 10);
}

void ShowAdjust(DGUS_VP_Variable &var, void *val_ptr) {
}

void BedMeasure(DGUS_VP_Variable &var, void *val_ptr) {
  if (ExtUI::isMoving() || ExtUI::commandsInQueue()) {
    return;
  }
  unsigned char* bytes = (unsigned char*)val_ptr;
  switch(bytes[1]) {
  case 1:  // Home z
    if (!isAxisPositionKnown(ExtUI::axis_t::X) 
        || !isAxisPositionKnown(ExtUI::axis_t::Y))
      ExtUI::injectCommands_P(PSTR("G28\nG1F1500Z0.0"));
    else
      ExtUI::injectCommands_P(PSTR("G28Z\nG1F1500Z0.0"));
    break;
  case 2:  // Z+
    if (WITHIN((ExtUI::getZOffset_mm() + 0.1), Z_PROBE_OFFSET_RANGE_MIN, Z_PROBE_OFFSET_RANGE_MAX)) {
      ExtUI::smartAdjustAxis_steps(
          ExtUI::mmToWholeSteps(0.1,  ExtUI::axis_t::Z), 
          ExtUI::axis_t::Z, true);
      ExtUI::setZOffset_mm(ExtUI::getZOffset_mm() + 0.1);
      ExtUI::injectCommands_P(PSTR("M500"));
    }
    break;
  case 3:  // z-
    if (WITHIN((ExtUI::getZOffset_mm() - 0.1), Z_PROBE_OFFSET_RANGE_MIN, Z_PROBE_OFFSET_RANGE_MAX)) {
      ExtUI::smartAdjustAxis_steps(
          ExtUI::mmToWholeSteps(-0.1,  ExtUI::axis_t::Z), 
          ExtUI::axis_t::Z, true);
      ExtUI::setZOffset_mm(ExtUI::getZOffset_mm() - 0.1);
      ExtUI::injectCommands_P(PSTR("M500"));
    }
    break;
  case 5:  // Measure
    if (!ExtUI::isPositionKnown()) {
      ExtUI::injectCommands_P(PSTR("G28"));
    }
    ExtUI::injectCommands_P(PSTR("G29P1\nG29S1\nG29S0\nG29F0.0\nG29A\nM500"));
  }
}

const struct DGUS_VP_Variable ListOfVP[] PROGMEM = {
    // Helper to detect touch events

    {.VP = VP_MARLIN_VERSION, .memadr = (void *)MarlinVersion, .size = VP_MARLIN_VERSION_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenHandler::DGUSLCD_SendStringToDisplayPGM},
    {.VP = VP_MACHINE_NAME, .memadr = (void *)MachineName, .size = VP_MACHINE_NAME_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenHandler::DGUSLCD_SendStringToDisplayPGM},
    {.VP = VP_PrinterSize, .memadr = (void *)PrinterSize, .size = sizeof(PrinterSize), .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenHandler::DGUSLCD_SendStringToDisplayPGM},

    VPHELPER(VP_T_E0_Is, nullptr, nullptr, 
             (&dgusdisplay.SetVariable<extruder_t, getActualTemp_celsius, E0, long>)),
    VPHELPER(VP_T_E0_Set, nullptr,
            (&dgusdisplay.GetVariable<extruder_t, setTargetTemp_celsius, E0>),
            (&dgusdisplay.SetVariable<extruder_t, getTargetTemp_celsius, E0>)),

    VPHELPER(VP_T_Bed_Is, nullptr, nullptr, 
             (&dgusdisplay.SetVariable<heater_t, getActualTemp_celsius, H0, long>)),
    VPHELPER(VP_T_Bed_Set, nullptr,
            (&dgusdisplay.GetVariable<heater_t, setTargetTemp_celsius, H0>),
            (&dgusdisplay.SetVariable<heater_t, getTargetTemp_celsius, H0>)),

    VPHELPER(VP_CONFIRMED, nullptr, DGUSScreenHandler::ScreenConfirmedOK, nullptr),
    VPHELPER(VP_MAIN_SCREEN, nullptr, MainScreenControl, nullptr),
    VPHELPER(VP_SETTING_SCREEN, nullptr, SettingScreenControl, nullptr),
    VPHELPER(VP_LEVELING_SCREEN, nullptr, LevelingScreenControl, nullptr),
    VPHELPER(VP_SETTEMP_SCREEN, nullptr, SetTempScreenControl, nullptr),
    VPHELPER(VP_HOME_XY, nullptr, HomeXY, nullptr),
    VPHELPER(VP_BED_AUTO_MEASURE, nullptr, BedMeasure, nullptr),

    VPHELPER(VP_MOVE_X, &current_position.x, MoveX,                     DGUSScreenHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
    VPHELPER(VP_MOVE_Y, &current_position.y, MoveY, DGUSScreenHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
    VPHELPER(VP_MOVE_Z, &current_position.z, MoveZ, DGUSScreenHandler::DGUSLCD_SendFloatAsIntValueToDisplay<1>),
    VPHELPER(VP_PROGRESS, nullptr, nullptr, DGUSScreenHandler::DGUSLCD_SendPrintProgressToDisplay),
    //VPHELPER(VP_TIME_HOUR, &current_position.z, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),
    //VPHELPER(VP_TIME_HOUR, &current_position.z, nullptr, DGUSScreenVariableHandler::DGUSLCD_SendFloatAsLongValueToDisplay<2>),

    VPHELPER(VP_E0_PID_P, nullptr, nullptr,
            (&dgusdisplay.SetVariable<extruder_t, getPIDValues_Kp, E0>)),
    VPHELPER(VP_E0_PID_I, nullptr, nullptr,
            (&dgusdisplay.SetVariable<extruder_t, getPIDValues_Ki, E0>)),
    VPHELPER(VP_E0_PID_D, nullptr, nullptr,
            (&dgusdisplay.SetVariable<extruder_t, getPIDValues_Kd, E0>)),


    VPHELPER(VP_ADJUST_BTN, nullptr, ShowAdjust, nullptr),

    VPHELPER(VP_TEMP_ALL_OFF, nullptr, &DGUSScreenHandler::HandleAllHeatersOff, nullptr),

    VPHELPER(VP_MOTOR_LOCK_UNLOK, nullptr, &DGUSScreenHandler::HandleMotorLockUnlock, nullptr),

    // Messages for the User, shared by the popup and the kill screen. They cant be autouploaded as we do not buffer content.
    {.VP = VP_MSGSTR1, .memadr = nullptr, .size = VP_MSGSTR1_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenHandler::DGUSLCD_SendStringToDisplayPGM},
    {.VP = VP_MSGSTR2, .memadr = nullptr, .size = VP_MSGSTR2_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenHandler::DGUSLCD_SendStringToDisplayPGM},
    {.VP = VP_MSGSTR3, .memadr = nullptr, .size = VP_MSGSTR3_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenHandler::DGUSLCD_SendStringToDisplayPGM},
    {.VP = VP_MSGSTR4, .memadr = nullptr, .size = VP_MSGSTR4_LEN, .set_by_display_handler = nullptr, .send_to_display_handler = &DGUSScreenHandler::DGUSLCD_SendStringToDisplayPGM},

    VPHELPER(0, 0, 0, 0) // must be last entry.
};

#endif // DGUS_LCD_UI_ORIGIN
