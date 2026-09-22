#include <M5Unified.h>
#include <M5GFX.h>
#include <math.h>
#include "youtube_icon.h"
#include "steam_icon.h"
#include "edge_icon.h"
#include "apps_icon.h"
#include "play_pause_icon.h"
#include "prev_icon.h"
#include "next_icon.h"
#include "volume_low_icon.h"
#include "volume_mute_icon.h"
#include "volume_up_icon.h"
#include "restart_icon.h"
#include "toggle_off_icon.h"
#include "moon_icon.h"
#include "lock_icon.h"
#include <string.h>
#include <stdlib.h>




//note: When including notes make sure that the format is icon.h
// This is because of the fact arduino cant read png

//Pagenumbering system 

int currentPage = 0;
const int totalPages = 3;

//calibration of the cursor
//Note for when scrolling that for better scrolling the finger
// needs to scroll under the buttons otherwise it might click the buttons instead
int touchStartX = 0;
int touchStartY = 0;

unsigned long lastBatteryUpdate = 0;

// Smooth page swipe animation
M5Canvas oldPageCanvas(&M5.Display);
M5Canvas newPageCanvas(&M5.Display);
bool pageCanvasesReady = false;
LovyanGFX* drawTarget = &M5.Display;


// the button UI 
// Mock is on my ipad to refer too

void drawButton(int x, int y, int w, int h, uint16_t color, const char* title) {

  // Darken the button
  uint16_t darkColor = drawTarget->color565(
    ((color >> 11) * 255 / 31) / 3,
    (((color >> 5) & 0x3F) * 255 / 63) / 3,
    ((color & 0x1F) * 255 / 31) / 3
  );

  // Making the translucent look
  drawTarget->fillRoundRect(
    x, y, w, h, 12,
    darkColor
  );

  // Bright coloured outline
  drawTarget->drawRoundRect(
    x, y, w, h, 12,
    color
  );

  // Slight inner highlight
  drawTarget->drawRoundRect(
    x + 1, y + 1,
    w - 2, h - 2,
    11,
    color
  );

  // Text
  drawTarget->setFont(&fonts::Font2);
  drawTarget->setTextSize(1);
  drawTarget->setTextColor(TFT_WHITE);

  int textWidth = drawTarget->textWidth(title);
  int textX = x + (w - textWidth) / 2;
  int textY = y + h - 22;

  drawTarget->setCursor(textX, textY);
  drawTarget->print(title);
}

void drawButtonWithIcon(int x, int y, int w, int h, uint16_t color, const uint16_t* icon, int iconSize, const char* title) {

  // Darkbackground
  uint16_t darkColor = drawTarget->color565(
    (((color >> 11) * 255 / 31) / 3),
    (((color >> 5) & 0x3F) * 255 / 63) / 3,
    (((color & 0x1F) * 255 / 31) / 3)
  );

  drawTarget->fillRoundRect(
    x, y, w, h, 12, darkColor
  );

  // border
  drawTarget->drawRoundRect(
    x, y, w, h, 12, color
  );

  drawTarget->drawRoundRect(
    x + 1, y + 1,
    w - 2, h - 2,
    11, color
  );

  // Icon
  int iconX = x + (w - iconSize) / 2;
  int iconY = y + 10;

bool oldSwap = drawTarget->getSwapBytes();

drawTarget->setSwapBytes(true);

drawTarget->pushImage( iconX, iconY, iconSize, iconSize, icon, 0x0000);
drawTarget->setSwapBytes(oldSwap);
 

  // Text
  drawTarget->setFont(&fonts::Font2);
  drawTarget->setTextSize(1);
  drawTarget->setTextColor(TFT_WHITE);

  int textWidth = drawTarget->textWidth(title);

  int textX = x + (w - textWidth) / 2;
  int textY = y + h - 18;

  drawTarget->setCursor(textX, textY);
  drawTarget->print(title);
}


// feedback when the user presses a button
uint16_t brightenButtonColor(uint16_t color, float amount) {

  int r = (color >> 11) & 0x1F;
  int g = (color >> 5) & 0x3F;
  int b = color & 0x1F;

  int whiteR = 31;
  int whiteG = 63;
  int whiteB = 31;

  r += (int)((whiteR - r) * amount);
  g += (int)((whiteG - g) * amount);
  b += (int)((whiteB - b) * amount);

  return (r << 11) | (g << 5) | b;
}

