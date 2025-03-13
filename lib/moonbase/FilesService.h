/**
    @title     MoonLight
    @file      FilesService.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef FilesService_h
#define FilesService_h

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>
#include <ESP32SvelteKit.h>

class FilesState
{
public:
    std::vector<String> changedFiles;

    static void read(FilesState &settings, JsonObject &root);

    static StateUpdateResult update(JsonObject &root, FilesState &filesState);

};

class FilesService : public StatefulService<FilesState>
{
public:
    FilesService(PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit);

    void begin();

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<FilesState> _httpEndpoint;
    EventEndpoint<FilesState> _eventEndpoint;
    WebSocketServer<FilesState> _webSocketServer;
    PsychicHttpServer *_server;

    void onConfigUpdated();
};

#endif
