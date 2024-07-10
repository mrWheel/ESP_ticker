# ESP_ticker
A ticker display leaning heavily on the MD_Parola library by Marco Colli

See <a href="https://willem.aandewiel.nl/index.php/2020/06/09/an-esp8266-ticker/" target="_blank">this post</a> for
more information.


To upload the data-map to LittleFS look <a href="https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system" target="_blank">here</a> for more information.

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


## v1.7.4 (08-07-2024)
Prepair for Switch to VSCode / PlatformIO

## v1.7.3 (04-05-2023)
Small changes. Seems returned jsonString from Weerlive.nl has grown bigger..
