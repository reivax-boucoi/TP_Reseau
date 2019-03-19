# TP_Reseau
Repo de travail TP_Reseau 3IMACS-AE
> DEVEZ Laurent <br>
> BOURLOT Xavier

***

## Première partie : *tsock*

1. Seance 1   
	**v1**  
		- Réception UDP fonctionne
		- Le passage d'argument fonctionne s,p,u (non protégé)
		- Emission UDP fonctionne
2. Seance 2  
	**v2**:  
		- Emission et réception TCP fonctionnent  
	**v3**:  
		- options n et l gérées  
		- affichages de début et fin de transmission/réception formatés selon le cahier des charges  
	**v4**:  
		- commencé a nettoyer le programme (c'est pas du luxe)  
		-*v4 non implémentée pour le moment*  
		
***

## Seconde partie : *BAL*
### Structure

 * Programme principal : `main.c`
 * Programme de test : `test.c`
 * Dependances : `include/*`

### Compilation

* `make` ou `make all` pour compiler le main
* `make run` pour compiler et executer immediatement le main
* `make clean` pour nettoyer

### TODO

- [x] Fix inifinite loop when reading client
- [x] Use select to allow multiple simultaneous client connections
- [x] Make nicer and fewer printfs
- [ ] Comments !
- [ ] Adjustable message length *(bonus)*


### Issues

~~storeMsg() procedure is moving headUser instead of moving a copy !!~~
~~reading letters from any user always returns user0 letters~~
