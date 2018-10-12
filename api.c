//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//FILE* f_log = NULL;
//int contamalloc = 0;


struct riga {
	unsigned i;
	char l;
	char s;
	short m;
	unsigned f;
};

struct acc //accettazione
{
	int n_stato;
	struct acc* next;
};

struct inp //lista di input
{
	struct string2* s; //stringa di input
	struct inp* next;
};

struct rigals //lista di input
{
	struct riga s; //stringa di input
	struct rigals* next;
};


struct mossa {
	struct string2* stringa; //nastro
	int n_testa; //posizione della testa
	int n_mosse;
	int stato_corrente;
};

struct string2 {
	char* s;
	unsigned n_stringa;
	unsigned quante_volte_mi_usano;
};


struct soluz {
	char s;
	struct soluz* next;
};

struct mossal {
	struct mossa m;
	struct mossal* next;
};



#define CHAR_NIENTE ('_')
#define N_TRANS  (1024)
#define N_ACCET (32)
#define N_CHAR (75)
#define N_ACCRESCI_NASTRO (100)

long max;
struct acc* accl[N_ACCET] = { NULL }; //accettazione list

struct rigals* rigal[N_TRANS][N_CHAR] = { { NULL } }; //lista di righe di transizioni
char soluz;

struct mossal* lista_mosse_coda;

inline void* malloc2(size_t t)
{
	/*
	int chiamante = 0;
	__asm {
	mov eax, [ebp + 4]
	mov[chiamante], eax
	}
	*/
	void* r = malloc(t);
	//fprintf(f_log, "M %p %d %d %d\n", r, (int)t, chiamante, contamalloc);
	//contamalloc++;
	return r;
}

inline void free2(void* p)
{
	/*
	unsigned chiamante = 0;
	_asm {
	mov eax, [ebp + 4]
	mov[chiamante], eax
	}

	fprintf(f_log, "F %p %d\n", p, chiamante);
	*/
	free(p);
}

size_t getline2(char **lineptr, size_t *n, FILE *stream) {
	char *bufptr = NULL;

	size_t size = *n;


	unsigned l_read = 0;

	if (lineptr == NULL) {
		return -1;
	}
	if (stream == NULL) {
		return -1;
	}
	if (n == NULL) {
		return -1;
	}
	bufptr = *lineptr;


	int c = fgetc(stream);
	if (c == EOF) {
		return -1;
	}
	if (bufptr == NULL) {
		bufptr = (char*)malloc2(128);
		if (bufptr == NULL) {
			return -1;
		}
		size = 128;
	}

	while (c != EOF) {
		if ((l_read) > ((unsigned)(size - 1))) {
			size = size + 128;
			bufptr = (char*)realloc(bufptr, size);
			if (bufptr == NULL) {
				return -1;
			}
		}
		bufptr[l_read] = c;
		if (c == '\n') {
			break;
		}
		c = fgetc(stream);
		l_read++;
	}
	if (c == EOF)
	{
		if ((l_read) > ((unsigned)(size - 1))) {
			size = size + 128;
			bufptr = (char*)realloc(bufptr, size);
			if (bufptr == NULL) {
				return -1;
			}
		}
		//l_read++;
	}

	bufptr[l_read] = '\0';
	*lineptr = bufptr;
	*n = size;

	return l_read;
}


inline void insert_top_a(int num, struct acc *head[N_ACCET]) {
	int d = num & (N_ACCET - 1);
	struct acc *new_node = NULL;
	new_node = (struct acc*) malloc2(sizeof(struct acc));
	new_node->n_stato = num;
	new_node->next = head[d];
	head[d] = new_node;
}

inline struct soluz* insert_top_s(char num, struct soluz *head) {
	struct soluz *new_node = NULL;
	new_node = (struct soluz*) malloc2(sizeof(struct soluz));
	new_node->s = num;
	new_node->next = head;
	head = new_node;
	return head;
}

inline void insert_top_t(struct riga* num) {
	char destinazione_colonna = num->l - '0';
	int d1 = (num->i)&(N_TRANS - 1);
	struct rigals *new_node = NULL;
	new_node = (struct rigals*) malloc2(sizeof(struct rigals));
	new_node->s.f = num->f;
	new_node->s.i = num->i;
	new_node->s.l = num->l;
	new_node->s.m = num->m;
	new_node->s.s = num->s;
	new_node->next = rigal[d1][destinazione_colonna];
	rigal[d1][destinazione_colonna] = new_node;
}

