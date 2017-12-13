#include <Arduino.h>
#include <variant.h>

// U8g2_Arduino: Arduino Monochrome Graphics Library
#include <U8g2lib.h>    //      https://github.com/olikraus/U8g2_Arduino
                        //  or  https://github.com/olikraus/u8g2


#undef U8X8_HAVE_HW_I2C

#ifdef U8X8_HAVE_HW_SPI
    #include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
    #include <Wire.h>
#endif

// Macchina M2 pin assignments to use Connector J5 on the end of your M2 located under the SD card reader
#define OLED_MOSI   SPI0_MOSI
#define OLED_CLK    SPI0_CLK
#define OLED_DC     RXD3
#define OLED_CS     PIN_SPI_SS2
#define OLED_RESET  TXD3


//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ SPI0_CLK, /* data=*/ SPI0_MOSI, /* cs=*/ OLED_CS, /* dc=*/ OLED_DC, /* reset=*/ OLED_RESET);
U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ OLED_CS, /* dc=*/ OLED_DC, /* reset=*/ OLED_RESET);

//U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8g2(/* clock=*/ SPI0_CLK, /* data=*/ SPI0_MOSI, /* cs=*/ OLED_CS, /* dc=*/ OLED_DC, /* reset=*/ OLED_RESET);
//U8X8_SSD1306_128X64_NONAME_4W_HW_SPI u8g2(/* cs=*/ OLED_CS, /* dc=*/ OLED_DC, /* reset=*/ OLED_RESET);


// ************************************************************** //
//                    GUAGE EXAMPLE
float gs_rad; //stores angle from where to start in radinats
float ge_rad; //stores angle where to stop in radinats

//example values for testing, use the values you wish to pass as argument while calling the function
//byte cx = 64; //x center
//byte cy = 32; //y center
byte cx = u8g2.getDisplayWidth()/2; //x center
byte cy = u8g2.getDisplayHeight()/2; //y center
//byte radius = 20; //radius
byte radius = u8g2.getDisplayHeight()/2; //radius
byte percent = 80; //needle percent


// ************************************************************** //
//                    GRAPHIC EXAMPLE
#define cross_width 24
#define cross_height 24
#define cross_fill_width 24
#define cross_fill_height 24
#define cross_block_width 14
#define cross_block_height 14
uint8_t draw_state = 0;

// ************************************************************** //
//                    CLOCK EXAMPLE
uint8_t m = 24;

// ************************************************************** //
//                          XBM EXAMPLE
#define u8g2_logo_97x51_width 97
#define u8g2_logo_97x51_height 51

// ************************************************************** //
//                          MINI LOGO EXAMPLE
// ************************************************************** //

// ************************************************************** //
//                          XOR Test EXAMPLE
// ************************************************************** //

// ************************************************************** //
//                     Screen Power Save EXAMPLE
// ************************************************************** //

uint32_t Delay_Time = 0;

void setup(void){
/*
    // We dont actually need to set the pins to ouputs as this is handled in the variants file
    // & any necessary pins configurations are are handled in the library
    // Included below for posterity

    pinMode(OLED_CS, OUTPUT);

    pinMode(OLED_DC, OUTPUT);
    digitalWrite(OLED_DC, LOW);

    pinMode(OLED_RESET, OUTPUT);
*/
    u8g2.begin();
    u8g2.setFlipMode(0);


    //u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.setFont(u8g2_font_courR10_tr);
    //u8g2.setFont(u8g2_font_courR08_tr);
}

void loop(void)
{
    // Uncomment the below function you wish to demonstrate

    //Rotate_Flip();

    Guage();

    //Graphic();

    //Clock();

    //XBM();

    //Mini_Logo();

    //XOR();

    //Power_Save();

}

