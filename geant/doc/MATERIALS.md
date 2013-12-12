Materials
=========
List of materials accepted by the `-m` or `--material` option.
The fractions are defined in `SunDetectorConstruction.cc` and the numbers
are taken from [The Sun's Vital Statistics](http://solar-center.stanford.edu/vitalstats.html).

* `SUN`     - the default Sun composition (with only hydrogen and helium)

		 Material:      Sun    density: 160.000 g/cm3   RadL:   4.301 mm
		 Nucl.Int.Length:   2.429 mm   Imean:  21.502 eV   temperature: 15000000.00 K  pressure: 1000000000.00 atm
		   --->  Element: H (H)   Z =  1.0   N =   1.0   A =   1.01 g/mole
				 --->  Isotope:    H1   Z =  1   N =   1   A =   1.01 g/mole   abundance:  99.99 %
				 --->  Isotope:    H2   Z =  1   N =   2   A =   2.01 g/mole   abundance:   0.01 %
				  ElmMassFraction:  74.72 %  ElmAbundance  92.15 % 

		   --->  Element: He (He)   Z =  2.0   N =   4.0   A =   4.00 g/mole
				 --->  Isotope:   He3   Z =  2   N =   3   A =   3.02 g/mole   abundance:   0.00 %
				 --->  Isotope:   He4   Z =  2   N =   4   A =   4.00 g/mole   abundance: 100.00 %
				  ElmMassFraction:  25.28 %  ElmAbundance   7.85 % 

* `SUNFULL` - the composition with a "complete" list of fractions

		 Material:      Sun    density: 160.000 g/cm3   RadL:   4.203 mm
		 Nucl.Int.Length:   2.453 mm   Imean:  21.843 eV   temperature: 15000000.00 K  pressure: 1000000000.00 atm
		   --->  Element: H (H)   Z =  1.0   N =   1.0   A =   1.01 g/mole
				 --->  Isotope:    H1   Z =  1   N =   1   A =   1.01 g/mole   abundance:  99.99 %
				 --->  Isotope:    H2   Z =  1   N =   2   A =   2.01 g/mole   abundance:   0.01 %
				  ElmMassFraction:  73.49 %  ElmAbundance  92.04 % 

		   --->  Element: He (He)   Z =  2.0   N =   4.0   A =   4.00 g/mole
				 --->  Isotope:   He3   Z =  2   N =   3   A =   3.02 g/mole   abundance:   0.00 %
				 --->  Isotope:   He4   Z =  2   N =   4   A =   4.00 g/mole   abundance: 100.00 %
				  ElmMassFraction:  24.86 %  ElmAbundance   7.84 % 

		   --->  Element: O (O)   Z =  8.0   N =  16.0   A =  16.00 g/mole
				 --->  Isotope:   O16   Z =  8   N =  16   A =  15.99 g/mole   abundance:  99.76 %
				 --->  Isotope:   O17   Z =  8   N =  17   A =  17.00 g/mole   abundance:   0.04 %
				 --->  Isotope:   O18   Z =  8   N =  18   A =  18.00 g/mole   abundance:   0.20 %
				  ElmMassFraction:   0.77 %  ElmAbundance   0.06 % 

		   --->  Element: C (C)   Z =  6.0   N =  12.0   A =  12.01 g/mole
				 --->  Isotope:   C12   Z =  6   N =  12   A =  12.00 g/mole   abundance:  98.93 %
				 --->  Isotope:   C13   Z =  6   N =  13   A =  13.00 g/mole   abundance:   1.07 %
				  ElmMassFraction:   0.29 %  ElmAbundance   0.03 % 

		   --->  Element: Fe (Fe)   Z = 26.0   N =  55.9   A =  55.85 g/mole
				 --->  Isotope:  Fe54   Z = 26   N =  54   A =  53.94 g/mole   abundance:   5.84 %
				 --->  Isotope:  Fe56   Z = 26   N =  56   A =  55.93 g/mole   abundance:  91.75 %
				 --->  Isotope:  Fe57   Z = 26   N =  57   A =  56.94 g/mole   abundance:   2.12 %
				 --->  Isotope:  Fe58   Z = 26   N =  58   A =  57.93 g/mole   abundance:   0.28 %
				  ElmMassFraction:   0.16 %  ElmAbundance   0.00 % 

		   --->  Element: Ne (Ne)   Z = 10.0   N =  20.2   A =  20.18 g/mole
				 --->  Isotope:  Ne20   Z = 10   N =  20   A =  19.99 g/mole   abundance:  90.48 %
				 --->  Isotope:  Ne21   Z = 10   N =  21   A =  20.99 g/mole   abundance:   0.27 %
				 --->  Isotope:  Ne22   Z = 10   N =  22   A =  21.99 g/mole   abundance:   9.25 %
				  ElmMassFraction:   0.12 %  ElmAbundance   0.01 % 

		   --->  Element: S (S)   Z = 16.0   N =  32.1   A =  32.07 g/mole
				 --->  Isotope:   S32   Z = 16   N =  32   A =  31.97 g/mole   abundance:  94.93 %
				 --->  Isotope:   S33   Z = 16   N =  33   A =  32.97 g/mole   abundance:   0.76 %
				 --->  Isotope:   S34   Z = 16   N =  34   A =  33.97 g/mole   abundance:   4.29 %
				 --->  Isotope:   S36   Z = 16   N =  36   A =  35.97 g/mole   abundance:   0.02 %
				  ElmMassFraction:   0.10 %  ElmAbundance   0.00 % 

		   --->  Element: N (N)   Z =  7.0   N =  14.0   A =  14.01 g/mole
				 --->  Isotope:   N14   Z =  7   N =  14   A =  14.00 g/mole   abundance:  99.63 %
				 --->  Isotope:   N15   Z =  7   N =  15   A =  15.00 g/mole   abundance:   0.37 %
				  ElmMassFraction:   0.09 %  ElmAbundance   0.01 % 

		   --->  Element: Si (Si)   Z = 14.0   N =  28.1   A =  28.09 g/mole
				 --->  Isotope:  Si28   Z = 14   N =  28   A =  27.98 g/mole   abundance:  92.23 %
				 --->  Isotope:  Si29   Z = 14   N =  29   A =  28.98 g/mole   abundance:   4.68 %
				 --->  Isotope:  Si30   Z = 14   N =  30   A =  29.97 g/mole   abundance:   3.09 %
				  ElmMassFraction:   0.07 %  ElmAbundance   0.00 % 

		   --->  Element: Mg (Mg)   Z = 12.0   N =  24.3   A =  24.31 g/mole
				 --->  Isotope:  Mg24   Z = 12   N =  24   A =  23.98 g/mole   abundance:  78.99 %
				 --->  Isotope:  Mg25   Z = 12   N =  25   A =  24.99 g/mole   abundance:  10.00 %
				 --->  Isotope:  Mg26   Z = 12   N =  26   A =  25.98 g/mole   abundance:  11.01 %
				  ElmMassFraction:   0.05 %  ElmAbundance   0.00 % 

* `VAC`     - the composition of the interstellar void (effectively vacuum)

		 Material:   Vacuum    density:  0.000 kg/m3   RadL: 204727512.315 pc
		 Nucl.Int.Length: 114561548.020 pc   Imean:  19.200 eV   temperature:   0.10 K  pressure:   0.00 atm
		   --->  Element: Vacuum ( )   Z =  1.0   N =   1.0   A =   1.01 g/mole
		      --->  Isotope:     1   Z =  1   N =   1   A =   1.01 g/mole   abundance:  99.99 %
		      --->  Isotope:     2   Z =  1   N =   2   A =   2.01 g/mole   abundance:   0.01 %
		       ElmMassFraction: 100.00 %  ElmAbundance 100.00 % 

* `IRONCORE` - the density, temperature and pressure of the Sun's center
               but consists of only iron atoms (for testing purposes)

		Material: IronCore    density: 160.000 g/cm3   RadL: 864.906 um
		Nucl.Int.Length:   8.346 mm   Imean: 286.000 eV   temperature: 15000000.00 K  pressure: 1000000000.00 atm
		   --->  Element: Fe (Fe)   Z = 26.0   N =  55.9   A =  55.85 g/mole
				 --->  Isotope:  Fe54   Z = 26   N =  54   A =  53.94 g/mole   abundance:   5.84 %
				 --->  Isotope:  Fe56   Z = 26   N =  56   A =  55.93 g/mole   abundance:  91.75 %
				 --->  Isotope:  Fe57   Z = 26   N =  57   A =  56.94 g/mole   abundance:   2.12 %
				 --->  Isotope:  Fe58   Z = 26   N =  58   A =  57.93 g/mole   abundance:   0.28 %
				  ElmMassFraction: 100.00 %  ElmAbundance 100.00 % 

* `HIGHTEMP` - hydrogen and helium composition but with the temperature 1000 times higher

		Material:      Sun    density: 160.000 g/cm3   RadL:   4.301 mm
		Nucl.Int.Length:   2.429 mm   Imean:  21.502 eV   temperature: 15000000000.00 K  pressure: 1000000000.00 atm
		   --->  Element: H (H)   Z =  1.0   N =   1.0   A =   1.01 g/mole
				 --->  Isotope:    H1   Z =  1   N =   1   A =   1.01 g/mole   abundance:  99.99 %
				 --->  Isotope:    H2   Z =  1   N =   2   A =   2.01 g/mole   abundance:   0.01 %
				  ElmMassFraction:  74.72 %  ElmAbundance  92.15 % 

		   --->  Element: He (He)   Z =  2.0   N =   4.0   A =   4.00 g/mole
				 --->  Isotope:   He3   Z =  2   N =   3   A =   3.02 g/mole   abundance:   0.00 %
				 --->  Isotope:   He4   Z =  2   N =   4   A =   4.00 g/mole   abundance: 100.00 %
