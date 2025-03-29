# Modules

## Functional

With Moonbase Modules it is possible to create a new module entirely from one c++ file, e.g. ModuleDemo.h.

* inherits from Module.h
* implement setupDefinition
* implement onUpdate
* add the module in main.cpp
* add the module in menu.svelte

## Technical

### Server

[Instances.h](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/Instances.h) and [Instances.cpp](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/Instances.cpp)

### UI

[Instances.svelte](https://github.com/MoonModules/MoonLight/blob/main/interface/src/routes/system/status/Instances.svelte)