// ****************************************************** //
//                  FLIP ROTATE
// ****************************************************** //
void Rotate_Flip(){
    // Choose font to USE ??
    //u8g2.setFont(u8g2_font_helvB12_tr);
    u8g2.setFont(u8g2_font_courR10_tr);
    //u8g2.setFont(u8g2_font_courR08_tr);

    u8g2.setDisplayRotation(U8G2_R0);
    u8g2.setFlipMode(0);
    draw_Rotate_Flip("R0, F0");
    u8g2.setFlipMode(1);
    draw_Rotate_Flip("R0, F1");

    u8g2.setDisplayRotation(U8G2_R1);
    u8g2.setFlipMode(0);
    draw_Rotate_Flip("R1, F0");
    u8g2.setFlipMode(1);
    draw_Rotate_Flip("R1, F1");

    u8g2.setDisplayRotation(U8G2_R2);
    u8g2.setFlipMode(0);
    draw_Rotate_Flip("R2, F0");
    u8g2.setFlipMode(1);
    draw_Rotate_Flip("R2, F1");

    u8g2.setDisplayRotation(U8G2_R3);
    u8g2.setFlipMode(0);
    draw_Rotate_Flip("R3, F0");
    u8g2.setFlipMode(1);
    draw_Rotate_Flip("R3, F1");
}

void draw_Rotate_Flip(const char *s){
    u8g2.firstPage();
    do{
        u8g2.drawStr(0, 15, "Hello World!");
        u8g2.drawStr(0, 44, s);
        //u8g2.drawFrame(0, 0, u8g2.getWidth(), u8g2.getDisplayHeight());
    } while(u8g2.nextPage());
    delay(4000);
}

// ****************************************************** //
//                          GUAGE
// ****************************************************** //
/*Gauges 0.1  25 JAN 17
* Giuseppe Bellomo  bellomo.giuseppeATgmail.com
* Draws simple gauge based on a value passed to the function.
*   USAGE
*   Drawgauge(x,y,r,p,v,minVal,maxVal,t )
*   Draws a gauge with center in x,y (x is an int since u8g2lib supports screen with a width of more than 255, y is a byte to keep variable footprint as low as possible);
*   with radius r;
*   needle lenght is p percent of radius;
*   representing value v(must be an int);
*   minVal and maxVal are the min and max values of v (integer);
*   t is gauge type:
*      0 for quarter left,
*      1 for quarter right, anticlockwise
*      2 for upper half semicricle
*      3 for 3/4 starting at -180
*      4 for 3/4 starting at -90
*      very easy to add other versions
*/
void Guage(){
    for(byte t = 0; t <= 4; t++){

        for(int c = 0; c <= 100; c++){  //this loop is for testing needle movement
            Drawgauge(cx, cy, radius, percent, c, 0, 100, t);
        }
    }
    delay(500);
}

void Drawgauge(int x, byte y, byte r, byte p, int v, int minVal, int maxVal, byte t){
    int n = (r / 100.00)*p; // calculate needle percent lenght

    u8g2.firstPage();
    do{
        switch(t){
            case 0:
                { //left quarter
                    float gs_rad = -1.572; //-90 degrees in radiant
                    float ge_rad = 0;
                    float i = ((v - minVal)*(ge_rad - gs_rad) / (maxVal - minVal) + gs_rad);
                    int xp = x + (sin(i) * n);
                    int yp = y - (cos(i) * n);
                    u8g2.drawCircle(x, y, r, U8G2_DRAW_UPPER_LEFT);
                    u8g2.drawLine(x, y, xp, yp);
                }
                break;
            case 1:
                { //right quarter, needle anticlockwise

                    float gs_rad = 0;
                    float ge_rad = 1.572; //90 degrees in radiant
                    float i = ((v - minVal)*(ge_rad - gs_rad) / (maxVal - minVal) + gs_rad);
                    int xp = x + (cos(i) * n);
                    int yp = y - (sin(i) * n);
                    u8g2.drawCircle(x, y, r, U8G2_DRAW_UPPER_RIGHT);
                    u8g2.drawLine(x, y, xp, yp);
                }
                break;
            case 2:
                { //upper half
                    float gs_rad = -1.572;
                    float ge_rad = 1.572;
                    float i = ((v - minVal)*(ge_rad - gs_rad) / (maxVal - minVal) + gs_rad);
                    int xp = x + (sin(i) * n);
                    int yp = y - (cos(i) * n);
                    u8g2.drawCircle(x, y, r, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT);
                    u8g2.drawLine(x, y, xp, yp);
                }
                break;
            case 3:
                { //three quarter starting at -180
                    float gs_rad = -3.142;
                    float ge_rad = 1.572;
                    float i = ((v - minVal)*(ge_rad - gs_rad) / (maxVal - minVal) + gs_rad);
                    int xp = x + (sin(i) * n);
                    int yp = y - (cos(i) * n);
                    u8g2.drawCircle(x, y, r, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_LOWER_LEFT);
                    u8g2.drawLine(x, y, xp, yp);
                }
                break;
            case 4:
                { //three quarter starting at -90
                    float gs_rad = -1.572;
                    float ge_rad = 3.141;
                    float i = ((v - minVal)*(ge_rad - gs_rad) / (maxVal - minVal) + gs_rad);
                    int xp = x + (sin(i) * n);
                    int yp = y - (cos(i) * n);
                    u8g2.drawCircle(x, y, r, U8G2_DRAW_UPPER_LEFT | U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_LOWER_RIGHT);
                    u8g2.drawLine(x, y, xp, yp);
                }
                break;
        }

    } while(u8g2.nextPage());

}

