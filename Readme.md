
# 🐻‍❄️ STDIO - Abschlussprojekt

Das Projekt wurde von Erik Ensel, Nico Nöll und Edwin Rheindt bearbeitet.


## Aufgabenverteilung

#### @Erik Ensel
- Aufbau Grundstruktur des Projekts

#### @Nico Nöll
- 

#### @Edwin Rheindt
- Aufbau Grundstruktur des Projekts
- Implementierung der Help-Message für die CLI
- Implementierung der print Funktion
- Implementierung des Terminal-Moduls
- Implementierung der Tracefiles
- Hinzufügen der Result-Statistiken nach run_simulation





## Implementierung des TERMINAL-Moduls

Das TERMINAL-Modul fungiert als Memory-Mapped Peripheriegerät und verwendet die Adresse `0xFFFFFFFF` für den Terminal-Output. Schreibt man auf diese Adresse, so wird das niederwertigste Byte der Daten (`wdata`) als `char` gedeutet. Je nach CLI-Flag werden die Daten direkt ausgegeben oder in eine Datei geschrieben. Das Modul unterstützt eine Latenz von `--latency` Clock-Zyklen und zeigt über das `ready`-Flag, wenn es bereit für den nächsten Zugriff ist. Das `end`-Flag wird gesetzt, wenn der Wert `0` geschrieben wird, um das Ende der Ausgabe zu markieren.

## Erweiterung des TinyAssemblers
Um der nachfolgenden Unterstützung von Libraries gerecht zu werden, bedarf es an Labels und einem `jal` Befehl. Labels gestatten die Verwendung von symbolischen Adressen im Code, die der Assembler automatisch in Offsets umrechnet. Zusätzlich wurde ein vereinfachter Linker implementiert, der externe Libraries wie `stdio.S` einbindet. Der Assembler fügt den Code der Libraries an den Hauptcode an und passt die Adressen entsprechend an.

## stdio.S Library
In der `stdio.S`-Bibliothek ist die Funktion `print` enthalten, mit deren Hilfe ein Null-terminierter String aus dem Speicher im Terminal ausgegeben wird. Die verwendete Calling Convention findet sich weiter unten.
## TODO neue Funktion in der Library

## Rahmenprogramm und Simulation
Das Rahmenprogramm verarbeitet CLI-Optionen wie `--cycles`, `--tracefile` und `--terminal-file` und konfiguriert die Simulation entsprechend. Die dafür hinzugefügte Funktion `run_simulation` initialisiert die CPU, lädt das Programm in den Speicher und startet die Simulation. Die Resultate werden in einem `Result`-Struct übermittelt. Das Resultat umfasst unter anderem die erforderlichen Zyklen für das Ausgeben sowie die Anzahl der Ausgaben.

## Literaturrecherche
### Fachbegriffe
- **Memory Mapping**: Eine Technik, bei der Peripheriegeräte über spezielle Speicheradressen angesprochen werden. Dies ermöglicht eine einheitliche Adressierung von Speicher und Geräten.
- **Calling Convention**: Eine Vereinbarung, wie Funktionen aufgerufen werden, einschließlich Parameterübergabe und Registerverwendung. In diesem Projekt wird eine minimale Convention verwendet:
    | Register | ABI Name     | Beschreibung                | Saver | 
    | :-------- | :------- | :------------------------- | :------- | 
    | `x0` | `zero` | Fixiert auf Zahl 0 | - |
    | `x1 - x10` | `a0 - a9` | Funktionsargumente und Return values | Caller |
    | `x11 - x20` | `t0 - t9` | Temporaries | Caller |
    | `x21 - x29` | `s0 - s8` | Saved-Register | Callee |
     `x30` | `sp` | Stack-Pointer | Callee |
    | `x31` | `ra` | Return-Adresse | Caller |


- **Linking**: Der Prozess des Zusammenfügens von mehreren Objektdateien zu einem ausführbaren Programm. In diesem Projekt wurde ein vereinfachter Linker implementiert, der Libraries direkt während des Assemblierens einbindet und am Ende des assemblierten Codes anfügt.

## Beantwortung der Fragen

**_Die hier verwendete Calling Convention ist sehr minimalistisch. Was könnte beispielsweise schiefgehen, wenn verschachtelte Funktionen aufgerufen werden?_**

> Wenn eine der verschachtelten Funktionen eine andere Funktion aufruft, muss die Rücksprungadresse überschrieben werden.   
>
> Geschieht dies, ist die Rücksprungadresse nicht mehr zwangsläufig korrekt und es kommt zu einem ungewollten Verhalten des Programms.

**_Wie wird das vorherige Problem häufig gelöst?_**

> Um das Problem zu lösen wird häufig ein Stack verwendet. Auf diesem werden Register und die Rücksprungadresse gespeichert, um auch bei verschachtelten Funktionsaufrufen "Consistency" bewahren zu können.
>
> Somit kann die Rücksprungadresse immer wiederhergestellt werden.

**_Wie unterscheidet sich der vereinfachte Linker in diesem Projekt von herkömmlichen Linkern?_**

> Der vereinfachte Linker in diesem Projekt bindet Libraries wie `stdio.S` direkt während dem Assemblierens ein und fügt den Code am Ende des Hauptprogrammes an. Somit sind keine Abhängigkeiten zwischen Libraries selbst möglich.
>
> Im Vergleich dazu verarbeitet ein herkömmliche Linker oft bereits vorkompilierte Objektdateien. Dabei werden symbolische Referenzen (Labels) aufgelöst.
Im Gegensatz zu unserem Linker, kann ein herkömmlicher Linker auch Adressen anpassen und ermöglicht somit dynamisches Linken (also dem Laden von Bibliotheken zur Laufzeit). Damit sind auch Abhängigkeiten möglich.


## Quellen

**Memory Mapping:**     
[IBM - Understanding memory mapping](https://www.ibm.com/docs/sl/aix/7.2.0?topic=memory-understanding-mapping)  
[GeeksForGeeks - Memory Mapped I/O and Isolated I/O](https://www.geeksforgeeks.org/computer-organization-architecture/memory-mapped-i-o-and-isolated-i-o/)

**Calling Convention:**     
[RISC-V - Calling Convention](https://riscv.org/wp-content/uploads/2024/12/riscv-calling.pdf)  
Alle anderen Infos wurden der Vorlesung "Einführung in die Rechnerarchitektur" an der Technischen Universität München entnommen.

**Linking:**     
[Brainkart - Assembly and Linking](https://www.brainkart.com/article/Assembly-and-Linking_11818/)  
[Wikipedia - Linker](https://en.wikipedia.org/wiki/Linker_(computing))  
Weitere Infos konnten ebenfalls direkt der Projektbeschreibung entnommen werden.
