# 🎉 GraphTest v1.0 — "LARA's BIRTHDAY"

**Autore:** Alessandro Favretto  
**Data:** 31/10/2025  
**Linguaggio:** C++ (Win32 API)  
**Versione:** 1.0 (x64)  
**Licenza:** Freeware / Educational use  

---

## 🧭 Descrizione

**GraphTest** è una piccola applicazione **standalone Win32**, sviluppata in **VC++** senza dipendenze esterne (no MFC, no .NET).  
Mostra una **grafica animata** gestita tramite **GDI**, con un cerchio che rimbalza all’interno della finestra su uno sfondo personalizzabile (immagine BMP).  

Il progetto è stato realizzato come **test grafico e tecnico**, ma anche come piccolo omaggio personale a mia figlia:  
> *"LARA’s BIRTHDAY — una demo per sperimentare le WinAPI e la grafica GDI, dedicata a un giorno speciale."*

---

## ⚙️ Funzionalità principali

- ✅ **Grafica animata fluida** con gestione del ridisegno tramite `WM_PAINT`  
- ✅ **eliminazione del flickering Senza Double buffering**  
- ✅ **Caricamento dinamico di bitmap di sfondo (`Background.bmp`)**  
- ✅ **Tray icon** con menu contestuale  
- ✅ **Menu a tendina completo** (File, Finestra, Aiuto)  
- ✅ **Controllo finestra "Sempre in primo piano"**  
- ✅ **Esecuzione singola istanza** tramite `CreateMutexW`  
- ✅ **Timer interno** per aggiornamento frame (40 ms)  
- ✅ **Rilevamento esecuzione come amministratore**  

---

## 🖼️ Anteprima grafica

L’animazione mostra un cerchio in movimento che rimbalza sulle pareti della finestra, variando la traiettoria e la dimensione nel tempo.  
Il bordo irregolare viene preso dal file `Background.bmp`, nella stessa cartella dell’eseguibile.

---

## 🧩 Struttura del codice

- `WinMain` — entry point dell’applicazione  
- `WndProc` — gestione dei messaggi di finestra (`WM_PAINT`, `WM_TIMER`, `WM_COMMAND`, ecc.)  
- `SetAlwaysOnTop()` — attiva o disattiva la modalità “Sempre in primo piano”  
- `IsRunningAsAdmin()` — rileva se l’app è stata avviata come amministratore  

La logica di animazione è interamente gestita dal **timer** (`WM_TIMER`), che aggiorna posizione e direzione del cerchio in base agli urti contro i bordi.

---

## 🛠️ Requisiti e compilazione

### 💻 Requisiti minimi
- Windows 10 o Windows 11 (x64)
- Visual Studio 2022 o successivo  
- **Toolset:** `v142` o `v143`  
- **Subsystem:** Windows (/SUBSYSTEM:WINDOWS)
- **Runtime:** `/MT` (static runtime, nessuna dipendenza da VCRUNTIME140.dll)

### 🧱 Compilazione
1. Apri Visual Studio  
2. Crea un progetto “Win32 Project” vuoto  
3. Aggiungi il file sorgente principale (`GraphTest.cpp`)  
4. Aggiungi una **risorsa icona** (`IDI_ICON1`) e un file `resource.h` coerente  
5. Inserisci nella cartella dell’eseguibile un file `Background.bmp`  
6. Compila in modalità **x64 Release**

Infine, in Visual Studio:
Clic destro sul progetto → Properties. Vai in: Configuration Properties → C/C++ → Code Generation:
Alla voce Runtime Library, imposta:
- Multi-threaded (/MT) per build Release
- Multi-threaded Debug (/MTd) per build Debug
Compila tutto (CTRL+SHIFT+B).

---

## 🧰 Menu principali

### **File**
- Vai a → Cartella TEMP / USER / OS  
- Esci  

### **Finestra**
- Sempre in primo piano (toggle)

### **?**
- Versione OS  
- Informazioni di Sistema  
- Circa (About box)

### **Tray Icon**
- ▶ Apri GUI  
- Esci da GraphTest  
- Menu “?” contestuale

---

## 💬 Note tecniche

- **Rendering:** GDI classico con `Ellipse()` e `BitBlt()`  
- **Flicker-free:** grazie a DC in memoria (`CreateCompatibleDC`)  
- **No GDI+**, nessuna dipendenza esterna (eseguibile completamente standalone)  
- **WM_PAINT** utilizzato come entry grafico principale  
- **WM_TIMER** aggiorna posizione, direzione e raggio del cerchio  
- **WM_SYSCOMMAND + SC_MINIMIZE** → gestione della tray icon  

---

## 🧡 Dedica

> Realizzato in occasione del compleanno di **Lara**,  
> con curiosità, tecnica e un pizzico di cuore.

---

## 📜 Crediti

**© 2025 — Alessandro Favretto**  
Senior Software Engineer · .NET · Automazione Industriale  
Vicenza, Italia  
