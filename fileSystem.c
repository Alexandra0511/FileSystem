#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

void touch (Dir* parent, char* name) {
	//aloc memorie pentru noul fisier
	File* new_file = malloc(sizeof(File));
	new_file->name = malloc(strlen(name) + 1);
	strcpy(new_file->name, name);
	new_file->next = NULL;
	new_file->parent = parent;

	//daca lista de fisiere a directorului e goala, fisierul devine head
	if(parent->head_children_files == NULL)
	{
		parent->head_children_files = new_file;
		return;
	}

	//caut daca numele fisierului este deja folosit
	File* f = parent->head_children_files;
	while(f->next != NULL)
	{
		if(strcmp(f->name, name) == 0)
		{
			printf("File already exists\n");
			free(new_file->name);
			free(new_file);
			return;
		}
		f = f->next;
	}

	if(strcmp(f->name, name) == 0)
	{
		printf("File already exists\n");
		free(new_file->name);
		free(new_file);
		return;
	}

	//adaug fisierul in lista 
	f->next = new_file;
}

//analog functiei touch
void mkdir (Dir* parent, char* name) {
	Dir* new_dir = malloc(sizeof(Dir));
	new_dir->next = NULL;
	new_dir->name = malloc(strlen(name) + 1);
	strcpy(new_dir->name, name);
	new_dir->parent = parent;
	new_dir->head_children_files = NULL;

	if(parent->head_children_dirs == NULL)
	{
		parent->head_children_dirs = new_dir;
		return;
	}

	Dir* d = parent->head_children_dirs;
	while(d->next != NULL)
	{
		if(strcmp(d->name, name) == 0)
		{
			printf("Directory already exists\n");
			free(new_dir->name);
			free(new_dir);
			return;
		}
		d = d->next;
	}

	if(strcmp(d->name, name) == 0)
	{
		printf("Directory already exists\n");
		free(new_dir->name);
		free(new_dir);
		return;
	}

	d->next = new_dir;
}

void ls (Dir* parent) {
	Dir* d = parent->head_children_dirs;
	File* f = parent->head_children_files;

	while(d != NULL)
	{
		printf("%s\n", d->name);
		d = d->next;
	}
	
	while(f != NULL)
	{
		printf("%s\n", f->name);
		f = f->next;
	}	
}

void rm (Dir* parent, char* name) {
	File* f = parent->head_children_files;
	File* x = parent->head_children_files; //folosit pentru eliberarea memoriei

	if(f == NULL){
    	printf("Could not find the file\n");
		return;
	}
	
	if(strcmp(f->name, name) == 0) //fisierul cu numele "name" este head-ul listeu
	{
		parent->head_children_files = f->next;
		free(f->name);
		free(f);
		return;
	}

	while(f->next != NULL)
	{
		if(strcmp(f->next->name, name) == 0)
			break;
		f = f->next;
	}

	//am parcurs lista si nu am gasit fisierul cu nume "name"
	if(f->next == NULL){
    	printf("Could not find the file\n");
		return;
	}
    
	//fisierul cautat este ultimul din lista
    if(f->next->next == NULL)
    {
        x = f->next;
        f->next = NULL;
		free(x->name);
        free(x);
		return;
    }
	x = f->next;
    f->next = f->next->next;
	free(x->name);
    free(x);
}

//functie pentru stergerea recursiva a continutului unui director
void rmdir_rec(Dir* dir){
	if(dir == NULL)
		return;
	
	//sterg lista de fisiere a directorului
	File* f = dir->head_children_files;
	File* p;
	while(f != NULL){
		p = f->next;
		rm(dir, f->name);
		f = p;
	}

	//sterg recursiv lista de directoare
	Dir* d = dir->head_children_dirs;
	Dir* d2;
	while(d != NULL){
		d2 = d->next;
		rmdir_rec(d);
		free(d->name);
		free(d);
		d = d2;
	}
}

