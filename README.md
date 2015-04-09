TinyBatt - brief one-line battery information
=============================================
![screenshot](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/tmux-integration.png)

The goal of this software is to display battery information in the
format most useful for inclusion in text-based interfaces (read: my tmux
status line).

Although one can achieve this by piping output of any acpi tool through
gnu toolchain, polling such chain has a bigger footprint and I need
something to play with while learning C.

### Features
*   **Battery status indication:**
    * charging ![charging](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/charging.png)
    * discharging ![discharging](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/discharging.png)
    * warning ![warning](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/warning.png)
*   **Discharge rate indicator** ![discharge-rate](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/discharge-rate.png).
    Sometimes, after s2ram my notebook starts eating through battery really fast, this
    indicator designed to pop up after discharge rate exceeds specified limit (in watts).
*   **Multiple batteries support!** ![multiple-batteries](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/multiple-batteries.png)
*   **Don't like built in indicators?** Define your own with command line arguments! See `tinybatt --help` for details.

### Integration
*   **tmux**: insert `#(/path/to/tinybatt)` somewhere in your status(-left/right) template

### Installation
1.  **Build from source:** `tinybatt` is written in pure C and does not require
    anything above `libc-dev` and `cmake` to build. Run the following from the
    project root directory: `cmake -G "Unix Makefiles" && make`
2.  **Build packages:** run `make package`. You will get DEB, RPM and TGZ packages.`
3.  **Install package:**
    * **Debian:** run `sudo dpkg -i tinybatt*.deb`
    * **Red Hat:** run `sudo rpm -i tinybatt*.rpm`
    * **Manual:** binary will end up at `src/tinybatt`

### Notes
    The exact representation of Unicode symbols depend on your fonts.

### Oops, it crashed
My apologies. In order to help me figure out what's wrong please do the following:

1.  **Build source with debug info:** `cmake -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" && make`
2.  **Run binary under GDB:** execute `gdb src/tinybatt`. You will be greeted with GDB prompt.
    Enter `run`, tinybatt will be executed and after it crashes, show me the output of `backtrace` command.

### Changelog
* **v0.0.3**: Added ability to fine tune output.
* **v0.0.2**: Added support for multiple batteries.
* **v0.0.1**: Initial version.