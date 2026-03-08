# ☦ Pravoslavni Kalendar — Ultra Modern Edition

> Moderni pravoslavni kalendar za Windows sa muzičkim plejerom, podsjetnikom i svim srpskim pravoslavnim praznicima

![Preview](https://img.shields.io/badge/Qt-6.6-41CD52?style=flat-square&logo=qt)
![Platform](https://img.shields.io/badge/Windows-10%2F11-0078D6?style=flat-square&logo=windows)
![Language](https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat-square&logo=cplusplus)
![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)

---

## ✨ Funkcionalnosti

| Funkcija | Opis |
|----------|------|
| 📅 **Pravoslavni kalendar** | Svi srpski pravoslavni praznici označeni crvenim slovima |
| ☦ **Pokretni praznici** | Uskrs, Cveti, Spasovdan, Duhovi — automatski računaju se za svaku godinu |
| 🔔 **Podsjetnici** | Dodajte događaje sa tačnim vremenom i podsjetnikom |
| 🎵 **Muzički plejer** | Reprodukujte MP3, FLAC, WAV, OGG i ostale formate |
| 📋 **Plejlista** | Dodajte i upravljajte muzičkom plejlistom |
| 🖥️ **Puni ekran** | Puni ekran i prozorski prikaz (F11) |
| 🌙 **Tamna tema** | Ultra moderni tamni dizajn |
| 💾 **Automatsko čuvanje** | Događaji se čuvaju između pokretanja |

---

## 🚀 Pokretanje (preuzimanje gotovog EXE)

### Opcija 1: GitHub Actions (preporučeno)

1. **Fork** ovaj repozitorijum na vaš GitHub nalog
2. Idite na **Actions** tab u vašem forku
3. Kliknite na **"Build Windows EXE"** workflow
4. Kliknite **"Run workflow"** → **"Run workflow"**
5. Sačekajte ~5-10 minuta da se build završi
6. Preuzmite **`PravoslavniKalendar-Windows-x64.zip`** iz Artifacts sekcije
7. Raspakujte ZIP i pokrenite `PravoslavniKalendar.exe`

> ⚠️ **Nije potrebna instalacija Qt niti bilo čega drugog!** ZIP sadrži sve potrebne DLL fajlove.

### Opcija 2: Kreiranje Release-a

1. U Actions → "Build Windows EXE" → "Run workflow"
2. Uključite opciju **"Create a GitHub Release"**
3. EXE ZIP će biti dostupan i u **Releases** sekciji repozitorijuma

---

## 🔧 Kompajliranje lokalno (opcionalno)

### Zahtjevi
- **Qt 6.6+** (sa QtMultimedia modulom)
- **CMake 3.20+**
- **MSVC 2019/2022** ili **MinGW 13+**

### Koraci (Windows + MSVC)

```powershell
# 1. Instalirajte Qt 6.6 sa https://www.qt.io/download-open-source
#    Uključite: Qt6, QtMultimedia, QtMultimediaWidgets

# 2. Klonirajte repozitorijum
git clone https://github.com/VAS_NALOG/pravoslavni-kalendar.git
cd pravoslavni-kalendar

# 3. Konfigurirajte (zamijenite putanju Qt instalacijom)
cmake -B build -G "NMake Makefiles" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_PREFIX_PATH="C:/Qt/6.6.3/msvc2019_64"

# 4. Kompajlirajte
cmake --build build --config Release

# 5. Deployjte Qt DLL-ove
mkdir deploy
copy build\PravoslavniKalendar.exe deploy\
C:\Qt\6.6.3\msvc2019_64\bin\windeployqt6.exe --release deploy\PravoslavniKalendar.exe

# 6. Pokrenite
deploy\PravoslavniKalendar.exe
```

### Koraci (Windows + MinGW)

```bash
cmake -B build -G "MinGW Makefiles" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_PREFIX_PATH="C:/Qt/6.6.3/mingw_64"
cmake --build build
```

---

## ⌨️ Prečice na tastaturi

| Prečica | Akcija |
|---------|--------|
| `F11` | Prebaci puni ekran / prozor |
| `Ctrl + ←` | Prethodni mjesec |
| `Ctrl + →` | Sljedeći mjesec |
| `Ctrl + Home` | Idi na danas |
| `Escape` | Izađi iz punog ekrana |
| Dvostruki klik na dan | Dodaj novi događaj |

---

## 📅 Pravoslavni praznici

Kalendar koristi **Julijansko računanje** datuma konvertovano u Gregorijanski kalendar (+13 dana za 20. i 21. vijek).

### Veliki praznici (crvena slova ★)
- **7. januar** — Božić (Рођење Господа Исуса Христа)
- **19. januar** — Bogojavljenje (Крштење Господа)
- **27. januar** — Sveti Sava Srpski
- **15. februar** — Sretenje Gospodnje
- **7. april** — Blagovesti
- **Pokretno** — Cveti, Uskrs, Spasovdan, Duhovi
- **28. jun** — Vidovdan
- **29. jun** — Petrovdan
- **2. avgust** — Ilindan
- **19. avgust** — Preobraženje
- **28. avgust** — Velika Gospojina
- **8. novembar** — Aranđelovdan
- **21. novembar** — Vavedenje
- **19. decembar** — Nikolindan

---

## 🎵 Muzički plejer

- Podržani formati: **MP3, FLAC, WAV, OGG, AAC, M4A, WMA, OPUS**
- Kliknite **📂** da dodate muziku u plejlistu
- **⇄** — Slučajni raspored (shuffle)
- **↻** — Ponavljanje (repeat)
- Dvostruki klik na pjesmu u listi — reprodukujte odmah

---

## 📁 Struktura projekta

```
pravoslavni-kalendar/
├── CMakeLists.txt              ← Build konfiguracija
├── src/
│   ├── main.cpp                ← Ulazna tačka
│   ├── mainwindow.h/cpp        ← Glavni prozor
│   ├── calendarwidget.h/cpp    ← Vlastiti kalendarski widget
│   ├── orthodoxholidays.h/cpp  ← Računanje pravoslavnih praznika
│   ├── eventmanager.h/cpp      ← Upravljanje događajima i podsjetnicima
│   ├── musicplayer.h/cpp       ← Muzički plejer
│   └── style.h                 ← Tamna tema (QSS)
├── resources/
│   └── resources.qrc
├── .github/
│   └── workflows/
│       └── build-windows.yml   ← GitHub Actions CI/CD
└── README.md
```

---

## 📜 Licenca

MIT License — slobodan za ličnu i komercijalnu upotrebu.

---

*Neka Gospod blagoslovi ovaj program* ☦