// ****************************************************** //
//                          GRAPHICS
// ****************************************************** //
/*
U8glib Example Overview:
Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.

This is a page buffer example.
*/

void Graphic(){  // picture loop  
    u8g2.firstPage();
    do{
        Graphic_draw();
    } while(u8g2.nextPage());

    // increase the state
    draw_state++;
    if(draw_state >= 14 * 8)
        draw_state = 0;

    // delay between each page
    delay(500);
}

void Graphic_draw(void){
    u8g2_prepare();
    switch(draw_state >> 3){
        case 0: u8g2_box_title(draw_state & 7); break;
        case 1: u8g2_box_frame(draw_state & 7); break;
        case 2: u8g2_disc_circle(draw_state & 7); break;
        case 3: u8g2_r_frame(draw_state & 7); break;
        case 4: u8g2_string(draw_state & 7); break;
        case 5: u8g2_line(draw_state & 7); break;
        case 6: u8g2_triangle(draw_state & 7); break;
        case 7: u8g2_ascii_1(); break;
        case 8: u8g2_ascii_2(); break;
        case 9: u8g2_extra_page(draw_state & 7); break;
        case 10: u8g2_xor(draw_state & 7); break;
        case 11: u8g2_bitmap_modes(0); break;
        case 12: u8g2_bitmap_modes(1); break;
        case 13: u8g2_bitmap_overlay(draw_state & 7); break;
    }
}

void u8g2_prepare(void){
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
}

void u8g2_box_title(uint8_t a){
    u8g2.drawStr(10 + a * 2, 5, "U8g2");
    u8g2.drawStr(10, 20, "GraphicsTest");

    u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
}

void u8g2_box_frame(uint8_t a){
    u8g2.drawStr(0, 0, "drawBox");
    u8g2.drawBox(5, 10, 20, 10);
    u8g2.drawBox(10 + a, 15, 30, 7);
    u8g2.drawStr(0, 30, "drawFrame");
    u8g2.drawFrame(5, 10 + 30, 20, 10);
    u8g2.drawFrame(10 + a, 15 + 30, 30, 7);
}

void u8g2_disc_circle(uint8_t a){
    u8g2.drawStr(0, 0, "drawDisc");
    u8g2.drawDisc(10, 18, 9);
    u8g2.drawDisc(24 + a, 16, 7);
    u8g2.drawStr(0, 30, "drawCircle");
    u8g2.drawCircle(10, 18 + 30, 9);
    u8g2.drawCircle(24 + a, 16 + 30, 7);
}

void u8g2_r_frame(uint8_t a){
    u8g2.drawStr(0, 0, "drawRFrame/Box");
    u8g2.drawRFrame(5, 10, 40, 30, a + 1);
    u8g2.drawRBox(50, 10, 25, 40, a + 1);
}

void u8g2_string(uint8_t a){
    u8g2.setFontDirection(0);
    u8g2.drawStr(30 + a, 31, " 0");
    u8g2.setFontDirection(1);
    u8g2.drawStr(30, 31 + a, " 90");
    u8g2.setFontDirection(2);
    u8g2.drawStr(30 - a, 31, " 180");
    u8g2.setFontDirection(3);
    u8g2.drawStr(30, 31 - a, " 270");
}

