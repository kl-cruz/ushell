# ushell
Small portable and fully configurable shell for constrained devices based on microcontrollers.<br>
<br>
ushell is suitable solution for microcontrollers and other constrainted devices.
Library uses only one dependency with standard C library. It can be amend
in any time.
ushell options: <br>
 -> Defined line lenght and arguments count<br>
 -> Double tapped tabulator key can tip you some commands (like bash)<br>
 -> Naive autofilling with double tab when inly one function occurs on list<br>
 -> Unlimited user functions<br>
 -> Self defined new line characters (\n and \r\n or other)<br>
<br>
# How to compile
On posix systems it is necessary to install libncurses with developer package:<br>
sudo apt-get install libncurses5-dev <br>
Of course it is necessary to install compiler and related stuff. Then type make and use demo app!
# Lightweight printf
It is good to use ushell with small library provided printf function.
Popular libraries are eg. uClib or format:<br>
https://www.uclibc.org <br>
https://github.com/nejohnson/format <br>
More informations can be found in README.md and LICENSE<br>
<br>