void animateButtonPress(int x, int y, int w, int h, uint16_t color, const uint16_t* icon, int iconSize, const char* title) {


  // only change the button area rather than redrawing the wholepage (wasting recources + looks ugly )
  const int margin = 10;
  const int spriteX = x - margin;
  const int spriteY = y - margin;
  const int spriteW = w + (margin * 2);
  const int spriteH = h + (margin * 2);

  M5Canvas buttonCanvas(&M5.Display);
  buttonCanvas.setPsram(true);
  buttonCanvas.createSprite(spriteW, spriteH);

  size_t pixelCount = (size_t)spriteW * (size_t)spriteH;
  size_t bufferSize = pixelCount * sizeof(uint16_t);

  uint16_t* originalBuffer = (uint16_t*)malloc(bufferSize);

  if (originalBuffer == nullptr) {
    return;
  }

  
  M5.Display.readRect(spriteX,spriteY,spriteW,spriteH,originalBuffer);

  LovyanGFX* oldTarget = drawTarget;
  drawTarget = &buttonCanvas;


  for (int frame = 0; frame <= 10; frame++) {

    float progress = frame / 10.0f;

    
    float pressAmount = sinf(progress * 3.14159f) * 3.0f;

    // when they press to make the button brighter to show
    float brightness = sinf(progress * 3.14159f) * 0.65f;
    uint16_t pressedColor = brightenButtonColor(color, brightness);

   //bring old page
   //temporary removal trying something out
    //drawPage(); 


    memcpy(buttonCanvas.getBuffer(), originalBuffer, bufferSize);



    // glow effect
    for (int glow = 8; glow >= 2; glow -= 2) {

      float glowAmount = brightness * (1.0f - ((glow - 2) / 8.0f));
      uint16_t glowColor = brightenButtonColor(color, 0.15f + (glowAmount * 0.85f));

      buttonCanvas.drawRoundRect((x - spriteX) - glow, (y - spriteY) - glow, w + (glow * 2), h + (glow * 2), 12 + glow, glowColor);
    }

    // button gets shrinked a tiny bit like ios for example, (little effect can remove if want)

    int buttonX = (x - spriteX) + (int)pressAmount;
    int buttonY = (y - spriteY) + (int)pressAmount;
    int buttonW = w - ((int)pressAmount * 2);
    int buttonH = h - ((int)pressAmount * 2);

    drawButtonWithIcon(buttonX,buttonY,buttonW,buttonH,pressedColor,icon,iconSize,title);
    buttonCanvas.pushSprite(spriteX, spriteY);
    delay(12);
  }

  // Return to the normal page
  //drawPage();
  // once again temp change
  memcpy(buttonCanvas.getBuffer(), originalBuffer, bufferSize);
  buttonCanvas.pushSprite(spriteX, spriteY);

  drawTarget = oldTarget;

  free(originalBuffer);

}

// page numbering system

void drawPageNumber() {

  drawTarget->setFont(&fonts::Font0);
  drawTarget->setTextSize(1);
  drawTarget->setTextColor(TFT_LIGHTGREY);

  String pageNumber = String(currentPage + 1) + "/" + String(totalPages);

  int textWidth = drawTarget->textWidth(pageNumber);

  drawTarget->setCursor(
    320 - textWidth - 6,
    4
  );

  drawTarget->print(pageNumber);
}


// dot ui for the bottom, it stopped working 

void drawPageDots() {

  drawTarget->setTextSize(2);

  // Start around the centre of the screen
  int startX = 135;

  for (int i = 0; i < totalPages; i++) {

    drawTarget->setCursor(startX + (i * 20), 220);

    if (i == currentPage) {
      drawTarget->setTextColor(TFT_WHITE);
      drawTarget->print("●");
    }
    else {
      drawTarget->setTextColor(TFT_DARKGREY);
      drawTarget->print("○");
    }
  }
}


// stream deck page
void drawHome() {

drawTarget->setFont(&fonts::Font2);
drawTarget->setTextSize(2);
drawTarget->setTextColor(TFT_CYAN);

const char* title = "MY STREAM DECK";

int titleWidth = drawTarget->textWidth(title);

drawTarget->setCursor(
  (320 - titleWidth) / 2,
  6
);

drawTarget->print(title);
// buttons 
// Add the images after the colour of the button
// note to self when adding an icon please make sure you changed it from drawButton to drawButtonWithIcon
  drawButtonWithIcon(10, 45, 145, 75, TFT_RED, youtube_icon, 48, "YOUTUBE");
  drawButtonWithIcon(165, 45, 145, 75, TFT_BLUE, steam_icon, 48, "STEAM");
  drawButtonWithIcon(10, 130, 145, 75, TFT_BLUE, edge_icon, 48, "EDGE");
  drawButtonWithIcon(165, 130, 145, 75, TFT_PURPLE, apps_icon, 48, "APPS");
}


//Page for media

