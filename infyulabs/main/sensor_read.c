// Well Documented code for better understanding.
//Referance 1. https://embeddedexplorer.com/esp32-gpio-tutorial/ 
//          2. https://my-esp-idf.readthedocs.io/en/latest/api-reference/peripherals/gpio.html
//          3. https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
//     page: 26,32, 63, 

// i'
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5e used abo
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5e for making this code i ha
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5en't add gpio.h header, i'
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5e directly mapped resisters.
#include <stdint.h>
#include <stdio.h>

#define GPIO_INPUT_PIN 25 // Define GPIO pin number

// Register addresses
#define GPIO_ENABLE_REG 0x3FF44020  // GPIO enable register
#define GPIO_OUT_REG    0x3FF44004  // GPIO output register
#define GPIO_IN_REG     0x3FF4403C  // GPIO input register           --- Added for future use
#define GPIO_PIN_MUX_REG 0x3FF49024 // Pin multiplexing register    --- Added for future use

// Function to set a GPIO pin as input

* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5oid gpio_set_direction(uint8_t gpio_num, uint8_t mode) {
    if (mode) { // If mode is 1, set as output
        *(
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5olatile uint32_t *)(GPIO_ENABLE_REG) |= (1 << gpio_num);
    } else { // Set as input if 0 is placed
        *(
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5olatile uint32_t *)(GPIO_ENABLE_REG) &= ~(1 << gpio_num);
    }
}

// Function to read the le
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5el of a GPIO pin
uint8_t gpio_get_le
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5el(uint8_t gpio_num) {
    return (*(
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5olatile uint32_t *)(GPIO_IN_REG) >> gpio_num) & 0x01; // & 0x01 is used to isolate the least significant bit (LSB)
}                                                                    // of the 
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5alue read from the GPIO input register, mask all, (to extract a single bit from a multi-bit 
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5alue)


* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5oid main() {
    // Set GPIO_INPUT_PIN as input
    gpio_set_direction(GPIO_INPUT_PIN, 0);

    while (1) {
        // Read the le
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5el of the input pin
        uint8_t le
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5el = gpio_get_le
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5el(GPIO_INPUT_PIN);
        
        // Print the le
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5el to Terminal or serial monitor. same time connect FFT Analyzer 
        printf("GPIO %d Le
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5el: %d\n", GPIO_INPUT_PIN, le
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5el);
        
        // Add a delay if necessary (e.g., using 
* Parameters directly from Table 1 (Toward Simplified... paper)
.PARAM a0=4.8195e-5
.PARAM a1=2.4006e-6
.PARAM a2=1.03685
.PARAM a3=0.34567
.PARAM b0=7.0526e-4
.PARAM b1=4.2383e-5
.PARAM c0=4.0041
.PARAM c1=2.8646
.PARAM c2=4.2125
.PARAM c3=1.4134
.PARAM d0=6.6103
.PARAM d1=1.4524
.PARAM d2=7.4235
.PARAM d3=4.0585
.PARAM f0=6.326e-4
.PARAM f1=1.4711
.PARAM f2=0.5199
.PARAM f3=1.561
.PARAM g0=4.84e-3
.PARAM g1=0.1353
.PARAM h0=5.548
.PARAM h1=6.8648
.PARAM h2=51.586
.PARAM h3=0.36
.PARAM j0=1.054
.PARAM k0=1.0526

* Oxygen vacancy concentration parameters
.PARAM Nd_min = 0.008         ; Minimum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Nd_max = 20            ; Maximum oxygen vacancy concentration in the disc [10^26/m^3]
.PARAM Npp = 20               ; Oxygen vacancy concentration in the plug [10^26/m^3]
.PARAM alpha = 0.25e-9       ; Ion hopping distance (m)
.PARAM v0 = 2e13             ; Attempt frequency (Hz)
.PARAM WA = 1.2              ; Nominal activation barrier (eV)
.PARAM kB = 1.3807e-23       ; Boltzmann constant (J/K)
.PARAM T0 = 293              ; Ambient temperature (K)
.PARAM qe = 1.602e-19        ; Elementary charge (C)
.PARAM zVO = 2               ; Oxygen vacancy charge number
.PARAM ld = 0.4e-9           ; Length of disc (m)
.PARAM lc = 3e-9             ; Length of cell switching layer (m)
.param Nd_initial = 0.100     ; Initial oxygen vacancy concentration in the disc

* Additional thermal and resistance parameters
.PARAM RTiOx = 650           ; Series resistance of the TiOx layer [Ohm]
.PARAM R0 = 719.2437         ; Line resistance for a current of 0 A [Ohm]
.PARAM aline = 3.92e-3       ; Temperature coefficient of the lines [1/K]
.PARAM Rthl = 90471.47       ; Thermal resistance of the lines [W/K]
.PARAM Rth0 = 15.72e6        ; Thermal resistance of Hafnium Oxide (K/W)
.PARAM Rtheff_scaling = 0.27 ; Scaling factor for gradual RESET

* New parameters for mobility and filament radius
.PARAM mun = 4e-6           ; Electron mobility (m^2/Vs)
.PARAM radi = 45e-9         ; Radius of the filament area (m)

* Input voltage across memristor terminals
Vm POS NEG VALUE={V(POS,NEG)}

* Define series resistance Rs based on Equation (1)
.func Rs()={RTiOx + R0 * (1 + aline * R0 * I(Vmeas)^2 * Rthl)}

* Define cross-sectional area Ar
.PARAM Ar = {3.14159 * radi^2} ; Cross-sectional area of the filament (m^2)

* Disc Resistance (Rd) as a behavioral resistor(equation 2)
.func Rd(V(Nd))={ld / (zVO * qe * Ar * V(Nd) * mun)}

* Define Voltage Across Disc (Vd)
.func Vd(V(Nd))={I(Vmeas) * Rd(V(Nd))}

* Define Vc 
.func Vc(Vm)={Vm - I(Vmeas) * Rs()}

* Calculate electric field (SET and RESET transitions)
.func Es(Vm, V(Nd))={IF(Vm < 0, Vd(V(Nd)) / ld, 0)}
.func Er(Vm, V(Nd))={IF(Vm > 0, Vc(Vm) / lc, 0)}

* Define gamma 
.func gamma(Vm, V(Nd))={qe * zVO * alpha * IF(Vm < 0, Es(Vm, V(Nd)), Er(Vm, V(Nd))) / (WA * pi)}

* Forward and reverse barriers 
.func Wf(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) - gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}
.func Wr(Vm, V(Nd))={WA * (sqrt(1 - gamma(Vm, V(Nd))^2) + gamma(Vm, V(Nd)) * pi / 2 + gamma(Vm, V(Nd)) * asin(gamma(Vm,V(Nd))))}

* Average ion concentration in the HfO2 layer 
.func cVO(V(Nd))={(Npp + V(Nd)) / 2}

* Ion hopping current 
.func Iion(V(Nd), Vm)={(zVO * qe * Ar * cVO(V(Nd)) * alpha * v0 * Fl(V(Nd), Vm)) * 
                      (exp(-Wf(Vm, V(Nd)) / (kB * T0)) - exp(-Wr(Vm, V(Nd)) / (kB * T0)))}

* Define window function Fl 
.func Fl(V(Nd), Vm)={IF(Vm > 0, 1 - (Nd_min / V(Nd))^10, 1 - (V(Nd) / Nd_max)^10)}

* State equation 
Bstate GND Nd_update VALUE={-Iion / (zVO * qe * Ar * ld)}

* Voltage-controlled current source to solve state equation 
Gx 0 Nd VALUE={-Iion(V(Nd), Vm) / (zVO * qe * Ar * ld)}
Cx Nd GND {1p}                ; Small capacitance for state variable integration 
.ic V(Nd) = Nd_initial        ; Initial condition for Nd

* Memristor Current Source Implementation 

* Define intermediate variables as functions 
.func A(Vm)={((a0 * exp(-a2 / a3) + a0) / (1 + exp(-(V(POS,NEG) + a2) / a3))) - a0} 
.func Q(Vm)={d2 * exp(-Vm / d3) + d1 * Vm - d0} 
.func F(Vm)={f0 + ((f1 - f0) / (1 + (-Vm / f2)^f3))}
.func B(Vm, V(Nd))={(1 + ((c2 * exp(-Vm / c3) + c1 * Vm - c0) / V(Nd))^Q(Vm)) )^ F(Vm)}
.func D(Vm, V(Nd))={1 + (h0 + h1 * Vm + h2 * exp(-h3 * Vm))*(V(Nd)/Nd_min)^(-j0)}

* Define the behavioral current source 
Vmeas POS MeasNode GND          ; Insert zero-volt voltage source in series with Gm 
Gm MeasNode NEG VALUE={IF(v(POS,NEG)< 0, (-A(v(POS,NEG)-b1*(1-exp(-v(POS,NEG)))+b0*v(POS,NEG))/B(v(POS,NEG),V(nD)), (-g0*(exp(-g1*(v(POS,NEG))) - 1)/D(v(POS,NEG),V(nD)))^(k(01/k)))}

.ENDS vcm_check5TaskDelay)
    }
}