inline struct inp* insert_top_i(char* s, unsigned l, struct inp *head) {
	struct inp *new_node = NULL;
	new_node = (struct inp*) malloc2(sizeof(struct inp));
	new_node->s = (struct string2*)malloc2(sizeof(struct string2));
	new_node->s->s = (char*)malloc2(sizeof(char)*l);
	new_node->s->n_stringa = l;
	unsigned i = 0;
	for (i = 0; i<l; i++)
		new_node->s->s[i] = s[i];

	new_node->next = head;
	head = new_node;
	return head;
}

inline struct string2* string_copy(struct string2* source, int inc)
{
	struct string2* x = (struct string2*)malloc2(sizeof(struct string2));

	if (inc == 0)
	{
		x->n_stringa = source->n_stringa;
	}
	else
	{
		x->n_stringa = source->n_stringa + N_ACCRESCI_NASTRO;
	}
	x->s = (char*)malloc2((sizeof(char))*(x->n_stringa));

	if (inc == 0)
	{
		memcpy(x->s, source->s, source->n_stringa);
	}
	else if (inc == 1)
	{
		memcpy(x->s, source->s, source->n_stringa);

		char* s2 = x->s;
		s2 += x->n_stringa - N_ACCRESCI_NASTRO;
		memset(s2, CHAR_NIENTE, N_ACCRESCI_NASTRO);
	}
	else if (inc == -1)
	{
		char* s2 = x->s;
		s2 += N_ACCRESCI_NASTRO;
		memcpy(s2, source->s, source->n_stringa);

		memset(x->s, CHAR_NIENTE, N_ACCRESCI_NASTRO);
	}

	return x;
}







inline void AggiornaSoluzione(char s)
{
	if (s == '1')
	{
		soluz = '1';
	}
	else if (s == 'U')
	{
		if (soluz != '1')
		{
			soluz = 'U';
		}
	}
}

inline struct rigals* Risolvi(struct mossa m)
{
	if (m.n_mosse > max)
	{
		AggiornaSoluzione('U');
		m.stringa->quante_volte_mi_usano--;
		return NULL;
	}

	int quanti_figli = 0;
	char char_puntato_nastro = m.stringa->s[m.n_testa];
	int d1 = m.stato_corrente & (N_TRANS - 1);

	struct rigals* t_ritornare = NULL;

	struct rigals* t = rigal[d1][char_puntato_nastro - '0'];
	while (t != NULL)
	{
		if (t->s.i == m.stato_corrente)
		{
			if (t->s.l == char_puntato_nastro)
			{
				t_ritornare = t;
				quanti_figli++;
			}
		}
		t = t->next;
	}

	if (quanti_figli == 1)
	{
		return t_ritornare;
	}

	t = rigal[d1][char_puntato_nastro - '0'];
	while (t != NULL)
	{
		if (t->s.i == m.stato_corrente)
		{
			if (t->s.l == char_puntato_nastro)
			{
				
				if (t->s.f == t->s.i)
				{
					if (t->s.l == CHAR_NIENTE && t->s.m == -1 && m.n_testa == N_ACCRESCI_NASTRO - 1)
					{
						AggiornaSoluzione('U');
						t = t->next;
						continue;
					}
					else if (t->s.l == CHAR_NIENTE && t->s.m == 1 && m.n_testa == m.stringa->n_stringa - N_ACCRESCI_NASTRO)
					{
						AggiornaSoluzione('U');
						t = t->next;
						continue;
					}
					else if (t->s.m == 0 && t->s.l == t->s.s)
					{
						AggiornaSoluzione('U');
						t = t->next;
						continue;
					}
				}
				

				//questo vuole dire che questa transizione è permessa, e quindi bisogna farla :)
				struct mossal* ml = (struct mossal*)malloc2(sizeof(struct mossal));

				ml->m.n_mosse = m.n_mosse + 1;
				ml->m.stato_corrente = t->s.f;

				int n_testa2 = m.n_testa;

				ml->m.n_testa = n_testa2 + t->s.m;

				int inc = 0;
				if (ml->m.n_testa >= (int)(m.stringa->n_stringa))
				{
					inc = 1;
				}
				else if (ml->m.n_testa < 0)
				{
					inc = -1;
					n_testa2 += N_ACCRESCI_NASTRO;
					ml->m.n_testa = N_ACCRESCI_NASTRO - 1;
				}

				struct string2* s2 = NULL;

				if (t->s.l == t->s.s && inc == 0)
				{
					s2 = m.stringa;
					s2->quante_volte_mi_usano++;
				}
				else
				{
					s2 = string_copy(m.stringa, inc);
					s2->s[n_testa2] = t->s.s;
					s2->quante_volte_mi_usano = 1;
				}

				ml->m.stringa = s2;




				ml->next = NULL;
				lista_mosse_coda->next = ml;
				lista_mosse_coda = ml;


				if (ml->m.n_mosse <= max)
				{
					struct acc* t2 = accl[ml->m.stato_corrente&(N_ACCET - 1)];
					while (t2 != NULL)
					{
						if (ml->m.stato_corrente == t2->n_stato)
						{
							AggiornaSoluzione('1');
							ml->m.stringa->quante_volte_mi_usano--;
							m.stringa->quante_volte_mi_usano--;
							return NULL;
						}

						t2 = t2->next;
					}
				}

			}
		}
		t = t->next;
	}

	m.stringa->quante_volte_mi_usano--;
	return NULL;
}