void u8g2_line(uint8_t a){
    u8g2.drawStr(0, 0, "drawLine");
    u8g2.drawLine(7 + a, 10, 40, 55);
    u8g2.drawLine(7 + a * 2, 10, 60, 55);
    u8g2.drawLine(7 + a * 3, 10, 80, 55);
    u8g2.drawLine(7 + a * 4, 10, 100, 55);
}

void u8g2_triangle(uint8_t a){
    uint16_t offset = a;
    u8g2.drawStr(0, 0, "drawTriangle");
    u8g2.drawTriangle(14, 7, 45, 30, 10, 40);
    u8g2.drawTriangle(14 + offset, 7 - offset, 45 + offset, 30 - offset, 57 + offset, 10 - offset);
    u8g2.drawTriangle(57 + offset * 2, 10, 45 + offset * 2, 30, 86 + offset * 2, 53);
    u8g2.drawTriangle(10 + offset, 40 + offset, 45 + offset, 30 + offset, 86 + offset, 53 + offset);
}

void u8g2_ascii_1(){
    char s[2] = " ";
    uint8_t x, y;
    u8g2.drawStr(0, 0, "ASCII page 1");
    for(y = 0; y < 6; y++){
        for(x = 0; x < 16; x++){
            s[0] = y * 16 + x + 32;
            u8g2.drawStr(x * 7, y * 10 + 10, s);
        }
    }
}

void u8g2_ascii_2(){
    char s[2] = " ";
    uint8_t x, y;
    u8g2.drawStr(0, 0, "ASCII page 2");
    for(y = 0; y < 6; y++){
        for(x = 0; x < 16; x++){
            s[0] = y * 16 + x + 160;
            u8g2.drawStr(x * 7, y * 10 + 10, s);
        }
    }
}

void u8g2_extra_page(uint8_t a){
    u8g2.drawStr(0, 0, "Unicode");
    u8g2.setFont(u8g2_font_unifont_t_symbols);
    u8g2.setFontPosTop();
    u8g2.drawUTF8(0, 24, "☀ ☁");
    switch(a){
        case 0:
        case 1:
        case 2:
        case 3:
            u8g2.drawUTF8(a * 3, 36, "☂");
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            u8g2.drawUTF8(a * 3, 36, "☔");
            break;
    }
}

void u8g2_xor(uint8_t a){
    uint8_t i;
    u8g2.drawStr(0, 0, "XOR");
    u8g2.setFontMode(1);
    u8g2.setDrawColor(2);
    for(i = 0; i < 5; i++){
        u8g2.drawBox(10 + i * 16, 18 + (i & 1) * 4, 21, 31);
    }
    u8g2.drawStr(5 + a, 19, "XOR XOR XOR XOR");
    u8g2.setDrawColor(0);
    u8g2.drawStr(5 + a, 29, "CLR CLR CLR CLR");
    u8g2.setDrawColor(1);
    u8g2.drawStr(5 + a, 39, "SET SET SET SET");
    u8g2.setFontMode(0);

}

static const unsigned char cross_bits[] U8X8_PROGMEM = {
    0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x42, 0x00,
    0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x81, 0x00, 0x00, 0x81, 0x00,
    0xC0, 0x00, 0x03, 0x38, 0x3C, 0x1C, 0x06, 0x42, 0x60, 0x01, 0x42, 0x80,
    0x01, 0x42, 0x80, 0x06, 0x42, 0x60, 0x38, 0x3C, 0x1C, 0xC0, 0x00, 0x03,
    0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00,
    0x00, 0x42, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x18, 0x00, };

static const unsigned char cross_fill_bits[] U8X8_PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x64, 0x00, 0x26,
    0x84, 0x00, 0x21, 0x08, 0x81, 0x10, 0x08, 0x42, 0x10, 0x10, 0x3C, 0x08,
    0x20, 0x00, 0x04, 0x40, 0x00, 0x02, 0x80, 0x00, 0x01, 0x80, 0x18, 0x01,
    0x80, 0x18, 0x01, 0x80, 0x00, 0x01, 0x40, 0x00, 0x02, 0x20, 0x00, 0x04,
    0x10, 0x3C, 0x08, 0x08, 0x42, 0x10, 0x08, 0x81, 0x10, 0x84, 0x00, 0x21,
    0x64, 0x00, 0x26, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

