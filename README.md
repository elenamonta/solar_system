# Solar System
### Descrizione
Il progetto rappresenta una simulazione interattiva di un sistema solare tridimensionale, realizzata in OpenGL. L’obiettivo è fornire un'esperienza immersiva, permettendo all’utente di esplorare lo spazio, osservare i pianeti orbitanti attorno al Sole e interagire con essi tramite controlli della telecamera e un’interfaccia grafica (GUI)
### Installazione e esecuzione
Per scaricare il progetto, clona la repository e assicurati di avviarlo in modalità **Release**
### Controlli
Una volta avviato il programma, puoi muoverti all'interno della scena utilizzando:
- W, A, S, D - Movimento avanti, indietro, sinistra e destra
- Mouse - Rotazione della visuale
- Rotellina del mouse - Zoom in/out
### Modalità di visualizzazione 
Sono disponibili due modalità di navigazione all'interno della scena:
- Trackball Mode
- Free Mode
La modalità di visualizzazione può essere modificata tramite l'interfaccia ImGui.
### Interazioni con la scena
Tramite i widget di imgui è possibilie:
- Modificare le posizioni delle due luci puntiformi
- Selezionare il materiale del Sole, tra un insieme di materiali di base
- Selezionare il modello di illuminazione: Phong, Blinn-Phong, Gourad
- Scegliere la modalità di navigazione all’interno della scena: Trackball mode, Free mode
- Visualizzare il nome dell'oggetto selezionato all'interno della scena
### Tecnologie utilizzate
Il progetto è stato sviluppato utilizzando le seguenti librerie:
- OpenGL - Libreria grafica utilizzata
- GLAD - Loader-Generator per OpenGL/GLES/EGL/GLX/WGL
- GLFW - Gestione di finestre, contesti e input
- GLM - Libreria matematica
- ImGui - Interfaccia utente grafica per debug e modifiche runtime