inline void Risolvi2(struct mossal* m)
{
	char gia_vinto = 0;
	if (m->m.n_mosse <= max)
	{
		struct acc* t2 = accl[m->m.stato_corrente&(N_ACCET - 1)];
		while (t2 != NULL)
		{
			if (m->m.stato_corrente == t2->n_stato)
			{
				AggiornaSoluzione('1');
				m->m.stringa->quante_volte_mi_usano--;
				gia_vinto = 1;
			}

			t2 = t2->next;
		}
	}

	lista_mosse_coda = m;

	struct mossal* t = lista_mosse_coda;

	if (gia_vinto==0)
	{

		do {
			if (soluz == '1')
				break;

			struct rigals* t_eseguire = Risolvi(t->m);






			if (t_eseguire == NULL)
			{
				struct mossal* to_delete = t;
				t = t->next;
				if (to_delete->m.stringa->quante_volte_mi_usano <= 0)
				{
					free2(to_delete->m.stringa->s);
					free2(to_delete->m.stringa);
				}
				free2(to_delete);
			}
			else
			{
				if (t_eseguire->s.f == t_eseguire->s.i)
				{
					if (t_eseguire->s.l == CHAR_NIENTE && t_eseguire->s.m == -1 && t->m.n_testa == N_ACCRESCI_NASTRO - 1)
					{
						AggiornaSoluzione('U');
						struct mossal* to_delete = t;
						t = t->next;
						if (to_delete->m.stringa->quante_volte_mi_usano <= 0)
						{
							free2(to_delete->m.stringa->s);
							free2(to_delete->m.stringa);
						}
						free2(to_delete);
						continue;
					}
					else if (t_eseguire->s.l == CHAR_NIENTE && t_eseguire->s.m == 1 && t->m.n_testa == t->m.stringa->n_stringa - N_ACCRESCI_NASTRO)
					{
						AggiornaSoluzione('U');
						struct mossal* to_delete = t;
						t = t->next;
						if (to_delete->m.stringa->quante_volte_mi_usano <= 0)
						{
							free2(to_delete->m.stringa->s);
							free2(to_delete->m.stringa);
						}
						free2(to_delete);
						continue;
					}
					else if (t_eseguire->s.m == 0 && t_eseguire->s.l == t_eseguire->s.s)
					{
						AggiornaSoluzione('U');
						struct mossal* to_delete = t;
						t = t->next;
						if (to_delete->m.stringa->quante_volte_mi_usano <= 0)
						{
							free2(to_delete->m.stringa->s);
							free2(to_delete->m.stringa);
						}
						free2(to_delete);
						continue;
					}
				}


				t->m.n_mosse++;
				t->m.stato_corrente = t_eseguire->s.f;

				int n_testa2 = t->m.n_testa;

				t->m.n_testa = n_testa2 + t_eseguire->s.m;

				int inc = 0;
				if (t->m.n_testa >= (int)(t->m.stringa->n_stringa))
				{
					inc = 1;
				}
				else if (t->m.n_testa < 0)
				{
					inc = -1;
					n_testa2 += N_ACCRESCI_NASTRO;
					t->m.n_testa = N_ACCRESCI_NASTRO - 1;
				}

				struct string2* s2 = NULL;

				if (t_eseguire->s.l != t_eseguire->s.s || inc != 0)
				{
					s2 = string_copy(t->m.stringa, inc);

					t->m.stringa->quante_volte_mi_usano--;
					if (t->m.stringa->quante_volte_mi_usano <= 0)
					{
						free2(t->m.stringa->s);
						free2(t->m.stringa);
					}

					s2->s[n_testa2] = t_eseguire->s.s;
					s2->quante_volte_mi_usano = 1;
					t->m.stringa = s2;
				}

				if (t->m.n_mosse <= max)
				{
					struct acc* t2 = accl[t->m.stato_corrente&(N_ACCET - 1)];
					while (t2 != NULL)
					{
						if (t->m.stato_corrente == t2->n_stato)
						{
							AggiornaSoluzione('1');
							t->m.stringa->quante_volte_mi_usano--;
							break;
						}

						t2 = t2->next;
					}
				}
			}



		} while (t != NULL);
	}
	while (t != NULL)	//POTREBBE ESSERE USCITO PERCHE SOLUZ=1 MA VA PULITA LO STESSO LA CODA
	{
		struct mossal* to_delete = t;
		t = t->next;

		if (to_delete->m.stringa->quante_volte_mi_usano <= 0)
		{
			free2(to_delete->m.stringa->s);
			free2(to_delete->m.stringa);
		}

		free2(to_delete);
	}


}