static const unsigned char cross_block_bits[] U8X8_PROGMEM = {
    0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
    0xC1, 0x20, 0xC1, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
    0x01, 0x20, 0xFF, 0x3F, };

void u8g2_bitmap_overlay(uint8_t a){
    uint8_t frame_size = 28;

    u8g2.drawStr(0, 0, "Bitmap overlay");

    u8g2.drawStr(0, frame_size + 12, "Solid / transparent");
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawFrame(0, 10, frame_size, frame_size);
    u8g2.drawXBMP(2, 12, cross_width, cross_height, cross_bits);
    if(a & 4)
        u8g2.drawXBMP(7, 17, cross_block_width, cross_block_height, cross_block_bits);

    u8g2.setBitmapMode(true /* transparent*/);
    u8g2.drawFrame(frame_size + 5, 10, frame_size, frame_size);
    u8g2.drawXBMP(frame_size + 7, 12, cross_width, cross_height, cross_bits);
    if(a & 4)
        u8g2.drawXBMP(frame_size + 12, 17, cross_block_width, cross_block_height, cross_block_bits);
}

void u8g2_bitmap_modes(uint8_t transparent){
    const uint8_t frame_size = 24;

    u8g2.drawBox(0, frame_size * 0.5, frame_size * 5, frame_size);
    u8g2.drawStr(frame_size * 0.5, 50, "Black");
    u8g2.drawStr(frame_size * 2, 50, "White");
    u8g2.drawStr(frame_size * 3.5, 50, "XOR");

    if(!transparent){
        u8g2.setBitmapMode(false /* solid */);
        u8g2.drawStr(0, 0, "Solid bitmap");
    } else{
        u8g2.setBitmapMode(true /* transparent*/);
        u8g2.drawStr(0, 0, "Transparent bitmap");
    }
    u8g2.setDrawColor(0);// Black
    u8g2.drawXBMP(frame_size * 0.5, 24, cross_width, cross_height, cross_bits);
    u8g2.setDrawColor(1); // White
    u8g2.drawXBMP(frame_size * 2, 24, cross_width, cross_height, cross_bits);
    u8g2.setDrawColor(2); // XOR
    u8g2.drawXBMP(frame_size * 3.5, 24, cross_width, cross_height, cross_bits);
}

// ****************************************************** //
//                          CLOCK
// ****************************************************** //
//
/*
        Demonstrates a clock - like display with a very huge font.
        Also shows usage of u8x8_u8toa().

U8glib Example Overview:
Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.

This is a page buffer example.
*/
void Clock(){
    char m_str[3];
    strcpy(m_str, u8x8_u8toa(m, 2));		/* convert m to a string with two digits */
    u8g2.firstPage();
    do{
        u8g2.setFont(u8g2_font_logisoso42_tn);
        u8g2.drawStr(0, 53, "19");
        u8g2.drawStr(55, 53, ":");
        u8g2.drawStr(70, 53, m_str);
    } while(u8g2.nextPage());
    delay(1000);
    m++;
    if(m == 60)
        m = 0;
}

