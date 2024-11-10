# ESP_ticker
A ticker display leaning heavily on the `MD_Parola` library by `Marco Colli`

See <a href="https://willem.aandewiel.nl/index.php/2020/06/09/an-esp8266-ticker/" target="_blank">this post</a> for
more information.


To upload the data-map to LittleFS look <a href="https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system" target="_blank">here</a> for more information (with `VSCode/PlatformIO`, simply click on the ‘PROJECT TASKS’ icon (the scary mask icon) and select `Upload Filesystem Image`).

### weerlive configuration
In the file `src/WeerliveClass.cpp` in the function `configereFilters()` you can select what you will see on your ESP_ticker.
Don't change anything but the `true` or `false` keywords.

```
void Weerlive::configureFilters()
{
  filter["liveweer"][0]["plaats"]   = true;
  filter["liveweer"][0]["time"]     = false;
  filter["liveweer"][0]["temp"]     = true;   //-- actuele temperatuur in graden Celsius
  filter["liveweer"][0]["gtemp"]    = false;  //-- gevoeld temperatuur
  filter["liveweer"][0]["samenv"]   = true;   //-- samenvatting
  filter["liveweer"][0]["lv"]       = true;   //-- relatieve luchtvochtigheid
  filter["liveweer"][0]["windr"]    = true;   //-- windrichting
  filter["liveweer"][0]["windrgr"]  = false;  //-- windrichting in graden
  filter["liveweer"][0]["windms"]   = false;  //-- windkracht in m/s
  filter["liveweer"][0]["windbft"]  = true;   //-- windkracht in bft
  filter["liveweer"][0]["windknp"]  = false;  //-- windsnelheid in knoppen
  filter["liveweer"][0]["windkmh"]  = false;  //-- windsnelheid in km/h
  filter["liveweer"][0]["luchtd"]   = true;   //-- luchtdruk
  filter["liveweer"][0]["ldmmhg"]   = false;  //-- luchtdruk in mmHg
  filter["liveweer"][0]["dauwp"]    = true;   //-- dauwpunt
  filter["liveweer"][0]["zicht"]    = false;  //-- zicht in meters
  filter["liveweer"][0]["gr"]       = true;   //-- globale (zonne)straling in Watt/M2
  filter["liveweer"][0]["verw"]     = false;  //-- korte dagverwachting
  filter["liveweer"][0]["sup"]      = false;  //-- zon op
  filter["liveweer"][0]["sunder"]   = true;   //-- zon onder
  filter["liveweer"][0]["image"]    = true;   //-- afbeeldingsnaam
  filter["liveweer"][0]["alarm"]    = true;   //-- Geldt er een weerwaarschuwing? 1 = ja, 0 = nee
  filter["liveweer"][0]["lkop"]     = true;   //-- Weerwaarschuwing korte omschrijving
  filter["liveweer"][0]["ltekst"]   = false;  //-- Langere beschrijving van de waarschuwing
  filter["liveweer"][0]["wrschklr"] = false;  //-- KNMI kleurcode voor jouw regio
  filter["liveweer"][0]["wrsch_g"]  = false;  //-- Moment waarop de eerstkomende KNMI-waarschuwing geldt
  filter["liveweer"][0]["wrsch_gts"]= false;  //-- Timestamp van wrsch_g
  filter["liveweer"][0]["wrsch_gc"] = false;  //-- KNMI kleurcode voor de eerstkomende waarschuwing
  //---- week verwachting ---
  for(int i = 0; i < 4; i++)
  {
    filter["wk_verw"][i]["dag"]       = true;   //-- datum van deze dag
    filter["wk_verw"][i]["image"]     = true;   //-- afbeeldingsnaamafbeeldingsnaam
    filter["wk_verw"][i]["max_temp"]  = true;   //-- maximum temperatuur voor de dag
    filter["wk_verw"][i]["min_temp"]  = true;   //-- minimum temperatuur voor de dag
    filter["wk_verw"][i]["windbft"]   = false;   //--  windkracht in Beaufort
    filter["wk_verw"][i]["windkmh"]   = false;   //-- windkracht in km/h
    filter["wk_verw"][i]["windms"]    = false;   //-- windkracht in m/s
    filter["wk_verw"][i]["windknp"]   = false;   //-- windkracht in knopen
    filter["wk_verw"][i]["windrgr"]   = false;   //-- windrichting in graden
    filter["wk_verw"][i]["windr"]     = false;   //-- windrichting
    filter["wk_verw"][i]["neersl_perc_dag"] = true;   //-- kans op neerslag per dag
    filter["wk_verw"][i]["zond_perc_dag"]   = false;   //-- kans op zon per dag
  }

} //  configureFilters()
```

