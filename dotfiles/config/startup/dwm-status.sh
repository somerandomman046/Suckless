#!/bin/bash

focused_window() {
    WIN_ID=$(xdotool getactivewindow 2>/dev/null)
    [ -z "$WIN_ID" ] && return
    WIN_CLASS=$(xprop -id "$WIN_ID" WM_CLASS 2>/dev/null | awk -F '"' '{print $4}')
    [ -z "$WIN_CLASS" ] && return
    echo "$WIN_CLASS"
}

battery() {
    BAT="/sys/class/power_supply/BAT0/capacity"
    STATUS="/sys/class/power_supply/BAT0/status"
    
    if [ "$(head -n 3 "$STATUS" | tail -n 1 | awk -F '= ' '{print $2}')" = "Charging" ]; then
        echo " $(cat $BAT)%"
    else
        case $(cat $BAT) in
            100 | [9][0-9]) echo "  $(cat $BAT)%" ;;
            [7-8][0-9]) echo "  $(cat $BAT)%" ;;
            [5-6][0-9]) echo "  $(cat $BAT)%" ;;
            [3-4][0-9]) echo "  $(cat $BAT)%" ;;
            [0-2][0-9]) echo "  $(cat $BAT)%" ;;
            *) echo "  $(cat $BAT)%" ;;
        esac
    fi
}

volume() {
    VOL=$(pactl get-sink-volume @DEFAULT_SINK@ | awk '{print $5}' | tr -d '%')
    MUTE=$(pactl get-sink-mute @DEFAULT_SINK@ | awk -F '[/]' '{print $2}')
    
    if [ "$MUTE" = "0" ]; then
        echo "婢 Muted"
    else
        echo "  ${VOL}%"
    fi
}

brightness() {
    BRIGHT=$(brightnessctl get 2>/dev/null)
    echo "  ${BRIGHT}"
}

weather() {
    TEMP=$(curl -s "https://api.open-meteo.com/v1/(put your location)" | jq '.current_weather.temperature | round' 2>/dev/null)
    echo " ${TEMP}°C"
}

keyboard() {
    LAYOUT=$(setxkbmap -query | awk '/layout:/ {print toupper($2)}')
    echo "  ${LAYOUT}"
}

network() {
    if ip link show | grep -q "state UP"; then
        if ip link show | grep -q "eth0"; then
            echo " Ethernet"
        else
            echo "  Wi-Fi"
        fi
    else
        echo "睊 Disconnected"
    fi
}

clock() {
    date "+%A %d-%m-%Y  %H:%M"
}

while true; do
    STATUS_RIGHT="$(battery)% | $(volume) | $(keyboard) | $(brightness) | $(network) | $(weather)"
    STATUS_CENTER="$(clock)"
    STATUS_LEFT="$(focused_window)"

    xsetroot -name "$STATUS_LEFT$(printf '%*s' 2 '')|$(printf '%*s' 2 '')$STATUS_CENTER$(printf '%*s' 30 '')$STATUS_RIGHT$(printf '%*s' 1 '')"
    sleep 1
done
