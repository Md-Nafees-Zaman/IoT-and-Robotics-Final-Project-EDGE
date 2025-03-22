#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
int col_pos = 0, row_pos = 0;
// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

const byte ROWS = 4; /* four rows */
const byte COLS = 4; /* four columns */
/* define the symbols on the buttons of the keypads */
char hexaKeys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 13, 12, 14, 27 }; /* connect to the row pinouts of the keypad */
byte colPins[COLS] = { 26, 25, 33, 32 }; /* connect to the column pinouts of the keypad */

/* initialize an instance of class NewKeypad */
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);



void setup() {
  Serial.begin(115200);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  lcd.clear();
}

void loop() {
  // put your main code here, to run repeatedly:

  char customKey = customKeypad.getKey();

  if (customKey) {
    Serial.println(customKey);
    lcd.setCursor(col_pos, row_pos);
    lcd.print(customKey);
    if (col_pos < 16) col_pos++;
    else {
      row_pos++;
      col_pos = 0;
    }
  }
}
