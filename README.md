In this document you will find:

**1. Product overview**

**2. Feature description**

**3. What&#39;s new in this version**

**4. Requirements**

**5. Using VDMSound**

**6. Games compatibility**

**7. Contact and support information**

**Please read this entire document carefully**.    It will save you a lot of time and headaches, and will probably answer many of your current and future questions.

**WARNING** :    There have been, in the past, problems reported on systems running certain software like &quot;Motherboard Monitor&quot;.    These programs use special drivers to give them full, direct I/O access.    While full I/O access does not constitute a replacement for soundcard emulation (the aforementioned drivers do not – and can not – handle IRQs and DMA transactions), it will effectively disable VDMSound as soon as the DOS window goes in full-screen mode.    You might have to uninstall the problematic driver (typically called giveio.sys) or the software that uses it in order for VDMSound to work.    This, unfortunately, is not a problem with VDMSound, and I have no control over it.

**1. PRODUCT OVERVIEW**

VDMSound is a program that overcomes what has probably been the most exasperating limitation of DOS boxes since Windows NT — sound support.    VDMSound is an open, plug-in oriented platform that emulates an MPU-401 interface (for outputting high-quality MIDI music), a SoundBlaster compatible (SB16, SBPro 2, SB2, SBPro, etc.) implementation (for digital sound effects and FM/AdLib music), as well as a standard game-port interface (for playing games with joystick support).    In development are improvements to the existing joystick emulation, and possibly VESA support.

Unlike all the Win9x SoundBlaster ISA &#39;legacy&#39; drivers available from a variety of PCI soundcard manufacturers, VDMSound is not a mere &#39;wrapper&#39; or &#39;bridge&#39; to existing audio hardware.    It is a self-contained, 100% software emulation program that is completely independent of your audio hardware type and settings.    VDMSound works with any soundcard, and will even work on computers that have no audio hardware at all (for instance, instead of outputting sounds through your soundcard using the standard Windows drivers, VDMSound can easily output them to disk).

**2. Feature description**

VDMSound features Microsoft Installer technology for smooth product installation, removal and upgrading.    It also seamlessly integrates with Windows Explorer, bringing your old favourites literally within two mouse clicks from running with sound.    This unique feature allows you to enjoy the classics without spending any of your valuable time changing directories, setting environment variables or tuning memory settings.    Other features include:

- emulation support for a MPU-401 interface, complete with MIDI stream validation, MIDI activity indication (via the keyboard LEDs), and the ability to map MT‑32 music to GM/GS should the need arise;
- emulation support for SB16-compatible and AdLib cards, featuring DirectSound mixing of digital sound effects and FM-synthesis music, while still offering plain old WaveOut as an alternative to high-latency Windows NT 4 DirectSound 3 implementations;
- emulation support for up to two joysticks, ideal for those classic two-player games;
- versatile .ini file support, makes the system highly configurable, letting you tweak performance to satisfy even the most demanding DOS applications and games;
- support for multiple configurations, allowing different emulation settings for different applications or games;
- active support on the VDMSound site, http://ntvdm.cjb.net/.
-

**3. What&#39;s new in this version**

This version (VDMSound 2.0.4) features the following changes and improvements:

- AdLib emulation, now fully integrated with the existing SB16 emulation;
- DirectSound support for audio mixing;
- Added optional low-level CD-ROM support in AUTOEXEC.VDMS for games that try to access the CD directly as part of their copy-protection scheme(s);
- Modified VDMSRun.bat to include the SB model type in the BLASTER variable;
- Added &quot;Tip of the Day&quot; feature.
-

**4. Requirements**

VDMSound will only run on Windows NT 4 SP3, Windows 2000 or Windows XP.    It will not work with Windows 95, 98 or Millennium.    A soundcard is optional (although recommended).    Any soundcard will do (be it ISA, PCI, new or ancient), and as long as other Windows programs (like Media Player, Winamp, etc.) work with your soundcard, so will VDMSound.

**5. Using VDMSound**

- Integration with Windows Explorer:

This is the quickest, safest and most convenient way of running your games with sound.

Right-click on the DOS game executable that you wish to start (any .exe, .com or .bat), and select the « Run with VDMS » action.    A DOS window entitled &quot;VDMSound 2.0.4 - DOS Prompt&quot; will pop up; VDMS will automatically take care of emulation, environment and memory settings for the game.

- Running VDMSound manually:

This method gives you the most control over what is happening inside your DOS box.

You are able, at any moment, to manually invoke VDMSound inside a DOS window by typing dosdrv at your command prompt (you can do this from any directory on your system).    The emulation is active from the moment dosdrv returns control to you until the DOS box closes.

_Should you choose to run VDMS manually, however, you will have to set up the DOS environment and memory options yourself._

**6. Games compatibility**

A DOS-game compatibility list is maintained and is being constantly updated on the VDMSound site under the &quot;Compatibility&quot; section.    This list will tell you what games were tested, with what version of VDMS, and under what version of Windows.    It will also tell you if there are any special tricks you must perform in order to get a game to run properly under Windows.    This list is very valuable, as it will save you a lot of time and headaches should you be unable to get your favourite games or applications to work from the first try.

You are encouraged to contribute yourself with new compatibility information, either on the forum or by e-mail.    If you submit a new game or application compatibility report then you will be credited accordingly.

**7. Contact AND SUPPORT information**

The VDMSound page can be accessed through [http://ntvdm.cjb.net/](http://ntvdm.cjb.net/).    From this page you have access to a variety of resources such as:

- peer-to-peer support through the available forums;
- placing technical support requests;
- placing feature requests;
- reporting bugs;
- browsing DOS-game compatibility lists;
- subscribing to the VDMSound &#39;product updates and new releases&#39; mailing list;
- browsing or downloading the VDMSound source code.

You can also contact the author(s).    For inquiries please e-mail [ntvdm@hotmail.com](mailto:ntvdm@hotmail.com).