void drawMediaPage() {

  drawTarget->fillScreen(TFT_BLACK);

  drawTarget->setTextColor(TFT_WHITE);
  drawTarget->setTextSize(2);

  drawTarget->setCursor(105, 8);
  drawTarget->println("  MEDIA");

  drawButtonWithIcon(10, 45, 95, 75, TFT_PURPLE, prev_icon ,48 ,"PREV");
  drawButtonWithIcon(112, 45, 95, 75, TFT_GREEN, play_pause_icon, 48, "PLAY/PAUSE");
  drawButtonWithIcon(214, 45, 95, 75, TFT_PURPLE, next_icon, 48, "NEXT");

  drawButtonWithIcon(10, 130, 95, 75, TFT_RED, volume_mute_icon, 48, "MUTE");
  drawButtonWithIcon(112, 130, 95, 75, TFT_BLUE, volume_low_icon, 48, "VOL -");
  drawButtonWithIcon(214, 130, 95, 75, TFT_BLUE, volume_up_icon, 48, "VOL +");
}


// pc section page

void drawPCPage() {

  drawTarget->fillScreen(TFT_BLACK);

  drawTarget->setTextColor(TFT_WHITE);
  drawTarget->setTextSize(2);

  drawTarget->setCursor(125, 8);
  drawTarget->println("PC");

  
  drawButtonWithIcon(10, 45, 145, 75, TFT_BLUE, lock_icon, 48, "LOCK");
  drawButtonWithIcon(165, 45, 145, 75, TFT_YELLOW, moon_icon, 48, "SLEEP");

  
  drawButtonWithIcon(10, 130, 145, 75, TFT_ORANGE, restart_icon, 48, "RESTART");
  drawButtonWithIcon(165, 130, 145, 75, TFT_RED, toggle_off_icon, 48, "SHUTDOWN");
}



void drawBattery() {

  int batteryLevel = M5.Power.getBatteryLevel();

  drawTarget->setFont(&fonts::Font0);
  drawTarget->setTextSize(1);
  drawTarget->setTextColor(TFT_LIGHTGREY);

  String batteryText = String(batteryLevel) + "%";

  drawTarget->setCursor(6, 4);

  drawTarget->print(batteryText);
}

//smootheing of the swiping

void animatePageChange(int nextPage, int direction) {

  if (!pageCanvasesReady) {
    oldPageCanvas.setPsram(true);
    newPageCanvas.setPsram(true);

    oldPageCanvas.createSprite(320, 240);
    newPageCanvas.createSprite(320, 240);

    pageCanvasesReady = true;
  }

  // Save the current page before drawing the new one
  M5.Display.readRect(
    0, 0, 320, 240,
    (uint16_t*)oldPageCanvas.getBuffer()
  );

  // Draw the new page normally
  currentPage = nextPage;

  LovyanGFX* oldTarget = drawTarget;
  drawTarget = &newPageCanvas;
  drawPage();
  drawTarget = oldTarget;

  // Animate the two saved pages
  for (int frame = 0; frame <= 16; frame++) {

    float progress = frame / 16.0f;

    // Smooth ease-out
    float eased = 1.0f - powf(1.0f - progress, 3.0f);

    int offset = (int)(320.0f * eased);

    M5.Display.startWrite();

    if (direction < 0) {
      // Swipe LEFT: old page moves left, new page enters from right
      oldPageCanvas.pushSprite(-offset, 0);
      newPageCanvas.pushSprite(320 - offset, 0);
    }
    else {
      // Swipe RIGHT: old page moves right, new page enters from left
      oldPageCanvas.pushSprite(offset, 0);
      newPageCanvas.pushSprite(-320 + offset, 0);
    }

    M5.Display.endWrite();

    delay(8);
  }

  // Make sure the final page is exactly where it should be
  newPageCanvas.pushSprite(0, 0);
}

// start drawing the current on screen page
// drawing is rendering
void drawPage() {

  // Clear the previous page
  drawTarget->fillScreen(TFT_BLACK);

  if (currentPage == 0) {

    drawHome();

  }

  else if (currentPage == 1) {

    drawMediaPage();

  }

  else if (currentPage == 2) {

    drawPCPage();

  }

  // Small status information
  drawBattery();
  drawPageNumber();
}


\

void setup() {

  auto cfg = M5.config();

  M5.begin(cfg);

  Serial.begin(115200);

  drawPage();
}


// looping section
// detemines what can hapen more than once

