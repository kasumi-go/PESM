#include <Adafruit_GFX.h>     // 核心图像库，必须包含
#include <Adafruit_ST7789.h>  // ST7789 TFT驱动库，必须包含
#include <SPI.h>
#include <QMC5883L.h> //霍尔传感器库
#include <VL53L0X.h>  //激光测距传感器库

QMC5883L compass; 

VL53L0X LASER_RANGING;

/*单片机控制TFT的相关引脚，具体还需参考显示屏生产商手册*/
#define TFT_CS  10
#define TFT_RST 7 
#define TFT_DC  6

// 1.3寸TFT屏对应的控制函数
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup(void) {
  Wire.begin();
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));

  compass.init();  //初始化霍尔传感器

  LASER_RANGING.setTimeout(500);  //激光传感器初始化，如果超过500MS未响应则报错
  if (!LASER_RANGING.init()) {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

  tft.init(240, 240);  // 初始化屏幕分辨率

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(ST77XX_BLACK);  //黑屏测试
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  tft.fillScreen(ST77XX_BLACK);
  testdrawtext("INITIALIZATING", ST77XX_YELLOW);  //屏幕显示初始化
  delay(1000);
}

void loop() {
  /*读取并处理霍尔传感器数据*/
  int x, y, z;
  compass.read(&x, &y, &z); //读取霍尔传感器XYZ轴数据
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  // Atan2() automatically check the correct formula taking care of the quadrant you are in
  float heading = atan2(y, x);
  float declinationAngle = 0.0404;
  heading += declinationAngle;
  // Correct for when signs are reversed.
  if (heading < 0)
    heading += 2 * PI;
  // Check for wrap due to addition of declination.
  if (heading > 2 * PI)
    heading -= 2 * PI;
  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180 / M_PI;
  char string_DEGREESS[8];
  itoa(headingDegrees, string_DEGREESS, 10);
  char string_NORTH[8] = "N";
  char string_SOUTH[8] = "S";
  char string_WEST[8] = "W";
  char string_EAST[8] = "E";
  char string_DIRECTION[8] = "";
  if ((headingDegrees > 0 && headingDegrees < 20) || (headingDegrees > 340 && headingDegrees < 360)) {
    strcpy(string_DIRECTION, string_NORTH);
  } else if (headingDegrees > 105 && headingDegrees < 120) {
    strcpy(string_DIRECTION, string_SOUTH);
  } else if (headingDegrees > 195 && headingDegrees < 215) {
    strcpy(string_DIRECTION, string_WEST);
  } else if (headingDegrees > 55 && headingDegrees < 65) {
    strcpy(string_DIRECTION, string_EAST);
  }
  delay(100);

  /*读取并处理激光传感器数据*/
  Serial.println(LASER_RANGING.readRangeSingleMillimeters());  //调试用，输出激光测距数据，“LASER_RANGING.readRangeSingleMillimeters()”内就是激光测距数据
  char string_laser_ranging[8];
  char diliver[8] = "_______";
  char string_DATA_INAVILABLE[8] = "N/A";
  if (LASER_RANGING.readRangeSingleMillimeters() > 8000) {
    strcpy(string_laser_ranging, string_DATA_INAVILABLE);
  } else {
    itoa(LASER_RANGING.readRangeSingleMillimeters(), string_laser_ranging, 10);
  }

  /*往屏幕写入激光测距数据*/
  tft.fillScreen(ST77XX_BLACK);  //设置背景色
  tft.setCursor(0, 30);  //设置文本开始坐标
  tft.setTextSize(2);  //设置文本大小
  tft.setTextColor(ST77XX_YELLOW);  //设置文本颜色
  tft.setTextSize(2);
  tft.printf("DISTANCE:");  //写入文本
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(2);
  tft.printf(string_laser_ranging);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.println(" mm");
  delay(100);

  /*往屏幕写入方位数据*/
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.printf("DEGREES:");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(2);
  tft.println(string_DEGREESS);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.print("DIRECTION:");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(2);
  tft.println(string_DIRECTION);
  delay(10);
}

/*以下都是ADAFRUIT写的TFT屏操作函数，不要乱删*/
void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}
