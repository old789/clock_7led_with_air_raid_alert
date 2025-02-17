# clock_7led_with_air_raid_alert
 A digital clock with an air raid alerter

Based on the Node MCU with an TM1637 display.

Годинник з індикатором повітряної тревоги та термометром. Побудовано на ESP8266 Node MCU. Інформація про стан повітряних тривог береться з [API Ubilling](https://wiki.ubilling.net.ua/doku.php?id=aerialalertsapi). Інформація про час береться з NTP, але можлива і автономна робота ( без WiFi ), але в цьому режимі індикація повітряної тривоги не працює. Яскравість дисплея та індикатора повітряної тривоги регулюється автоматично за допомогою фоторезистора.

### Налаштування
Налаштування робиться через інтерфейс командного рядка. Режим налаштування вмикається при завантаженні з включенним відповідним перемикачом.

| Command *arg* | Explanation |
| --- | --- |
| ssid *word* | Set WiFi SSID |
| passw *word* | Set WiFi password |
| host *word* | Set NTP host/pool address |
| region *number* | Область, в який відслідковується стан повітряних тривог |
| tzdata *word* | Установка таймзони ( для Київа EET-2EEST,M3.5.0/3,M10.5.0/4 )|
| poll *number* | Set NTP poll |
| show | Show current configuration |
| save | Save configuration to EEPROM |
| reboot [ *hard* or *soft* ] | Reboot, *hard* doing ESP.reset(), *soft* doing ESP.restart(), default is *soft* |
| help | Get help |


### Додаткова індикація
|^v^v| Підключення до WiFi |
|not| "No time", немає NTP синхронізації |
|noA| "No answer", не відповідає API Ubilling |
  