// ****************************************************** //
//                          XBM
// ****************************************************** //
/*
XBM file start, paste the content of the XBM file directly into the .ino file
 variable name changed from
 static unsigned char u8g2_logo_97x51_bits[] 
 to
 static const unsigned char u8g2_logo_97x51_bits[] U8X8_PROGMEM
*/
static const unsigned char u8g2_logo_97x51_bits[] U8X8_PROGMEM = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x3c, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00,
    0x00, 0x00, 0x3c, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe,
    0x03, 0x00, 0x00, 0x3c, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0x03, 0x00, 0x00, 0x3c, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x80, 0xcf, 0x07, 0x00, 0x00, 0x3c, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x83, 0x07, 0x00, 0x00, 0x3c, 0x80, 0x07, 0xf8, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x81, 0x07, 0x00, 0x00, 0x3c, 0x80, 0x07, 0xfc, 0x03,
    0x1c, 0x00, 0x3e, 0x1c, 0xc0, 0x03, 0x00, 0x00, 0x3c, 0x80, 0x07, 0xff,
    0x07, 0x7f, 0x80, 0xff, 0x3f, 0xe0, 0x01, 0x00, 0x00, 0x3c, 0x80, 0x07,
    0xff, 0x8f, 0xff, 0xc1, 0xff, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x3c, 0x80,
    0x87, 0xff, 0xdf, 0xff, 0xc1, 0xc3, 0x07, 0x7c, 0x00, 0x00, 0x00, 0x3c,
    0x80, 0x87, 0x0f, 0xfe, 0xff, 0xe3, 0x81, 0x03, 0x1e, 0x00, 0x00, 0x00,
    0x3c, 0x80, 0xc7, 0x07, 0xfc, 0xe3, 0xe3, 0x81, 0x07, 0x0f, 0x00, 0x00,
    0x00, 0x3c, 0x80, 0xc7, 0x07, 0xf8, 0xc1, 0xe7, 0x81, 0x87, 0xff, 0x07,
    0x00, 0x00, 0x3c, 0x80, 0xc7, 0x03, 0xf0, 0x80, 0xe7, 0xc3, 0x87, 0xff,
    0x07, 0x00, 0x00, 0x3c, 0x80, 0xc7, 0x03, 0x70, 0x80, 0xc7, 0xe7, 0x83,
    0xff, 0x07, 0x00, 0x00, 0x3c, 0x80, 0xc7, 0x03, 0x78, 0x80, 0xc7, 0xff,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x80, 0xc7, 0x03, 0xf8, 0xc0, 0x87,
    0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x80, 0xc7, 0x07, 0xfc, 0xc1,
    0xc7, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0xc0, 0x87, 0x0f, 0xfe,
    0xff, 0xe3, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0xc0, 0x83, 0xff,
    0xdf, 0xff, 0xe3, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xf1, 0x03,
    0xff, 0x8f, 0xff, 0xe1, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff,
    0x01, 0xfe, 0x0f, 0xff, 0xc0, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xe0,
    0xff, 0x00, 0xfc, 0x03, 0x7c, 0xc0, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00,
    0x80, 0x3f, 0x00, 0xf8, 0x01, 0x00, 0xe0, 0x01, 0x1e, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x1e, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x1e, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xf0, 0xc7, 0x0f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xe0, 0xff,
    0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0xc0,
    0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x01, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x24, 0x20, 0x00,
    0x00, 0x08, 0x46, 0x02, 0x00, 0x80, 0xc0, 0x40, 0x00, 0x0c, 0x6e, 0x6a,
    0xc0, 0xa4, 0x48, 0x04, 0xaa, 0xac, 0x8c, 0xaa, 0xac, 0x00, 0x6a, 0xa4,
    0xaa, 0x20, 0xea, 0xa4, 0x64, 0x66, 0xaa, 0x46, 0x4a, 0x8a, 0x00, 0x4c,
    0xa4, 0xaa, 0x20, 0xaa, 0xa2, 0x44, 0x2a, 0xaa, 0x28, 0xaa, 0x4c, 0x00,
    0xe8, 0xa8, 0x6c, 0xc4, 0xa4, 0x42, 0xee, 0x2a, 0xcc, 0x26, 0x6c, 0xe8,
    0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00 };

void XBM(){
    u8g2.firstPage();
    do{
        u8g2.setFont(u8g2_font_ncenB14_tr);
        u8g2.drawXBMP(0, 0, u8g2_logo_97x51_width, u8g2_logo_97x51_height, u8g2_logo_97x51_bits);
    } while(u8g2.nextPage());
    //delay(1000);
}

// ****************************************************** //
//                      MINI LOGO
// ****************************************************** //
void Mini_Logo(void){
    u8g2.firstPage();
    do{
        drawLogo();
        drawURL();
    } while(u8g2.nextPage());
    delay(4000);
}

