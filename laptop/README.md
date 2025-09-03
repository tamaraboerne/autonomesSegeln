# autosail-2025

## Getting started

Klonen des Repos und seiner Submodule

```bash
git clone git@git.mylab.th-luebeck.de:autosail/autosail-2025.git
cd autosail-2025
git switch pixhawk-firmware
git submodule update --init
cd px4-autopilot
git switch autosail-dev
git remote add upstream https://github.com/PX4/PX4-Autopilot.git
git fetch upstream
git submodule update --init --recursive
```

Damit die Commits auch richtig zugeordnet werden können, sollte zudem die Git-Config angepasst werden:
```bash
git config --global user.name "<Name>"
git config --global user.email <E-Mail>
```

## Starten des Java-Backends

Das Backend wird als ausführbare JAR-Datei gestartet. Als erster Parameter kann der serielle Port angegeben werden, an dem das XBee-Modul angeschlossen ist (Standard: ttyUSB0).

### Beispiel (Windows, PowerShell):


1. JAR-Datei erstellen:

```powershell
mvn clean package
```

Die ausführbare JAR liegt danach unter:

    target/autosail-1.0-SNAPSHOT.jar

2. Backend starten (ersetze `ttyUSB0` ggf. durch korrekten Port):

```powershell
java -jar target/autosail-1.0-SNAPSHOT.jar ttyUSB0
```

- **ttyUSB0**: (optional) Name des seriellen Ports, an dem das XBee angeschlossen ist, Standard ist ttyUSB0