void loop() {

  M5.update();

  auto touch = M5.Touch.getDetail();




  if (touch.wasPressed()) {

    touchStartX = touch.x;
    touchStartY = touch.y;
  }


// checks if the user wants to swipe or not

  if (touch.wasReleased()) {

    int swipeX = touch.x - touchStartX;
    int swipeY = touch.y - touchStartY;

   
    if (abs(swipeX) > 60 && abs(swipeX) > abs(swipeY)) {

      
      if (swipeX < 0) {

        currentPage++;

        if (currentPage >= totalPages) {
          currentPage = 0;
        }

        animatePageChange(currentPage, -1);

        return;
      }

     
      if (swipeX > 0) {

        currentPage--;

        if (currentPage < 0) {
          currentPage = totalPages - 1;
        }

        animatePageChange(currentPage, 1);

        return;
      }
    }
  }

//logic for 
// what happens when a button is pressed

  if (touch.wasPressed()) {

    int x = touch.x;
    int y = touch.y;


// home page for all the main apps that i put on the 
// my stream deck page

    if (currentPage == 0) {

      // YouTube
      if (x >= 10 && x <= 155 && y >= 45 && y <= 120) {

        Serial.println("YOUTUBE");
        animateButtonPress(10, 45, 145, 75, TFT_RED, youtube_icon, 48, "YOUTUBE");

      }


      // Steam
      else if (x >= 165 && x <= 310 && y >= 45 && y <= 120) {

        Serial.println("STEAM");
        animateButtonPress(165, 45, 145, 75, TFT_BLUE, steam_icon, 48, "STEAM");
      }


      // Edge
      else if (x >= 10 && x <= 155 && y >= 130 && y <= 205) {
        Serial.println("EDGE");
        animateButtonPress(10, 130, 145, 75, TFT_BLUE, edge_icon, 48, "EDGE");
      }


      // APPS
      else if (x >= 165 && x <= 310 && y >= 130 && y <= 205) {

        Serial.println("APPS");
        animateButtonPress(165, 130, 145, 75, TFT_PURPLE, apps_icon, 48, "APPS");

      }
    }


//media controller place 
// will detect media playing and do things

    else if (currentPage == 1) {

      // Previous
      if (x >= 10 && x <= 105 && y >= 45 && y <= 120) {

        Serial.println("MEDIA_PREVIOUS");
        animateButtonPress(10, 45, 95, 75, TFT_PURPLE, prev_icon, 48, "PREV");

      }

      // Play
      else if (x >= 112 && x <= 207 && y >= 45 && y <= 120) {

        Serial.println("MEDIA_PLAY");
        animateButtonPress(112, 45, 95, 75, TFT_GREEN, play_pause_icon, 48, "PLAY/PAUSE");
        
      }

      // Next
      else if (x >= 214 && x <= 309 && y >= 45 && y <= 120) {

        Serial.println("MEDIA_NEXT");
        animateButtonPress(214, 45, 95, 75, TFT_PURPLE, next_icon, 48, "NEXT");

      }

      // Mute
      else if (x >= 10 && x <= 105 && y >= 130 && y <= 205) {

        Serial.println("MEDIA_MUTE");
        animateButtonPress(10, 130, 95, 75, TFT_RED, volume_mute_icon, 48, "MUTE");

      }

      // Volume down
      else if (x >= 112 && x <= 207 && y >= 130 && y <= 205) {

        Serial.println("VOLUME_DOWN");
        animateButtonPress(112, 130, 95, 75, TFT_BLUE, volume_low_icon, 48, "VOL -");

      }

      // Volume up
      else if (x >= 214 && x <= 309 && y >= 130 && y <= 205) {

        Serial.println("VOLUME_UP");
        animateButtonPress(214, 130, 95, 75, TFT_BLUE, volume_up_icon, 48, "VOL +");

      }
    }


//pc page 
//soon will have the feature to be able to control pc activities

    else if (currentPage == 2) {

      // Lock
      if (x >= 10 && x <= 155 && y >= 45 && y <= 120) {

        Serial.println("PC_LOCK");
        animateButtonPress(10, 45, 145, 75, TFT_BLUE, lock_icon, 48, "LOCK");

      }

      // Sleep
      else if (x >= 165 && x <= 310 && y >= 45 && y <= 120) {

        Serial.println("PC_SLEEP");
        animateButtonPress(165, 45, 145, 75, TFT_YELLOW, moon_icon, 48, "SLEEP");

      }

      // Restart
      else if (x >= 10 && x <= 155 && y >= 130 && y <= 205) {

        Serial.println("PC_RESTART");
        animateButtonPress(10, 130, 145, 75, TFT_ORANGE, restart_icon, 48, "RESTART");

      }

      // Shutdown
      else if (x >= 165 && x <= 310 && y >= 130 && y <= 205) {

        Serial.println("PC_SHUTDOWN");
        animateButtonPress(165, 130, 145, 75, TFT_RED, toggle_off_icon, 48, "SHUTDOWN");

      }
    }
  }
}