void rmdir (Dir* parent, char* name) {
	Dir* d = parent->head_children_dirs;
	Dir* d2;

	if(d == NULL){
		printf("Could not find the dir\n");
		return;
	}

	if(strcmp(d->name, name) == 0)
	{
		parent->head_children_dirs = d->next;
		rmdir_rec(d);
		free(d->name);
		free(d);
		return;
	}

	while(d->next != NULL){
		if(strcmp(d->next->name, name) == 0)
			break;
		d = d->next;
	}

	if(d->next == NULL){
		printf("Could not find the dir\n");
		return;
	}

	if(d->next->next == NULL)
    {
        d2 = d->next;
        d->next = NULL;
        rmdir_rec(d2);
		free(d2->name);
		free(d2);
		return;
    }
    d2 = d->next;
    d->next = d->next->next;
	rmdir_rec(d2);
	free(d2->name);
	free(d2);
}

void cd(Dir** target, char *name) {
	Dir* d = (*target)->head_children_dirs;

	if(strcmp(name, "..") == 0){
		//directorul curent nu are un director parinte
		if((*target)->parent == NULL)
			return;
		(*target) = (*target)->parent;
		return;
	}

	if(d == NULL){
		printf("No directories found!\n");
		return;
	}
	
	//caut directorul cu nume "name"
	while(d != NULL){
		if(strcmp(d->name, name) == 0)
			break;
		d = d->next;
	}
	if(d == NULL){
		printf("No directories found!\n");
		return;
	}

	//schimb directorul actual in cel cu nume "name"
	(*target) = d;
}

char *pwd (Dir* target) {
	Dir* p = target->parent;

	if(p == NULL){	//director fara parinte
		char *string = malloc(100);
		strcpy(string, "/");
		strcat(string, target->name);
		return string;
	}

	char *new_string = pwd(p);
	strcat(new_string, "/");
	strcat(new_string, target->name);

	return new_string;
}

void stop (Dir* target) {
	//eliberez toata memoria directorului target
	rmdir_rec(target);
	free(target->name);
	free(target);
}

void tree (Dir* target, int level) {
	if(target == NULL)
		return;

	Dir* d = target->head_children_dirs;
	while(d != NULL)
	{
		//afisez un numar de level * 4 spatii in fata fiecarui director
		for(int i = 0; i < level; i++)
			printf("    ");
		printf("%s\n", d->name);
		//intrand mai adanc in ierarhie, incrementez si level-ul
		tree(d, level+1);
		d = d->next;
	}

	File* f = target->head_children_files;
	while(f != NULL)
	{
		//afisez un numar de level * 4 spatii in fata fiecarui fisier
		for(int i = 0; i < level; i++)
			printf("    ");
		printf("%s\n", f->name);
		f = f->next;
	}
}

