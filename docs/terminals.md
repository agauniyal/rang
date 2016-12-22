= Terminal Implementation Notes
The following notes do _not_ refer to the ANSI and VT100 terminal standards.
They are based on observations of how the supported terminal emulators actually
behave.

== Supported Terminals
| Name            | $TERM=         |
|-----------------|----------------|
| Eterm           | Eterm          |
| gnome-terminal  | xterm-256color |
| konsole         | xterm          |
| lxterminal      | xterm          |
| pterm (PuTTY)   | xterm          |
| roxterm         | xterm          |
| rxvt            | rxvt           |
| termit          | xterm          |
| xterm           | xterm          |

These terminal emulators are tested during development. Other terminal
emulators approximating the behaviors of xterm or rxvt should also work. If
support for another terminal emulator is desired please open an issue on the
GitHub issue tracker starting with "Feature Request:".

== Input Sequences for Non-Printable Keys
_Note: In the table below the `^` character stands for `\033 - Escapement`_
| Key   | xterm | rxvt  | Eterm |
|:-----:|-------|-------|-------|
| ESC   | ^     | ^     | ^     |
| Up    | ^[A   | ^[A   | ^[A   |
| Down  | ^[B   | ^[B   | ^[B   |
| Right | ^[C   | ^[C   | ^[C   |
| Left  | ^[D   | ^[D   | ^[D   |
| INS   | ^[2~  | ^[2~  | ^[2~  |
| DEL   | ^[3~  | ^[3~  | ^[3~  |
| PgUp  | ^[5~  | ^[5~  | ^[5~  |
| PgDwn | ^[6~  | ^[6~  | ^[6~  |
| HOME  | ^[H   | ^[7~  | ^[7~  |
| END   | ^[F   | ^[8~  | ^[8~  |
| F1    | ^OP   | ^[11~ | ^[11~ |
| F2    | ^OQ   | ^[12~ | ^[12~ |
| F3    | ^OR   | ^[13~ | ^[13~ |
| F4    | ^OS   | ^[14~ | ^[14~ |
| F5    | ^[15~ | ^[15~ | ^[15~ |
| F6    | ^[17~ | ^[17~ | ^[17~ |
| F7    | ^[18~ | ^[18~ | ^[18~ |
| F8    | ^[19~ | ^[19~ | ^[19~ |
| F9    | ^[20~ | ^[20~ | ^[20~ |
| F10   | ^[21~ | ^[21~ | ^[21~ |
| F11   | ^[23~ | ^[23~ | ^[23~ |
| F12   | ^[24~ | ^[24~ | ^[24~ |
| Tab   | \011  | \011  | \011  |
| Enter | \012  | \012  | \012  |
| BKSPC | \177  | \177  | \177  |

=== Exceptions
**termit ($TERM="xterm")**
| Key   | Data  |
|:-----:|-------|
| HOME  | ^OH   |
| END   | ^OF   |

**pterm (PuTTY) ($TERM="xterm")**
| Key   | Data  |
|:-----:|-------|
| HOME  | ^[1~  |
| END   | ^[4~  |
| F1    | ^[11~ |
| F2    | ^[12~ |
| F3    | ^[13~ |
| F4    | ^[14~ |

**lxterminal ($TERM="xterm")**
| Key   | Data  |
|:-----:|-------|
| HOME  | ^OH   |
| END   | ^OF   |

=== Terminal Emulators that Intercept Keys
| Terminal       | Intercepted Keys |
|----------------|------------------|
| roxterm        |    F10 F11       |
| lxterminal     |    F10           |
| konsole        |        F11       |
| gnome-terminal | F1 F10 F11       |
| Windows 10 CMD |        F11       |

=== Observations
F1, F10, and F11 are not safe for program use. F12 is traditionally not used as
it is not available on some keyboard layouts.

To test all of the input sequences it is neccesary to test xterm (all keys),
rxvt (HOME, END, F1-F4), termit (HOME, END), and pterm (HOME, END).