## v1.9.0 (10-11-2024)

You now have two options to get news items on your ESP_ticker!
* newsapi.org [but this source seems not to provide Dutch news anymore]
* mediastack [only 100 requests per month - 3 per day :-( ]

Select which one you want with build-flag in `platformio.ini`

## v1.8.0 (10-07-2024)

I am excited to announce the release of version 1.8.0! This update brings significant improvements to enhance your development experience, especially with **Visual Studio Code** and **PlatformIO**. Here's what's new:

### Enhanced VSCode Compatibility
- **All code is now fully compatible with VSCode (PlatformIO)**: I've restructured the entire codebase to ensure seamless integration with Visual Studio Code and PlatformIO, making your development workflow smoother and more efficient.

### Code Restructuring
- **Migration from `.ino` to `.cpp`**: All `.ino` files have been renamed to `.cpp` and moved to the `src` directory. This change aligns with standard C++ practices and improves code organization and readability.
- **Header Files Creation**: For every `.cpp` file, a corresponding `.h` file has been created in the `include` directory. These header files contain all the (extern) variable declarations and function prototypes, ensuring a clean separation of declarations and definitions. This will help you better manage your code and understand the structure of your project.

### Why move to Visual Studio Code?
Visual Studio Code, coupled with PlatformIO, offers several advantages over the Arduino IDE for larger projects:
- **Modular and Scalable**: VSCode allows you to manage complex projects more efficiently by supporting modular programming and integrating seamlessly with version control systems like Git.
- **Advanced Editing Features**: VSCode provides powerful editing features such as IntelliSense, which offers smart code completion and navigation, making it easier to work with large codebases.
- **Rich Extension Ecosystem**: With a vast array of extensions available on the Visual Studio Code Marketplace, you can customize your development environment to suit your specific needs, from debugging tools to project management utilities.
- **Integrated Terminal**: VSCode includes an integrated terminal, allowing you to execute commands and run scripts without leaving the editor, streamlining your workflow.

### Getting Started with Visual Studio Code
To get started with Visual Studio Code and PlatformIO:
1. **Install Visual Studio Code**: Download and install Visual Studio Code from [https://code.visualstudio.com/](https://code.visualstudio.com/).
2. **Install PlatformIO Extension**: Launch Visual Studio Code, go to the Extensions view (Ctrl+Shift+X), and search for "PlatformIO IDE". Click "Install" to add the extension.
3. **Open Your Project**: Open your project folder (`ESP_ticker` for instance) in Visual Studio Code and start coding!


We believe these updates will greatly enhance your development experience. Happy coding!

## But what if you don't want to go to PlatformIO???
You’re in luck! There’s a Python script called `convert2ArduinoIDE.py` that converts PlatformIO code to a format suitable for the ArduinoIDE. However, be aware that you will miss out on all the benefits of using `VSCode` and `PlatformIO`!

For your convenience, I have already run the script, and you can find an ArduinoIDE-compatible folder in ArduinoIDE. Simply copy the ESP_ticker folder from the ArduinoIDE directory to your Arduino project folder. That’s all—it’s that simple!

After opening the `ESP_ticker.ino` file in the ArduinoIDE start off by readeing the `README.h` file!


## v1.7.4 (08-07-2024)
Prepair for Switch to VSCode / PlatformIO

## v1.7.3 (04-05-2023)
Small changes. Seems returned jsonString from Weerlive.nl has grown bigger..
