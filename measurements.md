# Scheda geometrica della simulazione MedLinac


## Parametri globali

| Volume | Solido e dimensioni | Materiale | Posizione e rotazione | Nota |
|---|---|---|---|---|
| `world` (`experimentalHall`) | `G4Box`; semi-dimensioni `[3, 3, 3] m`; dimensioni complete `[6, 6, 6] m` | Aria | `[0, 0, 0]`; nessuna rotazione | Volume di riferimento globale. |
| `vacuumBlock` | `G4Box`; semi-dimensioni `[9, 9, 8.755] cm`; dimensioni complete `[18, 18, 17.510] cm` | Vuoto (`Vacuum`/`interGalactic`) | Nel `world`: `[0, 0, 114.755 cm]` | Riferimento delle coordinate locali della testa. |

## Testa: finestre e collimatori

| Componente | Solido e dimensioni | Materiale | Posizione e rotazione | Nota / confronto |
|---|---|---|---|---|
| `windowUp` | `G4Box`; semi-dimensioni `[4, 4, 0.0125] cm`; dimensioni complete `[8, 8, 0.025] cm` | Berillio | `world [0, 0, 123.5225 cm]`; rotazione 0 | Nessuna osservazione specifica nella scheda. |
| `UpperCollimator` | `G4Tubs`; `rmin=1 cm`, `rmax=8 cm`, semi-altezza `4 cm`; angoli `[0, 360] deg` | Tungsteno | Locale `[0, 0, 2.645 cm]`; `world z=117.400 cm`; rotazione 0 | Materiale indicato nella documentazione, p. 14/23 (riferimento 30). I raggi non sono specificati nel disegno: sono stimati come coerenti. L'altezza totale di `8 cm` è solo parzialmente confrontabile con il valore `7.6 cm` riportato nel disegno. La quota è annotata come `starting point + 0.4 cm` rispetto al target. |
| `collim_cone` (apertura interna) | `G4Cons`; `rmin1=0 cm`, `rmax1=0.39 cm`; `rmin2=0 cm`, `rmax2=1.89 cm`; semi-altezza `3.1 cm`; angoli `[0, 360] deg` | Vuoto | Non piazzato autonomamente; usato per la sottrazione da `tracker_tube` | Il vuoto è coerente con la camera interna. Altezza totale `6.2 cm`: nella documentazione sono annotati sia `6 cm` (p. 13/23) sia `6.2 cm` (p. 14/23). I raggi non sono quotati e sono stati ricavati assumendo un'apertura di `14 deg`. Verificare se la descrizione documentale del cono corrisponde al solido usato come sottrattore. |
| `tracker_tube` / `CylMinusCone` | Solido iniziale `G4Tubs`; `rmin=0 cm`, `rmax=8 cm`, semi-altezza `3.1 cm`; angoli `[0, 360] deg`. Solido finale: `tracker_tube - collim_cone` | Tungsteno (solido finale) | Locale `[0, 0, -4.455 cm]`; `world z=110.300 cm`; rotazione `X=180 deg` | Raggio esterno e semi-altezza sono annotati come coerenti rispettivamente con `UpperCollimator` e cono. La faccia superiore risulta a `113.4 cm`, a contatto con la faccia inferiore di `UpperCollimator` (`113.4 cm`). Materiale indicato a p. 14/23 (riferimento 30). |
| `windowLow` | `G4Box`; semi-dimensioni `[4, 4, 0.0127] cm`; dimensioni complete `[8, 8, 0.0254] cm` | Berillio | `world [0, 0, 105.9873 cm]`; rotazione 0 | Spessore indicato a p. 14/23 (riferimento 30); le altre dimensioni non sono quotate e sono considerate sufficienti a coprire l'apertura. **Verificare la quota:** l'espressione annotata `114.755 + 2.645 - 10.2` restituisce `107.2 cm`, non `105.9873 cm`. |

## Camera a ionizzazione e specchio

