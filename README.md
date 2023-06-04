![img](https://github.com/tpecholt/imrad/actions/workflows/cmake.yml/badge.svg)

# ImRAD

ImRAD is a GUI builder for the ImGui library. It generates and parses C++ code.  

ImRAD runs on Windows and Linux. 

# Features

ImRAD is under active development but these are the main features:

* supports wide range of widgets (WIP)
  
  * basic widgets like `Text`, `Checkbox`, `Combo`, `Button`, `Slider`, `ColorEdit` etc.
  * container widgets like `Child`, `Table`, `CollapsingHeader`, `TreeNode`, `TabBar`
  * `MenuBar` editing
  * `CustomWidget` (a placeholder to user code)

* generates layout using `SameLine`/`Spacing`/`NextColumn`/`BeginGroup` instead of absolute positioning 
  
  * This ensures widgets respect item spacing and frame padding in a consistent way
  * There is a clear relationship between parent - child widget as well as children ordering which is important for container widgets like Table

* supports property binding 
  
  * class variables can be managed through simple class wizard or from binding dialog
  * property binding is important because ImGui is immediate mode GUI library so widget states like input text or combobox items must be set at the time of drawing from within the generated code. 
  * using property binding generated UI becomes dynamic and yet it can still be designed at the same time  

* supports generating event handlers and other support code
  
  * for example modal dialog will generate `OpenPopup` member function with a lambda callback called when dialog is closed
  * event handlers allow event handling user code to be separated from the generated part so the designer still works

* target window style is fully configurable
  * colors, style variables and used fonts can be configured via INI file in the `style` folder
  * ImRAD will follow style settings when designing your UI
  * stored style can be loaded in your app by using simple `imrad.h` function  

* generated code is delimited by comment markers and user is free to add additional code around and continue to use ImRAD at the same time
  
  * advanced widget positioning can be implemented this way - see the calculator example (todo)
  * it is also possible to use `CustomWidget` which will just call to a user code callback

* generated code is ready to use in your project and depends only on ImGui library and one accompanying header file (imrad.h)
  
  * for Image widget imrad.h takes an optional dependency to stb and GLFW libraries. This can be activated by defining `IMRAD_WITH_GLFW_TEXTURE` project wide
  * you are free to supply your own texture loading code if you target different backed
  * optional support for the popular `fmt` library can be activated by defining `IMRAD_WITH_FMT`. This will allow you to use formating flags throughout all string properties  

* ImRAD tracks changes to the opened files so files can be designed in ImRAD and edited in your IDE of choice at the same time
  
  * maybe the auto-save feature would be useful to have 

# License

* ImRAD source code is licensed under the GPL license 
* Any code generated by the tool is excluded from GPL and can be included in any project either open-source or commercial and it's up to the user to decide the license for it. 
* Additionally since `imrad.h` is used by the generated code it is also excluded from the GPL license  

# Download binaries

for up-to date version clone & build the repository using CMake. 

Somewhat older version can be downloaded from [Releases](https://github.com/tpecholt/imrad/releases)

# How to build

## Windows
1. Use CMake GUI to configure and generate sln file
2. Open the generated sln file in Visual Studio 2017 or newer (you can use Express or Community editions which are downloadable for free)
3. Build the INSTALL project
4. If you didn't alter CMAKE_INSTALL_PREFIX variable ImRAD will be installed into *C:\Program Files\imrad\latest*

## Linux
1. Run the provided installation script (script parameter is the ImRAD version you want to name the folder) 

   ```sudo ./release-linux 0.5```

2. ImRAD will be installed into *./install/imrad-0.5*

# Screenshots

![screen1](https://github.com/tpecholt/imrad/blob/main/doc/screen1.png)

# More information

Please check [wiki](https://github.com/tpecholt/imrad/wiki) for tutorials and more detailed content

# Credits

Design and implementation - [Tomas Pecholt](https://github.com/tpecholt)

Thanks to [Omar Cornut for Dear ImGui](https://github.com/ocornut/imgui)
