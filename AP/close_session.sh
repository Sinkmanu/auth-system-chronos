#!/bin/bash
export DISPLAY=:0.0
su $1 -c 'xfce4-session-logout -l'