| Componente | Solido e dimensioni | Materiale | Posizione e rotazione | Nota / confronto |
|---|---|---|---|---|
| Finestre della camera (`Window`, 3) | `G4Tubs`; `rmin=0 cm`, `rmax=4.7625 cm`, semi-altezza `0.0127 cm`; angoli `[0, 360] deg` | Kapton | `world z=100.165`, `99.450`, `98.735 cm`; `x=y=0`; rotazione 0 | Materiale indicato a p. 14/23 (riferimento 30). Distanza tra la prima e l'ultima quota: `1.430 cm`. La stima annotata prevede un'estensione approssimativa da `100.7` a `98 cm`; la documentazione disponibile è generica, quindi la corrispondenza resta non verificata. |
| Piastre di segnale (`SignalPlate`, 4) | `G4Tubs`; `rmin=0 cm`, `rmax=4.7625 cm`, semi-altezza `0.00254 cm`; angoli `[0, 360] deg` | Kapton | `world z=99.927`, `99.688`, `99.212`, `98.973 cm`; `x=y=0`; rotazione 0 | Materiale indicato a p. 14/23 (riferimento 30). |
| `Mirror` | `G4Tubs`; `rmin=0 cm`, `rmax=6 cm`, semi-altezza `0.00254 cm`; angoli `[0, 360] deg` | Mylar | `world [0, 0, 93.0 cm]`; rotazione `Y=35 deg` | Materiale e angolo indicati a p. 14/23 (riferimento 30); i raggi non sono quotati. Rispetto alla piastra più vicina (`z=98.973 cm`), la distanza tra i centri è `5.973 cm`, contro la stima annotata di `5.1 cm`. La quota assoluta dello specchio è considerata corretta; potrebbe mancare un elemento o una quota nella descrizione della camera. |
| `reticle` (light field reticle) | `G4Box`; semi-dimensioni `[15, 15, 0.00508] cm`; dimensioni complete `[30, 30, 0.01016] cm` | Mylar | `world [0, 0, 65.5 cm]`; rotazione 0 | Materiale e posizione indicati a p. 14/23 (riferimento 30); dimensioni non quotate. La distanza dal target è annotata come `49.5 cm`, coerente approssimativamente con la distanza dalle quote dei target (`49.33–49.46 cm`). |

## Foil elettronici

Specifiche annotate come tratte da *Monte Carlo High Energy*, p. 53. Il riferimento `startingDistance=102.4375 cm` è invece ripreso dalla modalità fotoni, assumendo che i foil occupino la stessa posizione.

| Componente | Solido e dimensioni (`rmin`, `rmax`, semi-altezza) | Materiale | `world z` | Nota |
|---|---|---|---:|---|
| `sidelower` | `G4Tubs`; `2.575`, `6.625`, `0.054 cm` | Alluminio | `102.4375 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `side` | `G4Tubs`; `2.575`, `3.375`, `1.02 cm` | Alluminio | `103.5115 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `alDiskHolder` | `G4Tubs`; `0.4`, `3.475`, `0.004 cm` | Alluminio | `102.3795 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `alDiskHolder2` | `G4Tubs`; `0`, `2.225`, `0.004 cm` | Alluminio | `102.2855 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `alDiskHolder3` | `G4Tubs`; `2.225`, `3.475`, `0.08 cm` | Alluminio | `102.2015 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `alDisk` | `G4Tubs`; `0`, `0.4`, `0.011 cm` | Alluminio | `102.3945 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `alDisk2` | `G4Tubs`; `0`, `0.499`, `0.043 cm` | Alluminio | `102.3325 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `taDiskHolder` | `G4Tubs`; `2.2`, `3.2`, `0.075 cm` | Alluminio | `104.6065 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `taDiskHolder3` | `G4Cons`; `rmin1=1.425`, `rmax1=1.575 cm`; `rmin2=2.2`, `rmax2=2.35 cm`; semi-altezza `0.52 cm` | Alluminio | `105.2015 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `taDiskHolder2` | `G4Tubs`; `0.635`, `1.575`, `0.075 cm` | Alluminio | `105.7965 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |
| `taDisk` | `G4Tubs`; `0`, `0.635`, `0.00255 cm` | Tantalio | `105.87405 cm` | Rotazione comune `X=180 deg`; `x=y=0`. |