int main()
{
	FILE *fp;
	//fp = fopen("C:\\Users\\FedericoArmellini\\source\\repos\\ProgettoApiTuring\\input_slide\\1.txt", "r");
	//fp = fopen("C:\\Users\\FedericoArmellini\\source\\repos\\ProgettoApiTuring\\pubblici\\input\\fl.txt", "r");
	//fp = fopen("C:\\Users\\FedericoArmellini\\source\\repos\\ProgettoApiTuring\\PossibiliInput\\41.txt", "r");
	fp = stdin;

	//f_log = fopen("C:\\git\\ProgettoApiTuring\\log\\log.txt","w");

	if (fp == NULL)
	{
		printf("Error!");
		return 1;
	}

	size_t read = 0;
	char * line = NULL;
	size_t len = 0;

	read = getline2(&line, &len, fp); // tr 

	while ((read = getline2(&line, &len, fp)) != -1) {
		if (line[0] == 'a')
			break;


		struct riga x;
		int i = 1;
		x.i = line[0] - '0';
		while (line[i] != ' ')// && line[i] != '\n' && line[i] != '\0')
		{
			x.i *= 10;
			x.i += line[i] - '0';
			i++;
		}

		while (line[i] == ' ')
		{
			i++;
		}

		x.l = line[i];
		i++;

		while (line[i] == ' ')
		{
			i++;
		}

		x.s = line[i];
		i++;

		while (line[i] == ' ')
		{
			i++;
		}

		switch (line[i])
		{
		case 'L':
		{
			x.m = -1;
			break;
		}
		case 'R':
		{
			x.m = 1;
			break;
		}
		default:
		{
			x.m = 0;
			break;
		}
		}
		i++;

		while (line[i] == ' ')
		{
			i++;
		}

		x.f = line[i] - '0';
		i++;
		while (line[i] >= '0' && line[i] <= '9')
		{
			x.f *= 10;
			x.f += line[i] - '0';
			i++;
		}

		insert_top_t(&x);
	}

	while ((read = getline2(&line, &len, fp)) != -1) {
		if (line[0] == 'm')
		{
			break;
		}


		int i = 1;
		int n = line[0] - '0';
		while (line[i] >= '0' && line[i] <= '9')
		{
			n *= 10;
			n += line[i] - '0';
			i++;
		}
		insert_top_a(n, accl);
	}

	read = getline2(&line, &len, fp); //max
	int i = 1;
	max = line[0] - '0';
	while (line[i] >= '0' && line[i] <= '9')
	{
		max *= 10;
		max += line[i] - '0';
		i++;
	}

	//  printf("%d",max);


	read = getline2(&line, &len, fp); //"run"
	while ((read = getline2(&line, &len, fp)) != -1) {


		unsigned r2 = (unsigned)read;
		struct mossal* ml = (struct mossal*)malloc2(sizeof(struct mossal));
		ml->m.stringa = (struct string2*)malloc2(sizeof(struct string2));
		char* s4 = (char*)malloc2(sizeof(char)*r2);
		for (unsigned i = 0; i<r2; i++)
		{
			s4[i] = line[i];
		}
		ml->m.stringa->s = s4;
		ml->m.stringa->n_stringa = r2;
		ml->m.stringa->quante_volte_mi_usano = 1;

		ml->m.n_testa = 0;
		ml->m.n_mosse = 0;
		ml->m.stato_corrente = 0;

		ml->next = NULL;

		soluz = '0';
		Risolvi2(ml);
		printf("%c\n", soluz);
	}

	free2(line);


	fclose(fp);

	for (int i = 0; i < N_ACCET; i++)
	{
		struct acc* tr = accl[i];
		while (tr != NULL)
		{
			struct acc* td = tr;
			tr = tr->next;
			free2(td);
		}
	}

	for (unsigned i = 0; i < N_TRANS; i++)
	{
		for (unsigned j = 0; j < N_CHAR; j++)
		{
			struct rigals* tr2 = rigal[i][j];
			while (tr2 != NULL)
			{
				struct rigals* td2 = tr2;
				tr2 = tr2->next;
				free2(td2);
			}
		}
	}


	//fclose(f_log);
	//system("pause");
	return 0;
}