void drawLogo(void){
    uint8_t mdy = 0;
    if(u8g2.getDisplayHeight() < 59)
        mdy = 5;


    u8g2.setFontMode(1);	// Transparent
    u8g2.setDrawColor(1);
#ifdef MINI_LOGO

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(0, 22, "U");

    u8g2.setFontDirection(1);
    u8g2.setFont(u8g2_font_inb19_mn);
    u8g2.drawStr(14, 8, "8");

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.drawStr(36, 22, "g");
    u8g2.drawStr(48, 22, "\xb2");

    u8g2.drawHLine(2, 25, 34);
    u8g2.drawHLine(3, 26, 34);
    u8g2.drawVLine(32, 22, 12);
    u8g2.drawVLine(33, 23, 12);
#else

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb24_mf);
    u8g2.drawStr(0, 30 - mdy, "U");

    u8g2.setFontDirection(1);
    u8g2.setFont(u8g2_font_inb30_mn);
    u8g2.drawStr(21, 8 - mdy, "8");

    u8g2.setFontDirection(0);
    u8g2.setFont(u8g2_font_inb24_mf);
    u8g2.drawStr(51, 30 - mdy, "g");
    u8g2.drawStr(67, 30 - mdy, "\xb2");

    u8g2.drawHLine(2, 35 - mdy, 47);
    u8g2.drawHLine(3, 36 - mdy, 47);
    u8g2.drawVLine(45, 32 - mdy, 12);
    u8g2.drawVLine(46, 33 - mdy, 12);

#endif
}

void drawURL(void){
#ifndef MINI_LOGO
    u8g2.setFont(u8g2_font_4x6_tr);
    if(u8g2.getDisplayHeight() < 59){
        u8g2.drawStr(89, 20 - 5, "github.com");
        u8g2.drawStr(73, 29 - 5, "/olikraus/u8g2");
    } else{
        u8g2.drawStr(1, 54, "github.com/olikraus/u8g2");
    }
#endif
}

// ****************************************************** //
//                          XOR Test
// ****************************************************** //
void XOR(void){
    uint8_t i;
    i = 20;
    do{
        testXOR(20 - i);
        delay(15);
        i--;
    } while(i != 0);
    i = 20;
    delay(15);
    do{
        testXOR(i);
        delay(15);
        i--;
    } while(i != 0);
    delay(15);
}

void testXOR(uint8_t x){
    u8g2.setFont(u8g2_font_ncenB14_tr);
    //u8g2.setFont(u8g2_font_courB24_tn);

    u8g2.firstPage();
    do{
        /* Solid font mode with XOR drawing: Does not make much sense */
        u8g2.setFontMode(0);
        u8g2.setDrawColor(1);
        u8g2.drawBox(10, 8, 10, 10);
        u8g2.drawBox(40, 20, 30, 12);
        u8g2.drawBox(100, 8, 4, 22);

        u8g2.setDrawColor(2);
        u8g2.drawStr(x, 26, "XOR Test");
        u8g2.drawHLine(0, 29, 128);

        /* Transparent font mode with XOR drawing: looks good */
        u8g2.setFontMode(1);
        u8g2.setDrawColor(1);
        u8g2.drawBox(10, 8 + 32, 10, 10);
        u8g2.drawBox(40, 20 + 32, 30, 12);
        u8g2.drawBox(100, 8 + 32, 4, 22);
        u8g2.setDrawColor(2);
        u8g2.drawStr(x, 26 + 32, "XOR Test");
        u8g2.drawHLine(0, 29 + 32, 128);


    } while(u8g2.nextPage());
}

// ****************************************************** //
//                      PowerSaveTest
// ****************************************************** //
/*
This test will turn on and off your display for two seconds.
If the text "power off" is visible on your display, then power save mode
does not work with your display.
*/
void Power_Save(void){
    u8g2.setPowerSave(0);
    Power_Save_draw("power on / x");
    u8g2.setPowerSave(1);
    Power_Save_draw("power off / x");
    u8g2.setPowerSave(0);
    Power_Save_draw("power on / +");
    u8g2.setPowerSave(1);
    Power_Save_draw("power off / +");
}

void Power_Save_draw(const char *s){
    u8g2.firstPage();
    do{
        u8g2.drawStr(2, 15, "PowerSaveTest");
        u8g2.drawStr(2, 30, s);
        u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight());
    } while(u8g2.nextPage());
    delay(2000);
}


