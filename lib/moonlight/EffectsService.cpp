/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *file
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <EffectsService.h>

#include <LittleFS.h>

#define DEFAULT_LED_STATE false

void EffectsState::read(EffectsState &settings, JsonObject &root)
{
    JsonArray array = root["effects"].to<JsonArray>();
    JsonObject object = array.add<JsonObject>();
    object["name"] = "Ripples effect";
    array.add(object);
    object["name"] = "Paintbrush effect";
    array.add(object);
    object["name"] = "Blender";
    array.add(object);
    object["name"] = "PinWheel projection";
    // array.add(object);
}

StateUpdateResult EffectsState::update(JsonObject &root, EffectsState &effectsService)
{
    //this must be changed to make it files specific
    boolean newState = root["files_on"] | DEFAULT_LED_STATE;
    if (effectsService.filesOn != newState)
    {
        effectsService.filesOn = newState;
        return StateUpdateResult::CHANGED;
    }
    return StateUpdateResult::UNCHANGED;
}

EffectsService::EffectsService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager) : _httpEndpoint(EffectsState::read,
                                                                                                         EffectsState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/effectsState",
                                                                                                         securityManager,
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(EffectsState::read,
                                                                                                          EffectsState::update,
                                                                                                          this,
                                                                                                          socket,
                                                                                                          "files"),
                                                                                           _webSocketServer(EffectsState::read,
                                                                                                            EffectsState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/effectsState",
                                                                                                            securityManager,
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(socket)
{

    // configure settings service update handler to update LED state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void EffectsService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _state.filesOn = DEFAULT_LED_STATE;

    FastLED.setMaxPowerInMilliWatts(10 * 1000); // 5v, 2000mA
    FastLED.setBrightness(10);

    FastLED.addLeds<NEOPIXEL, 16>(_state.ledsP, 0, 256).setCorrection(TypicalLEDStrip);

    onConfigUpdated();

}

void EffectsService::onConfigUpdated()
{
    // digitalWrite(LED_BUILTIN, _state.filesOn ? 1 : 0);
}

//utility function
float distance(float x1, float y1, float z1, float x2, float y2, float z2) {
  return sqrtf((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}

void EffectsService::loop()
{
    //run an effect

    uint8_t size_x = 10;
    uint8_t size_y = 10;
    uint8_t size_z = 10;

    //ripples effect
    if (true) {

    uint8_t speed = 50;
    uint8_t interval = 128;

    float ripple_interval = 1.3f * ((255.0f - interval)/128.0f) * sqrtf(size_y);
    float time_interval = millis()/(100.0 - speed)/((256.0f-128.0f)/20.0f);

    // leds.fill_solid(CRGB::Black);

    uint8_t pos_x = 0;
    uint8_t pos_y = 0;
    uint8_t pos_z = 0;
    for (pos_z=0; pos_z<size_z; pos_z++) {
      for (pos_x=0; pos_x<size_x; pos_x++) {

        float d = distance(size_x/2.0f, size_z/2.0f, 0.0f, (float)pos_x, (float)pos_z, 0.0f) / 9.899495f * size_y;

        for (pos_y=0; pos_y<size_y; pos_y++) {
            uint16_t index = pos_x + pos_y * size_x + pos_z * size_x * size_y;

            uint8_t pixely = floor(size_y/2.0f * (1 + sinf(d/ripple_interval + time_interval))); //between 0 and size_y-1
            // uint8_t pixely = beatsin8(60, 0, size_y-1); //between 0 and size_y
            // uint8_t pixely = map((millis()/200)%10, 0, 9, 0, size_y-1); 

            if (pixely == pos_y)
                _state.ledsP[index] = CHSV( millis()/50 + random8(64), 200, 255);// ColorFromPalette(leds.palette,call, bri);
            else
                _state.ledsP[index] = CRGB::Black;
            // _state.ledsP[index] = CRGB(pos_x, pos_y, pos_z);
        }
        // array.add(_state.ledsP[index].r);
        // array.add(_state.ledsP[index].g);
        // array.add(_state.ledsP[index].b);
      }
    }
    }
    else {
        size_x = 16;
        size_y = 16;
        size_z = 1;
        uint8_t xFrequency = 64;
        uint8_t fadeRate = 128;
        uint8_t speed = 128;
        bool smooth = false; 

        // leds.fadeToBlackBy(fadeRate);
        fadeToBlackBy(_state.ledsP, size_x*size_y*size_z, fadeRate);

        uint_fast16_t phase = millis() * speed / 256;  // allow user to control rotation speed, speed between 0 and 255!

        uint8_t locn_x = 0;
        uint8_t locn_y = 0;
        uint8_t locn_z = 0;
        for (int i=0; i < 256; i ++) {
            //WLEDMM: stick to the original calculations of xlocn and ylocn
            locn_x = sin8(phase/2 + (i*xFrequency)/64);
            locn_y = cos8(phase/2 + i*2);
            locn_x = (size_x < 2) ? 1 : (map(2*locn_x, 0,511, 0,2*(size_x-1)) +1) /2;    // softhack007: "*2 +1" for proper rounding
            locn_y = (size_y < 2) ? 1 : (map(2*locn_y, 0,511, 0,2*(size_y-1)) +1) /2;    // "size_y > 2" is needed to avoid div/0 in map()
            // leds.setPixelColor((uint8_t)xlocn, (uint8_t)ylocn, leds.color_from_palette(millis()/100+i, false, PALETTE_SOLID_WRAP, 0));
            uint16_t index = locn_x + locn_y * size_x + locn_z * size_x * size_y;
            _state.ledsP[index] = ColorFromPalette(RainbowColors_p, millis()/100+i);
        }        
    }

    // Serial.printf(".");
    FastLED.show();
    // FastLED.delay(1000 / 60);
}

