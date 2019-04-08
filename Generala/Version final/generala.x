struct tabla{ 
	long valores[11];
	int gano;
	int puntaje;
};

struct dados{
	int numeros[5];
};

struct jugada{
	 int dados[5];
	 int anoto;
};

program GENERALA {
	version GENERALAVERS {
		 int anotarse()=1;
		 int empezar()=2;
		 int jugadores()=3;
		 int permiso(int)=4;
		 struct jugada JugadaAnterior()=5;
		 string ResultadosParciales()=6;
		 struct dados jugar()=7;
		 int anotar(struct tabla)=8;
		 int tachar(struct tabla)=9;
		 int salir(int)=10;
		 int tirar(int)=11;
		 struct tabla terminar(int)=12;
		 int termine(int)=13;
	}=1;
}=0x20900005;