void mv(Dir* parent, char *oldname, char *newname) {
	Dir* d1 = parent->head_children_dirs;
	Dir* d2 = d1;
	File* f1 = parent->head_children_files;
	File* f2 = f1;
	int ok = 0; //pentru a determina daca numele apartine unui fisier sau unui director
	
	if((f1 == NULL) && (d1 == NULL)){
		printf("File/Director not found\n");
		return;
	}

	//parcurg pentru a determina daca "oldname" este fisier/director
	while(d1 != NULL)
	{
		if(strcmp(d1->name, oldname) == 0){
			ok = 1; // numele apartine unui director
			break;
		}
		d1 = d1->next;
	}
	while(f1 != NULL)
	{
		if(strcmp(f1->name, oldname) == 0){
			ok = 2; // numele apartine unui fisier
			break;
		}	
		f1 = f1->next;
	}

	//parcurg pentru a determina daca "newname" este deja folosit
	if((f1 == NULL) && (d1 == NULL)){
		printf("File/Director not found\n");
		return;
	}

	while(d2 != NULL){
		if(strcmp(d2->name, newname) == 0){
			printf("File/Director already exists\n");
			return;
		}
		d2 = d2->next;
	}
	while(f2 != NULL){
		if(strcmp(f2->name, newname) == 0){
			printf("File/Director already exists\n");
			return;
		}
		f2 = f2->next;
	}

	if(ok == 1){ //cazul pentru director
		//creez un nou director cu numele "newname"
		Dir* new_dir = malloc(sizeof(Dir));
		new_dir->next = NULL;
		new_dir->name = malloc(strlen(newname) + 1);
		strcpy(new_dir->name, newname);
		new_dir->parent = parent;

		//adaug directorul in lista de directoare a parintelui
		Dir* p = parent->head_children_dirs;
		while(p->next != NULL)
			p = p->next;
		p->next = new_dir;

		//mut toate fisierele si directoarele din directorul vechi in cel nou
		new_dir->head_children_files = d1->head_children_files;
		new_dir->head_children_dirs = d1->head_children_dirs;

		//sterg directorul vechi
		d1->head_children_dirs = NULL;
		d1->head_children_files = NULL;
		rmdir(parent, d1->name);
	}

	if(ok == 2){ //cazul pentru fisier
		//creez noul fisier
		File* new_file = malloc(sizeof(File));
		new_file->name = malloc(strlen(newname)+1);
		strcpy(new_file->name, newname);
		new_file->next = NULL;
		new_file->parent = parent;

		//il adaug in lista de fisiere a parintelui
		File* file = parent->head_children_files;
		while(file->next != NULL)
			file = file->next;
		file->next = new_file;

		//sterg fisierul vechi
		rm(parent, f1->name);
	}
}

int main () {
	char* input = malloc(MAX_INPUT_LINE_SIZE);

	//creez directorul home
	Dir* home = malloc(sizeof(Dir));
	home->next = NULL;
	home->name = malloc(strlen("home") + 1);
	strcpy(home->name, "home");
	home->parent = NULL;
	home->head_children_dirs = NULL;
	home->head_children_files = NULL;

	char* p, *p1, *p2;
	Dir* home_princ = home; 
	do
	{
		/*
		Summary:
			Reads from stdin a string and breaks it down into command and in
			case it needs into a name.
		*/
		fgets(input, 30, stdin);

		//sterg new-line-ul pe care il citeste fgets
		input[strlen(input)-1] = '\0';

		//iau primul cuvant din string care corespunde unei functii
		p = strtok(input, " ");
		if(strcmp(p, "touch") == 0)
		{
			//iau al doilea cuvant din string care corespunde celui de al doilea parametru al functiei
			p = strtok(NULL, " ");
			touch(home, p);
			
		}
		else if(strcmp(p, "mkdir") == 0)
		{
			p = strtok(NULL, " ");
			mkdir(home, p);
		}
		else if(strcmp(p, "rm") == 0)
		{
			p = strtok(NULL, " ");
			rm(home, p);
		}
		else if(strcmp(p, "rmdir") == 0)
		{
			p = strtok(NULL, " ");
			rmdir(home, p);
		}
		else if(strcmp(p, "tree") == 0)
		{
			tree(home, 0);
		}
		else if(strcmp(p, "cd") == 0)
		{
			p = strtok(NULL, " ");
			cd((&home), p);
		}
		else if(strcmp(p, "pwd") == 0)
		{
			char *string = pwd(home);
			fprintf(stdout, "%s\n", string);
			free(string);
		}
		else if(strcmp(p, "ls") == 0)
		{
			ls(home);
		}
		else if(strcmp(p, "mv") == 0)
		{
			p1 = strtok(NULL, " ");
			p2 = strtok(NULL, " ");
			mv(home, p1, p2);
		}
		else if(strcmp(p, "stop") == 0)
		{
			stop(home_princ);
			break;
		}
	} while (1 == 1); 
	
	free(input);

	return 0;
}
