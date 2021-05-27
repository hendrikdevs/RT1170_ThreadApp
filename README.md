# ThreadApp

Applikation zum testen der Interprozesskommunikation sowie Multithreading unter Zephyr. 

- Validation Thread (v1) im Usermode
	- Empfängt Nachrichten von den Gerätetreibern via extern_to_validation FIFO
	- "Validieren" der Nachricht
	- packt Nachricht in FIFO zum Worker Thread

- Worker Thread (w1) im Kernelmode
	- Empfängt die Nachricht aus der validation_to_worker FIFO
	- wertet die Nachricht aus
	- zurücksenden der bearbeiteten Nachricht an den Gerätetreiber via Callback
	
