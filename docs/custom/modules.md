# Modules

<img width="829" alt="image" src="https://github.com/user-attachments/assets/3384f3ba-b5e6-4993-9a8b-80c25878e176" />

## Functional

With Moonbase Modules it is possible to create new module entirely from one c++ class by only defining a json document describing the data structure and a function catching all the changes in the data. Http endpont and websockets are created automatically. There is no need to create any UI code, it is entirely driven by the json document.

This is inspired by WLED usermods, further developed in StarBase and now MoonBase (using the ESP32-Sveltekit infrastructure)

See [Demo](https://moonmodules.org/MoonLight/custom/module/demo/) and [Animations](https://moonmodules.org/MoonLight/custom/module/animations/) as examples

## Technical

* Create a class which inherits from Module
* Call the Module constructor with the name of the module.
    * This name will be used to set up http rest api and webserver sockets
    * See [ModuleDemo.h](https://github.com/ewowi/MoonBase/blob/main/src/custom/ModuleDemo.h)

```
class ModuleDemo : public Module
{
public:

ModuleDemo(PsychicHttpServer *server
      , ESP32SvelteKit *sveltekit
      , FilesService *filesService
    ) : Module("demo", server, sveltekit, filesService) {
        ESP_LOGD("", "ModuleDemo::constructor");
    }
}
```

* Implement function setupDefinition to create a json document with the datastructure
    * Store data on the file system
    * Generate the UI
    * Initialy create the module data

```
void setupDefinition(JsonArray root) override{
    JsonObject property;
    JsonArray details;
    JsonArray values;

    property = root.add<JsonObject>(); property["name"] = "hostName"; property["type"] = "text"; property["default"] = "MoonLight";
    property = root.add<JsonObject>(); property["name"] = "connectionMode"; property["type"] = "select"; property["default"] = "Signal Strength"; values = property["values"].to<JsonArray>();
    values.add("Offline");
    values.add("Signal Strength");
    values.add("Priority");

    property = root.add<JsonObject>(); property["name"] = "savedNetworks"; property["type"] = "array"; details = property["n"].to<JsonArray>();
    {
        property = details.add<JsonObject>(); property["name"] = "SSID"; property["type"] = "text"; property["default"] = "ewtr"; property["min"] = 3; property["max"] = 32; 
        property = details.add<JsonObject>(); property["name"] = "Password"; property["type"] = "password"; property["default"] = "";
    }

}

```

* Implement function onUpdate to define what happens if data changes
    * struct UpdatedItem defines the update (parent property, name of property, value and index (in case of multiple records)
    * This is run in the httpd / webserver task. To run it in the main (application task use runInLoopTask - see [ModuleAnimations](https://github.com/ewowi/MoonBase/blob/main/src/custom/ModuleAnimations.h))

```
void onUpdate(UpdatedItem updatedItem) override
{
  if (updatedItem.name == "lightsOn" || updatedItem.name == "brightness") {
      ESP_LOGD("", "handle %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>());
      FastLED.setBrightness(_state.data["lightsOn"]?_state.data["brightness"]:0);
  } else if (updatedItem.parent == "nodes" && updatedItem.name == "animation") {    
      ESP_LOGD("", "handle %s.%s[%d] %s", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, _state.data["nodes"][updatedItem.index]["animation"].as<String>().c_str());
      animation = _state.data["nodes"][updatedItem.index]["animation"].as<String>();
      compileAndRun(animation);
  } else
      ESP_LOGD("", "no handle for %s.%s[%d] %s (%d %s)", updatedItem.parent.c_str(), updatedItem.name.c_str(), updatedItem.index, updatedItem.value.as<String>().c_str(), _state.data["driverOn"].as<bool>(), _state.data["nodes"][0]["animation"].as<String>());
}
```

* Add the module in [main.cpp](https://github.com/ewowi/MoonBase/blob/main/src/main.cpp)

```
ModuleDemo moduleDemo = ModuleDemo(&server, &esp32sveltekit, &filesService);
...
moduleDemo.begin();
...
moduleDemo.loop();
```

* Add the module in [menu.svelte](https://github.com/ewowi/MoonBase/blob/main/interface/src/routes/menu.svelte) (this will be automated in the future)

```
submenu: [
   {
      title: 'Module Demo',
      icon: BulbIcon,
      href: '/custom/module?module=demo',
      feature: page.data.features.liveanimation,
   },
]
```

* This is all to create a fully functioning new module

Moonbase-Modules is implemented in:

* [Module.h](https://github.com/ewowi/MoonBase/blob/main/src/custom/Module.h) and [Module.cpp](https://github.com/ewowi/MoonBase/blob/main/src/custom/Module.cpp) will generate all the required server code
* [Module.svelte](https://github.com/ewowi/MoonBase/blob/main/interface/src/routes/custom/module/Module.svelte) will deal with the UI
* [MultiInput.svelte](https://github.com/ewowi/MoonBase/blob/main/interface/src/lib/components/custom/MultiInput.svelte) is used by Module.svelte to display the right UI widget based on what is defined in the definition json
* Modifications done in [menu.svelte](https://github.com/ewowi/MoonBase/blob/main/interface/src/routes/menu.svelte) do identify a module by href and not by title alone
