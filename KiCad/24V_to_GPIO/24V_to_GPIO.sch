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
Wire Wire Line
	2350 1400 2550 1400
Wire Wire Line
	2350 2450 2550 2450
Wire Wire Line
	2350 3500 2550 3500
Wire Wire Line
	2350 4550 2550 4550
$Comp
L power:GND #PWR?
U 1 1 647E5187
P 3150 1400
F 0 "#PWR?" H 3150 1150 50  0001 C CNN
F 1 "GND" V 3155 1272 50  0000 R CNN
F 2 "" H 3150 1400 50  0001 C CNN
F 3 "" H 3150 1400 50  0001 C CNN
	1    3150 1400
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 647E5D32
P 3150 3500
F 0 "#PWR?" H 3150 3250 50  0001 C CNN
F 1 "GND" V 3155 3372 50  0000 R CNN
F 2 "" H 3150 3500 50  0001 C CNN
F 3 "" H 3150 3500 50  0001 C CNN
	1    3150 3500
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 647E662C
P 3150 4550
F 0 "#PWR?" H 3150 4300 50  0001 C CNN
F 1 "GND" V 3155 4422 50  0000 R CNN
F 2 "" H 3150 4550 50  0001 C CNN
F 3 "" H 3150 4550 50  0001 C CNN
	1    3150 4550
	0    -1   -1   0   
$EndComp
Text GLabel 1950 1400 0    50   Input ~ 0
IN-1
Text GLabel 1950 2450 0    50   Input ~ 0
IN-2
Text GLabel 1950 3500 0    50   Input ~ 0
IN-3
Text GLabel 1950 4550 0    50   Input ~ 0
IN-4
Text GLabel 3350 1200 2    50   Output ~ 0
Out-1
Text GLabel 3350 2250 2    50   Output ~ 0
Out-2
Text GLabel 3350 3300 2    50   Output ~ 0
Out-3
Text GLabel 3350 4350 2    50   Output ~ 0
Out-4
Wire Wire Line
	3250 1200 3350 1200
Wire Wire Line
	3250 2250 3350 2250
Wire Wire Line
	3250 3300 3350 3300
Wire Wire Line
	3250 4350 3350 4350
Wire Wire Line
	1950 1400 2050 1400
Wire Wire Line
	1950 2450 2050 2450
Wire Wire Line
	1950 3500 2050 3500
Wire Wire Line
	1950 4550 2050 4550
$Comp
L Device:R_Pack04_SIP_Split RN1
U 1 1 6481CC63
P 2200 1400
F 0 "RN1" V 1993 1400 50  0000 C CNN
F 1 "R_Pack04_SIP_Split" V 2084 1400 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP8" V 2120 1400 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 2200 1400 50  0001 C CNN
	1    2200 1400
	0    1    1    0   
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN1
U 2 1 6481DCE4
P 2200 2450
F 0 "RN1" V 1993 2450 50  0000 C CNN
F 1 "R_Pack04_SIP_Split" V 2084 2450 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP8" V 2120 2450 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 2200 2450 50  0001 C CNN
	2    2200 2450
	0    1    1    0   
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN1
U 3 1 6481EDC9
P 2200 3500
F 0 "RN1" V 1993 3500 50  0000 C CNN
F 1 "R_Pack04_SIP_Split" V 2084 3500 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP8" V 2120 3500 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 2200 3500 50  0001 C CNN
	3    2200 3500
	0    1    1    0   
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN1
U 4 1 6481FA8A
P 2200 4550
F 0 "RN1" V 1993 4550 50  0000 C CNN
F 1 "R_Pack04_SIP_Split" V 2084 4550 50  0000 C CNN
F 2 "Resistor_THT:R_Array_SIP8" V 2120 4550 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 2200 4550 50  0001 C CNN
	4    2200 4550
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 648AD72D
P 6050 1400
F 0 "#PWR?" H 6050 1150 50  0001 C CNN
F 1 "GND" V 6055 1272 50  0000 R CNN
F 2 "" H 6050 1400 50  0001 C CNN
F 3 "" H 6050 1400 50  0001 C CNN
	1    6050 1400
	0    -1   -1   0   
$EndComp
Text GLabel 6550 1300 2    50   Output ~ 0
Out-1
$Comp
L Isolator:4N28 U1
U 1 1 648AF0D2
P 5750 1300
F 0 "U1" H 5750 1625 50  0000 C CNN
F 1 "4N28" H 5750 1534 50  0000 C CNN
F 2 "Package_DIP:DIP-6_W7.62mm" H 5550 1100 50  0001 L CIN
F 3 "https://www.vishay.com/docs/83725/4n25.pdf" H 5750 1300 50  0001 L CNN
	1    5750 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	6050 1300 6400 1300
