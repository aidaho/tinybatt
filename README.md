TinyBatt
========
## brief one-line battery information

![screenshot](/raw/master/screenshots/tmux-integration.png)

    The goal of this software is to display battery information in the
format most useful for inclusion in text-based interfaces (read: my tmux
status line).

    Although one can achieve this by piping output of any acpi tool through
gnu toolchain, polling such chain has a bigger footprint and I need
something to play with while learning C.

### Features
*   Battery status indication:
    * charging ![charging](/raw/master/screenshots/charging.png)
    * discharging ![discharging](/raw/master/screenshots/discharging.png)
    * warning ![warning](/raw/master/screenshots/warning.png)
*   Discharge rate indicator ![discharge-rate](/raw/master/screenshots/discharge-rate.png).
    Sometimes after s2ram my notebook starts eating through battery really fast, this
    indicator pops up after discharge rate exceeds specified limit (in watts).

### Notes
*   The exact representation of unicode symbols depends on your fonts.
