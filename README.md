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
*   Battery status indication:
    * charging ![charging](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/charging.png)
    * discharging ![discharging](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/discharging.png)
    * warning ![warning](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/warning.png)
*   Discharge rate indicator ![discharge-rate](https://raw.githubusercontent.com/aidaho/tinybatt/master/screenshots/discharge-rate.png).
    Sometimes after s2ram my notebook starts eating through battery really fast, this
    indicator pops up after discharge rate exceeds specified limit (in watts).

### Integration
*   **tmux**: insert `#(/path/to/tinybatt)` somewhere in your status(-left/right) line template

### Installation
*   From source:
**tinybatt** is written in pure C and does not require anything above `libc` and `cmake` to build.
Run the following from the project root directory:

    cmake -G "Unix Makefiles"
    make

That's it! The executable will end up at `src/tinybatt`

### Notes
    The exact representation of unicode symbols depend on your fonts.