## Target e filtro appiattitore fotonico

| Componente | Solido e dimensioni | Materiale | Posizione e rotazione | Nota |
|---|---|---|---|---|
| `targetA` | `G4Box`; semi-dimensioni `[0.6, 0.6, 0.04445] cm`; dimensioni complete `[1.2, 1.2, 0.0889] cm` | Tungsteno | Locale `[0, 0, 0.20055 cm]`; `world z=114.95555 cm`; rotazione 0 |  |
| `targetB` | `G4Box`; semi-dimensioni `[0.6, 0.6, 0.07874] cm`; dimensioni complete `[1.2, 1.2, 0.15748] cm` | Rame; densità `8.960 g/cm3`, massa atomica `63.55 g/mole` | Locale `[0, 0, 0.07736 cm]`; `world z=114.83236 cm`; rotazione 0 |  |

Il filtro è composto da 18 strati conici, più `layer19`, `layer20` composito e `layer21`. Tutti sono centrati su `x=y=0`. Gli strati 1–18 hanno rotazione `X=180 deg`; gli strati 19–21 hanno rotazione 0. Il materiale pieno è rame, fatta eccezione per il volume d'aria sottratto in `layer20`.

| Strato | Solido e dimensioni | Semi-altezza | `world z` | Materiale / rotazione |
|---|---|---:|---:|---|
| `layer1` | `G4Cons`; `(rmin1,rmax1)=(0, 0.00000001) cm` → `(rmin2,rmax2)=(0, 0.025) cm` | `0.006 cm` | `103.301 cm` | Rame; `X=180 deg` |
| `layer2` | `G4Cons`; `(0, 0.025)` → `(0, 0.050) cm` | `0.0065 cm` | `103.2885 cm` | Rame; `X=180 deg` |
| `layer3` | `G4Cons`; `(0, 0.050)` → `(0, 0.075) cm` | `0.0075 cm` | `103.2745 cm` | Rame; `X=180 deg` |
| `layer4` | `G4Cons`; `(0, 0.075)` → `(0, 0.100) cm` | `0.009 cm` | `103.258 cm` | Rame; `X=180 deg` |
| `layer5` | `G4Cons`; `(0, 0.100)` → `(0, 0.150) cm` | `0.022 cm` | `103.227 cm` | Rame; `X=180 deg` |
| `layer6` | `G4Cons`; `(0, 0.150)` → `(0, 0.200) cm` | `0.024 cm` | `103.181 cm` | Rame; `X=180 deg` |
| `layer7` | `G4Cons`; `(0, 0.200)` → `(0, 0.250) cm` | `0.023 cm` | `103.134 cm` | Rame; `X=180 deg` |
| `layer8` | `G4Cons`; `(0, 0.250)` → `(0, 0.300) cm` | `0.023 cm` | `103.088 cm` | Rame; `X=180 deg` |
| `layer9` | `G4Cons`; `(0, 0.300)` → `(0, 0.350) cm` | `0.0225 cm` | `103.0425 cm` | Rame; `X=180 deg` |
| `layer10` | `G4Cons`; `(0, 0.350)` → `(0, 0.400) cm` | `0.022 cm` | `102.998 cm` | Rame; `X=180 deg` |
| `layer11` | `G4Cons`; `(0, 0.400)` → `(0, 0.500) cm` | `0.0425 cm` | `102.9335 cm` | Rame; `X=180 deg` |
| `layer12` | `G4Cons`; `(0, 0.500)` → `(0, 0.600) cm` | `0.0395 cm` | `102.8515 cm` | Rame; `X=180 deg` |
| `layer13` | `G4Cons`; `(0, 0.600)` → `(0, 0.700) cm` | `0.038 cm` | `102.774 cm` | Rame; `X=180 deg` |
| `layer14` | `G4Cons`; `(0, 0.700)` → `(0, 0.800) cm` | `0.0335 cm` | `102.7025 cm` | Rame; `X=180 deg` |
| `layer15` | `G4Cons`; `(0, 0.800)` → `(0, 0.900) cm` | `0.0325 cm` | `102.6365 cm` | Rame; `X=180 deg` |
| `layer16` | `G4Cons`; `(0, 0.900)` → `(0, 1.000) cm` | `0.028 cm` | `102.576 cm` | Rame; `X=180 deg` |
| `layer17` | `G4Cons`; `(0, 1.000)` → `(0, 1.100) cm` | `0.0275 cm` | `102.5205 cm` | Rame; `X=180 deg` |
| `layer18` | `G4Cons`; `(0, 1.100)` → `(0, 1.205) cm` | `0.019 cm` | `102.474 cm` | Rame; `X=180 deg` |
| `layer19` | `G4Tubs`; `rmin=0`, `rmax=1.325 cm` | `0.040 cm` | `101.884 cm` (`z18 - 0.59 cm`) | Rame; rotazione 0 |
| `layer20A` / `cone20` | `layer20A`: `G4Tubs`, `rmin=1.300`, `rmax=1.325 cm`. Sottrattore `cone20`: `G4Cons`, `rmin1=rmin2=0`, `rmax1=1.326`, `rmax2=1.301 cm` | `0.0225 cm` per entrambi | `102.499 cm` (`z19 + 0.615 cm`) | `layer20 = layer20A - cone20`; rame meno aria; rotazione 0 |
| `layer21` | `G4Tubs`; `rmin=1.325`, `rmax=1.500 cm` | `0.0625 cm` | `102.099 cm` (`z20 - 0.4 cm`) | Rame; rotazione 0 |

