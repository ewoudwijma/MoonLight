#ifndef FixtureService_h
#define FixtureService_h

/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>
#include <FSPersistence.h>

class FixtureState
{
public:

    bool lightsOn;
    uint8_t brightness = UINT8_MAX;
    uint16_t width = UINT16_MAX;
    uint16_t height = UINT16_MAX;
    uint16_t depth = UINT16_MAX;
    bool driverOn;
    bool monitorOn;
    uint8_t pin = UINT8_MAX;

    static void read(FixtureState &state, JsonObject &root);

    static StateUpdateResult update(JsonObject &root, FixtureState &state);

};

class FixtureService : public StatefulService<FixtureState>
{
public:
    FixtureService(PsychicHttpServer *server,
                      EventSocket *socket,
                      SecurityManager *securityManager, FS *fs);

    void begin();
    void loop50ms();

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<FixtureState> _httpEndpoint;
    EventEndpoint<FixtureState> _eventEndpoint;
    WebSocketServer<FixtureState> _webSocketServer;
    FSPersistence<FixtureState> _fsPersistence;

    void onConfigUpdated();
};

#endif
