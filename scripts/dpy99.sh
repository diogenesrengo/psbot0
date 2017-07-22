#!/usr/bin/bash
# create a virtual X display (:99), start a vncserver and vncviewer, run a
# window manager and a terminal. needed programs: x11vnc, tigervnc or other.
# author: rngo

export DISPLAY=:99
Xvfb :99 -screen 0 1024x600x24 &
sleep 3
fluxbox &
# secure vnc: need set password in file with 'x11vnc -storepasswd password file'
#x11vnc -display :99 -bg -listen localhost -xkb -forever -rfbauth .vnc/passwd &
x11vnc -display :99 -bg -listen localhost -xkb -forever &
rxvt &
sleep 3
DISPLAY=:0 vncviewer :0