## Jaws

Campo predefinito: `X/Y = +/-2.5 cm`. Materiale: tungsteno. Le posizioni sono calcolate per puntare al piano a `100 cm`. Le sole quote attribuite alla documentazione riguardano `z`; le altre dimensioni sono riprese dal documento Pandora, *Low Energy v2*, p. 12.

| Jaw | Solido e dimensioni | Angolo e rotazione | Posizione calcolata | Nota |
|---|---|---|---|---|
| `JawY1` | `G4Box`; semi-dimensioni `[9.40, 5.97, 3.88] cm`; dimensioni complete `[18.80, 11.94, 7.76] cm`; `x=0` | `theta=atan(-fieldY1/(100 cm))`; rotazione `X=theta` | `y=-((3.9+28)sin(theta)+5.97cos(theta)) cm`; `z=115 cm-(3.9+28)cos(theta) cm+5.97sin(theta) cm` | Posizione dipendente dall'apertura del campo. La formula è ritenuta plausibile, ma non è stata rintracciata nella documentazione. |
| `JawY2` | Come `JawY1`; `x=0` | `theta=atan(fieldY2/(100 cm))`; rotazione `X=-theta` | `y=((3.9+28)sin(theta)+5.97cos(theta)) cm`; `z=115 cm-(3.9+28)cos(theta) cm+5.97sin(theta) cm` | Come `JawY1`. |
| `JawX1` | `G4Box`; semi-dimensioni `[6.73, 10.92, 3.88] cm`; dimensioni complete `[13.46, 21.84, 7.76] cm`; `y=0` | `theta=atan(-fieldX1/(100 cm))`; rotazione `Y=-theta` | `x=-((36.7+3.9)sin(theta)+6.73cos(theta)) cm`; `z=115 cm-(36.7+3.9)cos(theta) cm+6.73sin(theta) cm` | Posizione dipendente dall'apertura del campo. La formula è ritenuta plausibile, ma non è stata rintracciata nella documentazione. |
| `JawX2` | Come `JawX1`; `y=0` | `theta=atan(fieldX2/(100 cm))`; rotazione `Y=theta` | `x=((36.7+3.9)sin(theta)+6.73cos(theta)) cm`; `z=115 cm-(36.7+3.9)cos(theta) cm+6.73sin(theta) cm` | Come `JawX1`. |

