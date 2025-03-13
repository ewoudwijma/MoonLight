/**
    @title     MoonLight
    @file      FixtureService.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/fixture/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef FixtureService_h
#define FixtureService_h

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>
#include <FSPersistence.h>
#include <ESP32SvelteKit.h>

class FixtureState
{
public:

    bool lightsOn;
    uint8_t brightness = UINT8_MAX;
    uint16_t fixture = UINT8_MAX;
    // uint16_t width = UINT16_MAX;
    // uint16_t height = UINT16_MAX;
    // uint16_t depth = UINT16_MAX;
    bool driverOn;
    #if FT_ENABLED(FT_MONITOR)
        bool monitorOn;
    #endif
    // uint8_t pin = UINT8_MAX;

    static void read(FixtureState &state, JsonObject &root);

    static StateUpdateResult update(JsonObject &root, FixtureState &state);

};

class FixtureService : public StatefulService<FixtureState>
{
public:
    FixtureService(PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit);

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
