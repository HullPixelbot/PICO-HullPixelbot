#include <Adafruit_NeoPixel.h>

#define PIXELS 12
#define VERSION 1

#define NO_OF_LIGHTS 12

#define TICK_INTERVAL 20

#define NO_OF_GAPS 32


#ifdef ARDUINO_ARCH_ESP32

#define NEOPIN 21

#endif

#ifdef ARDUINO_ARCH_PICO

#define NEOPIN 6

#endif



//#define SERIAL_VERBOSE

//#define DISPLAY_LIGHT_SETTINGS

void setLightColor(byte r, byte g, byte b);

void setAllLilac();

void randomiseLight(byte lightNo);

void randomiseLights();

void setAllLightsOff();

void setupPixels();

void do_lightsOff();

void flickerOn();

void flickerOff();

void renderLights();

void updateLightColours(byte i);

bool transitionComplete();

void updateLightPosition(byte i);

void setFlickerUpdateSpeed(byte speed);

void updateLightFlicker(byte i);

// Start the transition of a light to a new colour

void startLightTransition(byte lightNo, byte speed, byte colourSpeed, byte r, byte g, byte b);

/* Colour Codes

Alice Blue,240,248,255,#f0f8ff
Antique White,250,235,215,#faebd7
Aqua,0,255,255,#00ffff
Aquamarine,127,255,212,#7fffd4
Azure,240,255,255,#f0ffff
Beige,245,245,220,#f5f5dc
Bisque,255,228,196,#ffe4c4
Black* 0,0,0,#000000
Blanched Almond,255,235,205,#ffebcd
Blue*,0,0,255,#0000ff
Blue-Violet,138,43,226,#8a2be2
Brown,165,42,42,#a52a2a
Burlywood,222,184,135,#deb887
Cadet Blue,95,158,160,#5f9ea0
Chartreuse,127,255,0,#7fff00
Chocolate,210,105,30,#d2691e
Coral,255,127,80,#ff7f50
Cornflower Blue,100,149,237,#6495ed
Cornsilk,255,248,220,#fff8dc
Cyan,0,255,255,#00ffff
Dark Blue,0,0,139,#00008b
Dark Cyan,0,139,139,#008b8b
Dark Goldenrod,184,134,11,#b8860b
Dark Gray,169,169,169,#a9a9a9
Dark Green,0,100,0,#006400
Dark Khaki,189,183,107,#bdb76b
Dark Magenta,139,0,139,#8b008b
Dark Olive Green,85,107,47,#556b2f
Dark Orange,255,140,0,#ff8c00
Dark Orchid,153,50,204,#9932cc
Dark Red,139,0,0,#8b0000
Dark Salmon,233,150,122,#e9967a
Dark Sea Green,143,188,143,#8fbc8f
Dark Slate Blue,72,61,139,#483d8b
Dark Slate Gray,47,79,79,#2f4f4f
Dark Turquoise,0,206,209,#00ced1
Dark Violet,148,0,211,#9400d3
Deep Pink,255,20,147,#ff1493
Deep Sky Blue,0,191,255,#00bfff
Dim Gray,105,105,105,#696969
Dodger Blue,30,144,255,#1e90ff
Firebrick,178,34,34,#b22222
Floral White,255,250,240,#fffaf0
Forest Green,34,139,34,#228b22
Fuschia* 255,0,255,#ff00ff
Gainsboro,220,220,220,#dcdcdc
Ghost White,255,250,250,#f8f8ff
Gold,255,215,0,#ffd700
Goldenrod,218,165,32,#daa520
Gray*,128,128,128,#808080
Green* 0,128,0,#008000
Green-Yellow,173,255,47,#adff2f
Honeydew,240,255,240,#f0fff0
Hot Pink,255,105,180,#ff69b4
Indian Red,205,92,92,#cd5c5c
Ivory,255,255,240,#fffff0
Khaki,240,230,140,#f0e68c
Lavender,230,230,250,#e6e6fa
Lavender Blush,255,240,245,#fff0f5
Lawn Green,124,252,0,#7cfc00
Lemon Chiffon,255,250,205,#fffacd
Light Blue,173,216,230,#add8e6
Light Coral,240,128,128,#f08080
Light Cyan,224,255,255,#e0ffff
Light Goldenrod,238,221,130,#eedd82
Light Goldenrod Yellow,250,250,210,#fafad2
Light Gray,211,211,211,#d3d3d3
Light Green,144,238,144,#90ee90
Light Pink,255,182,193,#ffb6c1
Light Salmon,255,160,122,#ffa07a
Light Sea Green,32,178,170,#20b2aa
Light Sky Blue,135,206,250,#87cefa
Light Slate Blue,132,112,255,#8470ff
Light Slate Gray,119,136,153,#778899
Light Steel Blue,176,196,222,#b0c4de
Light Yellow,255,255,224,#ffffe0
Lime*,0,255,0,#00ff00
Lime Green,50,205,50,#32cd32
Linen,250,240,230,#faf0e6
Magenta,255,0,255,#ff00ff
Maroon* 128,0,0,#800000
Medium Aquamarine,102,205,170,#66cdaa
Medium Blue,0,0,205,#0000cd
Medium Orchid,186,85,211,#ba55d3
Medium Purple,147,112,219,#9370db
Medium Sea Green,60,179,113,#3cb371
Medium Slate Blue,123,104,238,#7b68ee
Medium Spring Green,0,250,154,#00fa9a
Medium Turquoise,72,209,204,#48d1cc
Medium Violet-Red,199,21,133,#c71585
Midnight Blue,25,25,112,#191970
Mint Cream,245,255,250,#f5fffa
Misty Rose,255,228,225,#e1e4e1
Moccasin,255,228,181,#ffe4b5
Navajo White,255,222,173,#ffdead
Navy*,0,0,128,#000080
Old Lace,253,245,230,#fdf5e6
Olive* 128,128,0,#808000
Olive Drab,107,142,35,#6b8e23
Orange,255,165,0,#ffa500
Orange-Red,255,69,0,#ff4500
Orchid,218,112,214,#da70d6
Pale Goldenrod,238,232,170,#eee8aa
Pale Green,152,251,152,#98fb98
Pale Turquoise,175,238,238,#afeeee
Pale Violet-Red,219,112,147,#db7093
Papaya Whip,255,239,213,#ffefd5
Peach Puff,255,218,185,#ffdab9
Peru,205,133,63,#cd853f
Pink,255,192,203,#ffc0cb
Plum,221,160,221,#dda0dd
Powder Blue,176,224,230,#b0e0e6
Purple* 128,0,128,#800080
Red*,255,0,0,#ff0000
Rosy Brown,188,143,143,#bc8f8f
Royal Blue,65,105,225,#4169e1
Saddle Brown,139,69,19,#8b4513
Salmon,250,128,114,#fa8072
Sandy Brown,244,164,96,#f4a460
Sea Green,46,139,87,#2e8b57
Seashell,255,245,238,#fff5ee
Sienna,160,82,45,#a0522d
Silver* 192,192,192,#c0c0c0
Sky Blue,135,206,235,#87ceeb
Slate Blue,106,90,205,#6a5acd
Slate Gray,112,128,144,#708090
Snow,255,250,250,#fffafa
Spring Green,0,255,127,#00ff7f
Steel Blue,70,130,180,#4682b4
Tan,210,180,140,#d2b48c
Teal*,0,128,128,#008080
Thistle,216,191,216,#d8bfd8
Tomato,255,99,71,#ff6347
Turquoise,64,224,208,#40e0d0
Violet,238,130,238,#ee82ee
Violet-Red,208,32,144,#d02090
Wheat,245,222,179,#f5deb3
White* 255,255,255,#ffffff
White Smoke,245,245,245,#f5f5f5
Yellow* 255,255,0,#ffff00
Yellow-Green,154,205,50,#9acd32,
*/

enum lightColor
{
	red,
	blue,
	green,
	lilac,
	cyan,
	pink,
	lavender,
	plum,
	lime,
	orange,
	powder_blue,
	purple,
	teal
};

void selectColour(lightColor color, byte *r, byte *g, byte *b);

void pickRandomColour(byte *r, byte *g, byte *b);

void transitionToColor(byte speed, byte r, byte g, byte b);

void transitionToRandomColor();

void flickeringColouredLights(byte r, byte g, byte b, byte min, byte max);

void flickeringColouredLights(lightColor color, byte min, byte max);

void do_set_fade_colour(byte * buffer);

void do_set_brightness(byte * buffer);

void do_set_flickering_colour(byte * buffer);

void do_start_flickering();

void do_stop_flickering();

void do_start_sparkle();

void updateLights();

void updateLightsAndDelay(bool wantDelay);

void updateBusyPixel();

void startBusyPixel(byte red, byte green, byte blue);

void stopBusyPixel();

void displayBusyPixelWait(int ticks, int onTime, byte red, byte green, byte blue);