## MLC

Configurazione a 80 foglie: `A1–A40` e `B1–B40`. Geometria ricavata da un documento MLC e non verificata; la scheda indica che l'MLC non viene usato nella modalità elettroni.

| Elemento | Solido e dimensioni | Trasformazione / posizione | Nota |
|---|---|---|---|
| `preLeaf_box` | `G4Box`; semi-dimensioni `[2.715 mm, 64.72066 mm, 29.3 mm]` | Displacement `[0, 0, -50 cm]` | Parte del solido base della foglia. |
| `leaf end` | `G4Tubs`; `rmin=0.01 mm`, `rmax=80 mm`, semi-altezza `2.715 mm`; angoli `[0, 180] deg` | Displacement `[0, -9.4413249 mm, -50 cm]`; rotazione `Y=90 deg` | Aggiunto a `preLeaf_box`. |
| `gap` sottratto | `G4Box`; semi-dimensioni `[90.00 mm, 74.18 mm, 3.0 mm]` | Non specificata separatamente nella scheda | Sottratto dalla geometria. |
| `cutA` sottratto | `G4Box`; semi-dimensioni `[0.20 mm, 68.0 mm, 15.925 mm]` | `[-2.515 mm, 3.0 mm, -48.6625 cm]` | Sottratto dalla geometria. |
| `cutB` sottratto | `G4Box`; semi-dimensioni `[0.1625 mm, 68.0 mm, 14.415 mm]` | `[2.5525 mm, 3.0 mm, -51.4885 cm]` | Sottratto dalla geometria. |
| Solido foglia | `((preLeaf_box + leaf end) - cutA) - cutB` | — | Composizione riportata nella scheda. |
| Foglie A (`A1–A40`) | 40 foglie; passo `x=5.431 mm` | `x` iniziale `-111.335 mm`; `z=97.25 cm`; `x(i)=x(i-1)+5.431 mm`; `y(i)=-((yA(i)*(100-47.25))/100+70.5587 mm)`; rotazione 0 | Default `yA1…yA40=40 cm`. Posizioni `yA` modificabili via macro MLC. |
| Foglie B (`B1–B40`) | 40 foglie; passo `x=5.431 mm` | `x` iniziale `-111.335 mm`; `z=-2.75 cm`; `x(i)=x(i-1)+5.431 mm`; `y(i)=((yB(i)*(100-47.25))/100+70.5587 mm)`; rotazione `X=180 deg` | Default `yB1…yB40=40 cm`. Posizioni `yB` modificabili via macro MLC. |

## Punti da verificare

| Priorità | Punto | Verifica suggerita |
|---|---|---|
| Alta | Quota di `windowLow` | Confrontare la posizione dichiarata (`105.9873 cm`) con la formula annotata (`114.755 + 2.645 - 10.2 = 107.2 cm`) e con il posizionamento effettivo nel codice. |
| Media | `collim_cone` | Chiarire nella documentazione se il cono è un volume autonomo o la cavità ottenuta sottraendolo dal tungsteno; confrontare inoltre le altezze alternative di `6` e `6.2 cm`. |
| Media | Camera a ionizzazione e `Mirror` | Verificare se la descrizione generica della camera omette componenti o quote che spiegano la distanza stimata di `5.1 cm` rispetto ai `5.973 cm` tra i centri riportati. |
| Media | Jaws | Reperire la fonte delle formule di posizionamento e confermare le dimensioni non documentate nelle fonti citate. |
| Bassa | Foil elettronici | Confermare che il riferimento di posizione mutuato dalla modalità fotoni sia valido anche per la configurazione elettronica. |
| Bassa | MLC | Confrontare la geometria e le quote con la versione esatta del documento MLC; la scheda dichiara che non è stata verificata. |
