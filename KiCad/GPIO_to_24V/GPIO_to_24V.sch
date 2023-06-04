EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Isolator:TLP627-4 U1
U 1 1 647C32E3
P 2850 1300
F 0 "U1" H 2850 1625 50  0000 C CNN
F 1 "TLP627-4" H 2850 1534 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2550 1100 50  0001 L CIN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=16914&prodName=TLP627" H 2850 1300 50  0001 L CNN
	1    2850 1300
	1    0    0    -1  
$EndComp
$Comp
L Isolator:TLP627-4 U1
U 2 1 647C4A90
P 2850 1950
F 0 "U1" H 2850 2275 50  0000 C CNN
F 1 "TLP627-4" H 2850 2184 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2550 1750 50  0001 L CIN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=16914&prodName=TLP627" H 2850 1950 50  0001 L CNN
	2    2850 1950
	1    0    0    -1  
$EndComp
$Comp
L Isolator:TLP627-4 U1
U 3 1 647C76C9
P 2850 2600
F 0 "U1" H 2850 2925 50  0000 C CNN
F 1 "TLP627-4" H 2850 2834 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2550 2400 50  0001 L CIN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=16914&prodName=TLP627" H 2850 2600 50  0001 L CNN
	3    2850 2600
	1    0    0    -1  
$EndComp
$Comp
L Isolator:TLP627-4 U1
U 4 1 647C9E98
P 2850 3250
F 0 "U1" H 2850 3575 50  0000 C CNN
F 1 "TLP627-4" H 2850 3484 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2550 3050 50  0001 L CIN
F 3 "https://toshiba.semicon-storage.com/info/docget.jsp?did=16914&prodName=TLP627" H 2850 3250 50  0001 L CNN
	4    2850 3250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 1200 2450 1200
Wire Wire Line
	2250 1850 2450 1850
Wire Wire Line
	2250 2500 2450 2500
Wire Wire Line
	2250 3150 2450 3150
$Comp
L power:GND #PWR?
U 1 1 647E4458
P 2450 1400
F 0 "#PWR?" H 2450 1150 50  0001 C CNN
F 1 "GND" V 2455 1272 50  0000 R CNN
F 2 "" H 2450 1400 50  0001 C CNN
F 3 "" H 2450 1400 50  0001 C CNN
	1    2450 1400
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 647E5187
P 2450 2050
F 0 "#PWR?" H 2450 1800 50  0001 C CNN
F 1 "GND" V 2455 1922 50  0000 R CNN
F 2 "" H 2450 2050 50  0001 C CNN
F 3 "" H 2450 2050 50  0001 C CNN
	1    2450 2050
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 647E5D32
P 2450 2700
F 0 "#PWR?" H 2450 2450 50  0001 C CNN
F 1 "GND" V 2455 2572 50  0000 R CNN
F 2 "" H 2450 2700 50  0001 C CNN
F 3 "" H 2450 2700 50  0001 C CNN
	1    2450 2700
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 647E662C
P 2450 3350
F 0 "#PWR?" H 2450 3100 50  0001 C CNN
F 1 "GND" V 2455 3222 50  0000 R CNN
F 2 "" H 2450 3350 50  0001 C CNN
F 3 "" H 2450 3350 50  0001 C CNN
	1    2450 3350
	0    1    1    0   
$EndComp
$Comp
L power:GND1 #PWR?
U 1 1 647E728E
P 3250 1400
F 0 "#PWR?" H 3250 1150 50  0001 C CNN
F 1 "GND1" V 3255 1272 50  0000 R CNN
F 2 "" H 3250 1400 50  0001 C CNN
F 3 "" H 3250 1400 50  0001 C CNN
	1    3250 1400
	0    -1   -1   0   
$EndComp
$Comp
L power:GND1 #PWR?
U 1 1 647E7FBA
P 3250 2050
F 0 "#PWR?" H 3250 1800 50  0001 C CNN
F 1 "GND1" V 3255 1922 50  0000 R CNN
F 2 "" H 3250 2050 50  0001 C CNN
F 3 "" H 3250 2050 50  0001 C CNN
	1    3250 2050
	0    -1   -1   0   
$EndComp
$Comp
L power:GND1 #PWR?
U 1 1 647E87BA
P 3250 2700
F 0 "#PWR?" H 3250 2450 50  0001 C CNN
F 1 "GND1" V 3255 2572 50  0000 R CNN
F 2 "" H 3250 2700 50  0001 C CNN
F 3 "" H 3250 2700 50  0001 C CNN
	1    3250 2700
	0    -1   -1   0   
$EndComp
$Comp
L power:GND1 #PWR?
U 1 1 647E9177
P 3250 3350
F 0 "#PWR?" H 3250 3100 50  0001 C CNN
F 1 "GND1" V 3255 3222 50  0000 R CNN
F 2 "" H 3250 3350 50  0001 C CNN
F 3 "" H 3250 3350 50  0001 C CNN
	1    3250 3350
	0    -1   -1   0   
