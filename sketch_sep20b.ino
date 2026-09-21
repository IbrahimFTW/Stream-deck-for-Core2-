#include <M5Unified.h>
#include <M5GFX.h>
#include "youtube_icon.h"
#include "steam_icon.h"
#include "edge_icon.h"
#include "apps_icon.h"



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

// the button UI 
// Mock is on my ipad to refer too

void drawButton(int x, int y, int w, int h, uint16_t color, const char* title) {

  // Darken the button
  uint16_t darkColor = M5.Display.color565(
    ((color >> 11) * 255 / 31) / 3,
    (((color >> 5) & 0x3F) * 255 / 63) / 3,
    ((color & 0x1F) * 255 / 31) / 3
  );

  // Making the translucent look
  M5.Display.fillRoundRect(
    x, y, w, h, 12,
    darkColor
  );

  // Bright coloured outline
  M5.Display.drawRoundRect(
    x, y, w, h, 12,
    color
  );

  // Slight inner highlight
  M5.Display.drawRoundRect(
    x + 1, y + 1,
    w - 2, h - 2,
    11,
    color
  );

  // Text
  M5.Display.setFont(&fonts::Font2);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(TFT_WHITE);

  int textWidth = M5.Display.textWidth(title);
  int textX = x + (w - textWidth) / 2;
  int textY = y + h - 22;

  M5.Display.setCursor(textX, textY);
  M5.Display.print(title);
}

void drawButtonWithIcon(
  int x,
  int y,
  int w,
  int h,
  uint16_t color,
  const uint16_t* icon,
  int iconSize,
  const char* title
) {

  // Dark coloured background
  uint16_t darkColor = M5.Display.color565(
    (((color >> 11) * 255 / 31) / 3),
    (((color >> 5) & 0x3F) * 255 / 63) / 3,
    (((color & 0x1F) * 255 / 31) / 3)
  );

  M5.Display.fillRoundRect(
    x, y, w, h, 12, darkColor
  );

  // Coloured border
  M5.Display.drawRoundRect(
    x, y, w, h, 12, color
  );

  M5.Display.drawRoundRect(
    x + 1, y + 1,
    w - 2, h - 2,
    11, color
  );

  // Icon
  int iconX = x + (w - iconSize) / 2;
  int iconY = y + 10;

bool oldSwap = M5.Display.getSwapBytes();

M5.Display.setSwapBytes(true);

M5.Display.pushImage( iconX, iconY, iconSize, iconSize, icon, 0x0000);
M5.Display.setSwapBytes(oldSwap);
 

  // Text
  M5.Display.setFont(&fonts::Font2);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(TFT_WHITE);

  int textWidth = M5.Display.textWidth(title);

  int textX = x + (w - textWidth) / 2;
  int textY = y + h - 18;

  M5.Display.setCursor(textX, textY);
  M5.Display.print(title);
}

// =========================
// PAGE NUMBER
// =========================

void drawPageNumber() {

  M5.Display.setFont(&fonts::Font0);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(TFT_LIGHTGREY);

  String pageNumber = String(currentPage + 1) + "/" + String(totalPages);

  int textWidth = M5.Display.textWidth(pageNumber);

  M5.Display.setCursor(
    320 - textWidth - 6,
    4
  );

  M5.Display.print(pageNumber);
}


// =========================
// PAGE DOTS
// =========================

void drawPageDots() {

  M5.Display.setTextSize(2);

  // Start around the centre of the screen
  int startX = 135;

  for (int i = 0; i < totalPages; i++) {

    M5.Display.setCursor(startX + (i * 20), 220);

    if (i == currentPage) {
      M5.Display.setTextColor(TFT_WHITE);
      M5.Display.print("●");
    }
    else {
      M5.Display.setTextColor(TFT_DARKGREY);
      M5.Display.print("○");
    }
  }
}


// =========================
// HOME PAGE
// =========================

void drawHome() {

M5.Display.setFont(&fonts::Font2);
M5.Display.setTextSize(2);
M5.Display.setTextColor(TFT_CYAN);

const char* title = "MY STREAM DECK";

int titleWidth = M5.Display.textWidth(title);

M5.Display.setCursor(
  (320 - titleWidth) / 2,
  6
);

M5.Display.print(title);
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

  M5.Display.fillScreen(TFT_BLACK);

  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.setTextSize(2);

  M5.Display.setCursor(105, 8);
  M5.Display.println("  MEDIA");

  drawButton(10, 45, 95, 75, TFT_BLUE, "PREV");
  drawButton(112, 45, 95, 75, TFT_GREEN, "PLAY");
  drawButton(214, 45, 95, 75, TFT_BLUE, "NEXT");

  drawButton(10, 130, 95, 75, TFT_RED, "MUTE");
  drawButton(112, 130, 95, 75, TFT_ORANGE, "VOL -");
  drawButton(214, 130, 95, 75, TFT_ORANGE, "VOL +");
}


// =========================
// PC PAGE
// =========================

void drawPCPage() {

  M5.Display.fillScreen(TFT_BLACK);

  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.setTextSize(2);

  M5.Display.setCursor(125, 8);
  M5.Display.println("PC");

  // Top row
  drawButton(10, 45, 145, 75, TFT_BLUE, "LOCK");
  drawButton(165, 45, 145, 75, TFT_YELLOW, "SLEEP");

  // Bottom row
  drawButton(10, 130, 145, 75, TFT_ORANGE, "RESTART");
  drawButton(165, 130, 145, 75, TFT_RED, "SHUTDOWN");
}



