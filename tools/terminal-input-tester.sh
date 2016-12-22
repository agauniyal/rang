#!/bin/sh

echo

for x in 1 2 3 4 5 6 7 8 9 10 11 12; do
    echo -n "F$x       : "
    tput kf$x | cat -A
    echo
done

echo

echo -n "Left      : "; tput kcub1 | cat -A; echo;
echo -n "Right     : "; tput kcuf1 | cat -A; echo;
echo -n "Up        : "; tput kcuu1 | cat -A; echo;
echo -n "Down      : "; tput kcud1 | cat -A; echo;

echo

echo -n "Delete    : "; tput kdch1 | cat -A; echo;
echo -n "Home      : "; tput khome | cat -A; echo;
echo -n "End       : "; tput kend | cat -A; echo;
echo -n "Enter     : "; tput kent | cat -A; echo;
echo -n "Backspace : "; tput kbs | cat -A; echo;

