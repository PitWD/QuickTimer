# QuickTimer / CannaClocky
Arduino (tested on 328 µC) compatible Hard- and Software to get any timings, to grow plants, done.

As you can read in another repository from me: <a href="https://github.com/CannaParts/LetsGrowSmart/blob/main/FastVegaFlowerLowPower.md">To grow Cannabis based on 24h cycles is wasted time and energy.</a>

<br>

**CannaClocky** is made to support any smart and strange but also stupid lightning and watering scenario.

- including sunrise and sunset
- all timers can differ between "day" and "night" of other timers (e.g. for watering, ventilation, etc.)

<br>

**CannaClocky** is functioning without the need of any network connections. All configuration can be made via an onBoard running HMI with support for VT-100 compatible Terminals. So, Linux, macOS and since a short while even Windows, having everything on board to manage and view CannaClocky.  
Untested right now, but terminal-apps for mobile phones will be able to manage CannaClocky, too - as long they can manage VT-100 sequences!

- up to 16 independent timer (on a 328  Arduino)

- support for RTC (DS3231 & DS1307)

- once set, it will reboot after power losses safe back into the right state without the need of manual actions. Even if CannaClocky is in ModBUS-Slave-Mode and the master is dead - CannaClocky will run your predefined emergency settings until the master is back.


<br>

**CannaClocky** has two siblings...

- <a href="https://github.com/PitWD/QuickWater"> CannaWatery</a> to measure and regulate pH, EC, °C, Redox, O2 and level of your water. *All water specific sensors/modules from <a href="https://atlas-scientific.com/"> Atlas-Scientific</a> are supported.* 

- <a href="https://github.com/PitWD/QuickAir"> CannaBreeze</a> to measure and regulate temperature, humidity, CO2, O2. *All climate/air specific sensors/modules from <a href="https://atlas-scientific.com/"> Atlas-Scientific</a> are supported.*

All three together are the base to have a very solid control over your grow. Their ability to function as ModBUS slaves makes it possible to let (multiple of) them act in a bigger context under control of more complex controllers, HMIs and 3rd party products.

<br>

The **loop screen** during runtime. Visualization of the states and "last OnOff" and "next OnOff" times of the running timers.  
With hitting one of the menu keys (the underlined ones) you can jump from the **loop screen** direct into the edit menu of the selected timer.
![Loop Screen](/images/Auswahl_001.png)

<br>

The **main settings menu**. This menu gives you access on generic settings and all timers, even if they're disabled and not visible in the loop screen.  
You can individually setup the color and style of the menu-key and the dimmed text style, to have a nice appearance fitting to your terminal/desktop style. The screenshots are made with an "solarized terminal" just the green is a little greener than from the original solarized green.
![Main Screen](/images/Auswahl_002.png)
Key color changed:
![Main Screen](/images/Auswahl_003.png)
Dim color changed:
![Main Screen](/images/Auswahl_004.png)

<br>

The **timer edit menu**. This menu gives you access to all settings of a timer.  
In this example it's "just" an interval timer in automatic mode. 08:30:00 hours ON and 09:30:00 hours OFF. Active on all days of a week.
![Main Screen](/images/Auswahl_005.png)

<br>

The **timer edit menu** with another timer. 
In this example it's an timer which does different on/off intervals while the state of the main interval is on or off.
![Main Screen](/images/Auswahl_006.png)

<br>

**Cause there are plenty running prototypes I can declare this project as 100% usable - but the project is still under heavy development. Some planned extensions will break the actual structure of the internal eeprom - this will break the user made settings. There is actually no safe "fuse" to prevent that re-flashed µC's with a then broken eeprom will boot in a usable state.**