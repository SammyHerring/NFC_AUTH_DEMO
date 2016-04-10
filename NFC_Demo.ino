// Defined Libraries - all seen are required at latest version avaliable (10/04/16)
#include <Adafruit_PN532.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Defined Pins
#define IRQ 2 // IRQ for PN532
#define RESET 3 // Reset for PN532 - DO NOT CONNECT ON CIRCUIT FOR I2C/DEMO
#define NEOPIN 7 // Neopixel Pin
#define NUMPIXELS 1 // Number of Neopixels
#define OLED_RESET 4 // OLED Reset Pin

// Defined Commands
Adafruit_PN532 nfc(IRQ, RESET);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIN, NEO_GRB + NEO_KHZ800);
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  // Begin Neopixel Setup
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code
  pixels.begin(); // This initializes the NeoPixel library.
  // End Neopixel Setup

  // Begin PN532 and Serial Setup
  Serial.begin(9600);
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  // configure board to read RFID tags
  nfc.SAMConfig();
  // End PN532 and Serial Setup

  // Begin OLED Setup
  #if (SSD1306_LCDHEIGHT != 32)
  #error("Height incorrect, please fix Adafruit_SSD1306.h!");
  #endif
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  uint32_t cardidentifier = 0;
  // End OLED Setup
  
  // Start LED Pinmode Setup
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  // End LED Pinmode Setup
  
  // Start Ready ALL COMP Setup
  Reset(); 
  // End Ready ALL COMP Setup
}

unsigned digit = 0;

void loop() {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 }; // Buffer to store the returned UID
  uint8_t uidLength; // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // wait for RFID card to show up!
  Serial.println("Waiting for an ISO14443A Card ...");

  // Wait for an ISO14443A type cards (Mifare, etc.). When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  uint32_t cardidentifier = 0;
 
  if (success) {
    // Found a card!

    Serial.print("Card detected #");
    // turn the four byte UID of a mifare classic into a single variable #
    cardidentifier = uid[3];
    cardidentifier <<= 8; cardidentifier |= uid[2];
    cardidentifier <<= 8; cardidentifier |= uid[1];
    cardidentifier <<= 8; cardidentifier |= uid[0];
    Serial.println(cardidentifier);
    
    if (cardidentifier ==  4230093649) { //update with your RFID identifier!
      Serial.println("Valid");
      Serial.println("Colour: GREEN");
      Neo_Green();
      OLED_Valid(cardidentifier);
      delay(1000); //makes sure the password isn't repeated
      Reset();
    } else if (cardidentifier == 578365700) {
      Serial.println("Special");
      Serial.println("Colour: BLUE");
      Neo_Blue();
      OLED_Special(cardidentifier);
      delay(500);
      scrolltext(cardidentifier);
      LED_Special();
      OLED_Special(cardidentifier);
      delay(1000); //makes sure the password isn't repeated
      Reset();
    } else {
      Serial.println("Invalid");
      Serial.println("Colour: RED");
      Neo_Red();
      OLED_Invalid(cardidentifier);
      delay(1000);
      Reset();
    }
    }
  }

void Neo_Yellow() {
  // LED
  // Start Neopixel Command
      for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // Wipe colours
      pixels.setPixelColor(i, pixels.Color(50,50,0)); // Moderately bright yellow color.
      pixels.show();
      }
      // End Neopixel Command
}

void Neo_Red() {
  // Start Neopixel Command
      for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // Wipe colours
      pixels.setPixelColor(i, pixels.Color(50,0,0)); // Moderately bright red color.
      pixels.show();
      }
      // End Neopixel Command
}

void Neo_Green() {
  // Start Neopixel Command
      for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // Wipe colours
      pixels.setPixelColor(i, pixels.Color(0,50,0)); // Moderately bright green color.
      pixels.show();
      }
      // End Neopixel Command
}

void Neo_Blue() {
  // Start Neopixel Command
      for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i, pixels.Color(0,0,0)); // Wipe colours
      pixels.setPixelColor(i, pixels.Color(0,0,50)); // Moderately bright green color.
      pixels.show();
      }
      // End Neopixel Command
}

int OLED_Ready() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Ready");
  display.display();
}

int OLED_Valid(uint32_t cardidentifier) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Valid");
  display.println(cardidentifier);
  display.display();
}

int OLED_Invalid(uint32_t cardidentifier) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Invalid");
  display.println(cardidentifier);
  display.display();
}

int OLED_Special(uint32_t cardidentifier) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Special");
  display.println(cardidentifier);
  display.display();
}

void LED_Special() {
  digitalWrite(8, HIGH);
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
  delay(300);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
  delay(300);
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
  delay(300);
  digitalWrite(9, HIGH);
  digitalWrite(10, LOW);
  delay(300);
  digitalWrite(9, LOW);
  digitalWrite(8, LOW);
}

int scrolltext(uint32_t cardidentifier) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println(cardidentifier);
  display.display();
  delay(1);
 
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);    
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
}

void Reset() {
  Neo_Yellow();
  OLED_Ready();
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  display.println("---");
  display.println("Ready");
  display.println("---");
}

