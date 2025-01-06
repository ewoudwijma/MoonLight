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

#define DEFAULT_LED_STATE false

void EffectsState::read(EffectsState &settings, JsonObject &root)
{
    JsonArray array = root["effects"].to<JsonArray>();
    JsonObject object = array.add<JsonObject>();
    object["name"] = "Ripples effect";
    array.add(object);
    object["name"] = "Lissajous effect";
    array.add(object);
    object["name"] = "Blender";
    array.add(object);
    object["name"] = "PinWheel projection";
    // array.add(object);
}

StateUpdateResult EffectsState::update(JsonObject &root, EffectsState &effectsState)
{
    bool changed = false;

    // boolean newState = root["files_on"];
    // if (effectsState.filesOn != newState) {effectsState.filesOn = newState; changed = true;}

    if (changed)
        Serial.printf("EffectsState::update");

    return changed?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

EffectsService::EffectsService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager, FixtureService *fixtureService) : _httpEndpoint(EffectsState::read,
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
                                                                                                          "effects"),
                                                                                           _webSocketServer(EffectsState::read,
                                                                                                            EffectsState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/effectsState",
                                                                                                            securityManager,
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(socket),
                                                                                            _fixtureService(fixtureService)
{

    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void EffectsService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();

    onConfigUpdated();
}

void EffectsService::onConfigUpdated()
{
    Serial.printf("EffectsService::onConfigUpdated\n");
}

//utility function
float distance(float x1, float y1, float z1, float x2, float y2, float z2) {
  return sqrtf((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}

void EffectsService::loop()
{
    //use the fixtureState to set it's ledsP array
    _fixtureService->read([&](FixtureState &fixtureState) {
        //run an effect

        if (fixtureState.depth > 1) {
            //ripples effect

            uint8_t speed = 50;
            uint8_t interval = 128;

            float ripple_interval = 1.3f * ((255.0f - interval)/128.0f) * sqrtf(fixtureState.height);
            float time_interval = millis()/(100.0 - speed)/((256.0f-128.0f)/20.0f);

            // leds.fill_solid(CRGB::Black);

            uint8_t pos_x = 0;
            uint8_t pos_y = 0;
            uint8_t pos_z = 0;
            for (pos_z=0; pos_z<fixtureState.depth; pos_z++) {
            for (pos_x=0; pos_x<fixtureState.width; pos_x++) {

                float d = distance(fixtureState.width/2.0f, fixtureState.depth/2.0f, 0.0f, (float)pos_x, (float)pos_z, 0.0f) / 9.899495f * fixtureState.height;

                for (pos_y=0; pos_y<fixtureState.height; pos_y++) {
                    uint16_t index = pos_x + pos_y * fixtureState.width + pos_z * fixtureState.width * fixtureState.height;

                    uint8_t pixely = floor(fixtureState.height/2.0f * (1 + sinf(d/ripple_interval + time_interval))); //between 0 and fixtureState.height-1
                    // uint8_t pixely = beatsin8(60, 0, fixtureState.height-1); //between 0 and fixtureState.height
                    // uint8_t pixely = map((millis()/200)%10, 0, 9, 0, fixtureState.height-1); 

                    if (index < STARLIGHT_MAXLEDS) {
                        if (pixely == pos_y)
                            fixtureState.ledsP[index] = CHSV( millis()/50 + random8(64), 200, 255);// ColorFromPalette(leds.palette,call, bri);
                        else
                            fixtureState.ledsP[index] = CRGB::Black;
                        // fixtureState.ledsP[index] = CRGB(pos_x, pos_y, pos_z);
                    }
                }
            }
            }
        }
        else { //lissajous effect
            uint8_t xFrequency = 64;
            uint8_t fadeRate = 128;
            uint8_t speed = 128;
            bool smooth = false; 

            // leds.fadeToBlackBy(fadeRate);
            fadeToBlackBy(fixtureState.ledsP, fixtureState.width*fixtureState.height*fixtureState.depth, fadeRate);

            uint_fast16_t phase = millis() * speed / 256;  // allow user to control rotation speed, speed between 0 and 255!

            uint8_t locn_x = 0;
            uint8_t locn_y = 0;
            uint8_t locn_z = 0;
            for (int i=0; i < 256; i ++) {
                //WLEDMM: stick to the original calculations of xlocn and ylocn
                locn_x = sin8(phase/2 + (i*xFrequency)/64);
                locn_y = cos8(phase/2 + i*2);
                locn_x = (fixtureState.width < 2) ? 1 : (map(2*locn_x, 0,511, 0,2*(fixtureState.width-1)) +1) /2;    // softhack007: "*2 +1" for proper rounding
                locn_y = (fixtureState.height < 2) ? 1 : (map(2*locn_y, 0,511, 0,2*(fixtureState.height-1)) +1) /2;    // "fixtureState.height > 2" is needed to avoid div/0 in map()
                // leds.setPixelColor((uint8_t)xlocn, (uint8_t)ylocn, leds.color_from_palette(millis()/100+i, false, PALETTE_SOLID_WRAP, 0));
                uint16_t index = locn_x + locn_y * fixtureState.width + locn_z * fixtureState.width * fixtureState.height;
                if (index < STARLIGHT_MAXLEDS)
                    fixtureState.ledsP[index] = ColorFromPalette(RainbowColors_p, millis()/100+i);
            }        
        }

    });
}