void drawBattery() {

  int batteryLevel = M5.Power.getBatteryLevel();

  M5.Display.setFont(&fonts::Font0);
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(TFT_LIGHTGREY);

  String batteryText = String(batteryLevel) + "%";

  M5.Display.setCursor(6, 4);

  M5.Display.print(batteryText);
}

// =========================
// DRAW CURRENT PAGE
// =========================
void drawPage() {

  // Clear the previous page
  M5.Display.fillScreen(TFT_BLACK);

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


// =========================
// SETUP
// =========================

void setup() {

  auto cfg = M5.config();

  M5.begin(cfg);

  Serial.begin(115200);

  drawPage();
}


// =========================
// LOOP
// =========================

void loop() {

  M5.update();

  auto touch = M5.Touch.getDetail();


  // =========================
  // REMEMBER TOUCH START
  // =========================

  if (touch.wasPressed()) {

    touchStartX = touch.x;
    touchStartY = touch.y;
  }


  // =========================
  // SWIPE DETECTION
  // =========================

  if (touch.wasReleased()) {

    int swipeX = touch.x - touchStartX;
    int swipeY = touch.y - touchStartY;

    // Horizontal swipe
    if (abs(swipeX) > 60 && abs(swipeX) > abs(swipeY)) {

      // Swipe LEFT
      if (swipeX < 0) {

        currentPage++;

        if (currentPage >= totalPages) {
          currentPage = 0;
        }

        drawPage();

        return;
      }

      // Swipe RIGHT
      if (swipeX > 0) {

        currentPage--;

        if (currentPage < 0) {
          currentPage = totalPages - 1;
        }

        drawPage();

        return;
      }
    }
  }


  // =========================
  // BUTTON PRESSES
  // =========================

  if (touch.wasPressed()) {

    int x = touch.x;
    int y = touch.y;


    // =========================
    // HOME PAGE
    // =========================

    if (currentPage == 0) {

      // YouTube
      if (x >= 10 && x <= 155 && y >= 45 && y <= 120) {

        Serial.println("YOUTUBE");

        M5.Display.fillScreen(TFT_RED);

        M5.Display.setTextColor(TFT_BLACK);
        M5.Display.setTextSize(3);

        M5.Display.setCursor(75, 105);
        M5.Display.println("YOUTUBE");

        delay(500);

        drawPage();
      }


      // Steam
      else if (x >= 165 && x <= 310 && y >= 45 && y <= 120) {

        Serial.println("STEAM");

        M5.Display.fillScreen(TFT_BLUE);

        M5.Display.setTextColor(TFT_BLACK);
        M5.Display.setTextSize(3);

        M5.Display.setCursor(90, 105);
        M5.Display.println("STEAM");

        delay(500);

        drawPage();
      }


      // Edge
      else if (x >= 10 && x <= 155 && y >= 130 && y <= 205) {

        Serial.println("EDGE");

        M5.Display.fillScreen(TFT_GREEN);

        M5.Display.setTextColor(TFT_BLACK);
        M5.Display.setTextSize(3);

        M5.Display.setCursor(80, 105);
        M5.Display.println("Edge");

        delay(500);

        drawPage();
      }


      // APPS
      else if (x >= 165 && x <= 310 && y >= 130 && y <= 205) {

        Serial.println("APPS");

        M5.Display.fillScreen(TFT_GREEN);

        M5.Display.setTextColor(TFT_BLACK);
        M5.Display.setTextSize(3);

        M5.Display.setCursor(105, 105);
        M5.Display.println("APPS");

        delay(500);

        drawPage();
      }
    }


    // =========================
    // MEDIA PAGE
    // =========================

    else if (currentPage == 1) {

      // Previous
      if (x >= 10 && x <= 105 && y >= 45 && y <= 120) {

        Serial.println("MEDIA_PREVIOUS");
      }

      // Play
      else if (x >= 112 && x <= 207 && y >= 45 && y <= 120) {

        Serial.println("MEDIA_PLAY");
      }

      // Next
      else if (x >= 214 && x <= 309 && y >= 45 && y <= 120) {

        Serial.println("MEDIA_NEXT");
      }

      // Mute
      else if (x >= 10 && x <= 105 && y >= 130 && y <= 205) {

        Serial.println("MEDIA_MUTE");
      }

      // Volume down
      else if (x >= 112 && x <= 207 && y >= 130 && y <= 205) {

        Serial.println("VOLUME_DOWN");
      }

      // Volume up
      else if (x >= 214 && x <= 309 && y >= 130 && y <= 205) {

        Serial.println("VOLUME_UP");
      }
    }


    // =========================
    // PC PAGE
    // =========================

    else if (currentPage == 2) {

      // Lock
      if (x >= 10 && x <= 155 && y >= 45 && y <= 120) {

        Serial.println("PC_LOCK");
      }

      // Sleep
      else if (x >= 165 && x <= 310 && y >= 45 && y <= 120) {

        Serial.println("PC_SLEEP");
      }

      // Restart
      else if (x >= 10 && x <= 155 && y >= 130 && y <= 205) {

        Serial.println("PC_RESTART");
      }

      // Shutdown
      else if (x >= 165 && x <= 310 && y >= 130 && y <= 205) {

        Serial.println("PC_SHUTDOWN");
      }
    }
  }
}
