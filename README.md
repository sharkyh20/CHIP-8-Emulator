# CHIP-8-Emulator
This is just a small little project for me to learn emulation in my spare time, as it's fairly lightweight and can run on just about anything.

I've still been working on KinectToVR, it's just that this one is a little easier (read: actually possible) to run on my laptop away from home.

# DIY
I found documentation and accompaning resources to help make the project here:
http://www.codeslinger.co.uk/pages/projects/chip8.html
https://en.wikipedia.org/wiki/CHIP-8
http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

# Building the Project
Don't download the zip file directly from Github, as last time I tested, it doesn't properly grab the submodules, which are needed to compile the project.

1. Pull the project from git: `git clone --recurse-submodules http://github.com/sharkyh20/CHIP-8-Emulator/`
1. Go to `CHIP-8-Emulator\external\nativefiledialog\build\vs2010` and open the NativeFileDialog.sln
1. Depending on your version of VS, retarget the solution when it prompts (Click OK in the popup window)
1. Compile the `nfd` project, in x64, as both Debug and Release
1. In the `CHIP-8-Emulator\src\CHIP8_EMU` folder, open the Visual Studio solution (I used 2017)
1. Build the project in release or debug (x64) as desired
1. A file dialog box will open on starting the emulator, open any .ch8 rom and it should work
