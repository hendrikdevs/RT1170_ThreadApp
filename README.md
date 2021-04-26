# ThreadApp

- User Thread
	- CAN / Ethernet Kommunikation?
- Kernel Thread
	- Auswertung der CAN Nachricht
	- Austausch mit User Mode Thread, der dann Antwort sendet
	
- Austausch per FIFO?
	- Nachrichten können Priorisiert sein
	- Pro Priorität eine FIFO vom kleinen CPU zum großen
	- Eine FIFO Queue für alle Nachrichten vom großen CPU zum kleinen
	- Interrupt wenn CAN Nachricht kommt & wenn Item in FIFO Queue (vom großen CPU zum kleinen) liegt, sonst in sleep Modus gehen