$EndComp
Text GLabel 1850 1200 0    50   Input ~ 0
IN-1
Text GLabel 1850 1850 0    50   Input ~ 0
IN-2
Text GLabel 1850 2500 0    50   Input ~ 0
IN-3
Text GLabel 1850 3150 0    50   Input ~ 0
IN-4
Text GLabel 3350 1200 2    50   Output ~ 0
Out-1
Text GLabel 3350 1850 2    50   Output ~ 0
Out-2
Text GLabel 3350 2500 2    50   Output ~ 0
Out-3
Text GLabel 3350 3150 2    50   Output ~ 0
Out-4
Wire Wire Line
	3250 1200 3350 1200
Wire Wire Line
	3250 1850 3350 1850
Wire Wire Line
	3250 2500 3350 2500
Wire Wire Line
	3250 3150 3350 3150
Wire Wire Line
	1850 1200 1950 1200
Wire Wire Line
	1850 1850 1950 1850
Wire Wire Line
	1850 2500 1950 2500
Wire Wire Line
	1850 3150 1950 3150
$Comp
L Device:R_Pack04_SIP_Split RN1
U 1 1 6481CC63
P 2100 1200
F 0 "RN1" V 1893 1200 50  0000 C CNN
F 1 "R_Pack04_SIP_Split" V 1984 1200 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP8" V 2020 1200 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 2100 1200 50  0001 C CNN
	1    2100 1200
	0    1    1    0   
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN1
U 2 1 6481DCE4
P 2100 1850
F 0 "RN1" V 1893 1850 50  0000 C CNN
F 1 "R_Pack04_SIP_Split" V 1984 1850 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP8" V 2020 1850 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 2100 1850 50  0001 C CNN
	2    2100 1850
	0    1    1    0   
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN1
U 3 1 6481EDC9
P 2100 2500
F 0 "RN1" V 1893 2500 50  0000 C CNN
F 1 "R_Pack04_SIP_Split" V 1984 2500 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP8" V 2020 2500 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 2100 2500 50  0001 C CNN
	3    2100 2500
	0    1    1    0   
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN1
U 4 1 6481FA8A
P 2100 3150
F 0 "RN1" V 1893 3150 50  0000 C CNN
F 1 "R_Pack04_SIP_Split" V 1984 3150 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP8" V 2020 3150 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 2100 3150 50  0001 C CNN
	4    2100 3150
	0    1    1    0   
$EndComp
Wire Wire Line
	5250 1200 5450 1200
$Comp
L power:GND #PWR?
U 1 1 648AD72D
P 5450 1400
F 0 "#PWR?" H 5450 1150 50  0001 C CNN
F 1 "GND" V 5455 1272 50  0000 R CNN
F 2 "" H 5450 1400 50  0001 C CNN
F 3 "" H 5450 1400 50  0001 C CNN
	1    5450 1400
	0    1    1    0   
$EndComp
$Comp
L power:GND1 #PWR?
U 1 1 648AD745
P 6250 1600
F 0 "#PWR?" H 6250 1350 50  0001 C CNN
F 1 "GND1" H 6255 1427 50  0000 C CNN
F 2 "" H 6250 1600 50  0001 C CNN
F 3 "" H 6250 1600 50  0001 C CNN
	1    6250 1600
	1    0    0    -1  
$EndComp
Text GLabel 4850 1200 0    50   Input ~ 0
IN-1
Text GLabel 6550 1300 2    50   Output ~ 0
Out-1
Wire Wire Line
	4850 1200 4950 1200
$Comp
L Isolator:4N28 U1
U 1 1 648AF0D2
P 5750 1300
F 0 "U1" H 5750 1625 50  0000 C CNN
F 1 "! 4N33 !" H 5750 1534 50  0000 C CNN
F 2 "Package_DIP:DIP-6_W7.62mm" H 5550 1100 50  0001 L CIN
F 3 "https://www.vishay.com/docs/83725/4n25.pdf" H 5750 1300 50  0001 L CNN
	1    5750 1300
	1    0    0    -1  
$EndComp
$Comp
L Diode:1N4148 D?
U 1 1 648B71CF
P 6250 1450
F 0 "D?" V 6204 1530 50  0000 L CNN
F 1 "1N4148" V 6295 1530 50  0000 L CNN
F 2 "Diode_THT:D_DO-35_SOD27_P7.62mm_Horizontal" H 6250 1275 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/1N4148_1N4448.pdf" H 6250 1450 50  0001 C CNN
	1    6250 1450
	0    1    1    0   
$EndComp
Wire Wire Line
	6050 1600 6050 1400
Wire Wire Line
	6250 1300 6550 1300
Wire Wire Line
	6050 1300 6250 1300
Connection ~ 6250 1300
Wire Wire Line
	6050 1600 6250 1600
Connection ~ 6250 1600
$Comp
L Device:R R1
U 1 1 648FD96B
P 5100 1200
F 0 "R1" V 4893 1200 50  0000 C CNN
F 1 "R" V 4984 1200 50  0000 C CNN
F 2 "" V 5030 1200 50  0001 C CNN
F 3 "~" H 5100 1200 50  0001 C CNN
	1    5100 1200
	0    1    1    0   
$EndComp
$EndSCHEMATC