$Comp
L Device:R R1
U 1 1 648FD96B
P 5300 1400
F 0 "R1" V 5093 1400 50  0000 C CNN
F 1 "R" V 5184 1400 50  0000 C CNN
F 2 "" V 5230 1400 50  0001 C CNN
F 3 "~" H 5300 1400 50  0001 C CNN
	1    5300 1400
	0    1    1    0   
$EndComp
Wire Wire Line
	5050 1400 5150 1400
Text GLabel 5050 1400 0    50   Input ~ 0
IN-1
$Comp
L Device:R R?
U 1 1 647D8FE8
P 6400 1150
F 0 "R?" V 6193 1150 50  0000 C CNN
F 1 "R" V 6284 1150 50  0000 C CNN
F 2 "" V 6330 1150 50  0001 C CNN
F 3 "~" H 6400 1150 50  0001 C CNN
	1    6400 1150
	-1   0    0    1   
$EndComp
Connection ~ 6400 1300
Wire Wire Line
	6400 1300 6550 1300
$Comp
L power:+5V #PWR?
U 1 1 647DAE23
P 6400 1000
F 0 "#PWR?" H 6400 850 50  0001 C CNN
F 1 "+5V" H 6415 1173 50  0000 C CNN
F 2 "" H 6400 1000 50  0001 C CNN
F 3 "" H 6400 1000 50  0001 C CNN
	1    6400 1000
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR?
U 1 1 647DCF3F
P 5450 1050
F 0 "#PWR?" H 5450 900 50  0001 C CNN
F 1 "+24V" H 5465 1223 50  0000 C CNN
F 2 "" H 5450 1050 50  0001 C CNN
F 3 "" H 5450 1050 50  0001 C CNN
	1    5450 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 1050 5450 1200
$Comp
L Isolator:PC847 U?
U 1 1 647E69DC
P 2850 1300
F 0 "U?" H 2850 1625 50  0000 C CNN
F 1 "PC847" H 2850 1534 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2650 1100 50  0001 L CIN
F 3 "http://www.soselectronic.cz/a_info/resource/d/pc817.pdf" H 2850 1300 50  0001 L CNN
	1    2850 1300
	1    0    0    -1  
$EndComp
$Comp
L Isolator:PC847 U?
U 2 1 647E8B91
P 2850 2350
F 0 "U?" H 2850 2675 50  0000 C CNN
F 1 "PC847" H 2850 2584 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2650 2150 50  0001 L CIN
F 3 "http://www.soselectronic.cz/a_info/resource/d/pc817.pdf" H 2850 2350 50  0001 L CNN
	2    2850 2350
	1    0    0    -1  
$EndComp
$Comp
L Isolator:PC847 U?
U 3 1 647EA300
P 2850 3400
F 0 "U?" H 2850 3725 50  0000 C CNN
F 1 "PC847" H 2850 3634 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2650 3200 50  0001 L CIN
F 3 "http://www.soselectronic.cz/a_info/resource/d/pc817.pdf" H 2850 3400 50  0001 L CNN
	3    2850 3400
	1    0    0    -1  
$EndComp
$Comp
L Isolator:PC847 U?
U 4 1 647EAF4D
P 2850 4450
F 0 "U?" H 2850 4775 50  0000 C CNN
F 1 "PC847" H 2850 4684 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 2650 4250 50  0001 L CIN
F 3 "http://www.soselectronic.cz/a_info/resource/d/pc817.pdf" H 2850 4450 50  0001 L CNN
	4    2850 4450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 6481EFAF
P 3150 2450
F 0 "#PWR?" H 3150 2200 50  0001 C CNN
F 1 "GND" V 3155 2322 50  0000 R CNN
F 2 "" H 3150 2450 50  0001 C CNN
F 3 "" H 3150 2450 50  0001 C CNN
	1    3150 2450
	0    -1   -1   0   
$EndComp
$Comp
L power:+24V #PWR?
U 1 1 64821F55
P 2550 1050
F 0 "#PWR?" H 2550 900 50  0001 C CNN
F 1 "+24V" H 2565 1223 50  0000 C CNN
F 2 "" H 2550 1050 50  0001 C CNN
F 3 "" H 2550 1050 50  0001 C CNN
	1    2550 1050
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR?
U 1 1 64822A4C
P 2550 2100
F 0 "#PWR?" H 2550 1950 50  0001 C CNN
F 1 "+24V" H 2565 2273 50  0000 C CNN
F 2 "" H 2550 2100 50  0001 C CNN
F 3 "" H 2550 2100 50  0001 C CNN
	1    2550 2100
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR?
U 1 1 64822CE1
P 2550 3150
F 0 "#PWR?" H 2550 3000 50  0001 C CNN
F 1 "+24V" H 2565 3323 50  0000 C CNN
F 2 "" H 2550 3150 50  0001 C CNN
F 3 "" H 2550 3150 50  0001 C CNN
	1    2550 3150
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR?
U 1 1 64823325
P 2550 4200
F 0 "#PWR?" H 2550 4050 50  0001 C CNN
F 1 "+24V" H 2565 4373 50  0000 C CNN
F 2 "" H 2550 4200 50  0001 C CNN
F 3 "" H 2550 4200 50  0001 C CNN
	1    2550 4200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 1050 2550 1200
Wire Wire Line
	2550 2100 2550 2250
Wire Wire Line
	2550 3150 2550 3300
Wire Wire Line
	2550 4200 2550 4350
$Comp
L Device:R_Pack04_SIP_Split RN?
U 1 1 64845D43
P 3250 1050
F 0 "RN?" H 3338 1096 50  0000 L CNN
F 1 "R_Pack04_SIP_Split" H 3338 1005 50  0000 L CNN
F 2 "Resistor_THT:R_Array_SIP8" V 3170 1050 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 3250 1050 50  0001 C CNN
	1    3250 1050
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN?
U 2 1 648470C1
P 3250 2100
F 0 "RN?" H 3338 2146 50  0000 L CNN
F 1 "R_Pack04_SIP_Split" H 3338 2055 50  0000 L CNN
F 2 "Resistor_THT:R_Array_SIP8" V 3170 2100 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 3250 2100 50  0001 C CNN
	2    3250 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN?
U 3 1 6484828C
P 3250 3150
F 0 "RN?" H 3338 3196 50  0000 L CNN
F 1 "R_Pack04_SIP_Split" H 3338 3105 50  0000 L CNN
F 2 "Resistor_THT:R_Array_SIP8" V 3170 3150 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 3250 3150 50  0001 C CNN
	3    3250 3150
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Pack04_SIP_Split RN?
U 4 1 648498C8
P 3250 4200
F 0 "RN?" H 3338 4246 50  0000 L CNN
F 1 "R_Pack04_SIP_Split" H 3338 4155 50  0000 L CNN
F 2 "Resistor_THT:R_Array_SIP8" V 3170 4200 50  0001 C CNN
F 3 "http://www.vishay.com/docs/31509/csc.pdf" H 3250 4200 50  0001 C CNN
	4    3250 4200
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 6484ABB0
P 3250 900
F 0 "#PWR?" H 3250 750 50  0001 C CNN
F 1 "+5V" H 3265 1073 50  0000 C CNN
F 2 "" H 3250 900 50  0001 C CNN
F 3 "" H 3250 900 50  0001 C CNN
	1    3250 900 
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 6484B2F2
P 3250 1950
F 0 "#PWR?" H 3250 1800 50  0001 C CNN
F 1 "+5V" H 3265 2123 50  0000 C CNN
F 2 "" H 3250 1950 50  0001 C CNN
F 3 "" H 3250 1950 50  0001 C CNN
	1    3250 1950
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 6484C58C
P 3250 3000
F 0 "#PWR?" H 3250 2850 50  0001 C CNN
F 1 "+5V" H 3265 3173 50  0000 C CNN
F 2 "" H 3250 3000 50  0001 C CNN
F 3 "" H 3250 3000 50  0001 C CNN
	1    3250 3000
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 6484CF34
P 3250 4050
F 0 "#PWR?" H 3250 3900 50  0001 C CNN
F 1 "+5V" H 3265 4223 50  0000 C CNN
F 2 "" H 3250 4050 50  0001 C CNN
F 3 "" H 3250 4050 50  0001 C CNN
	1    3250 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 1200 3250 1200
Connection ~ 3250 1200
Wire Wire Line
	3150 2250 3250 2250
Connection ~ 3250 2250
Wire Wire Line
	3150 3300 3250 3300
Connection ~ 3250 3300
Wire Wire Line
	3150 4350 3250 4350
Connection ~ 3250 4350
Wire Notes Line
	24600 3700 26400 3700
Wire Notes Line
	26400 3700 26400 3650
$EndSCHEMATC
