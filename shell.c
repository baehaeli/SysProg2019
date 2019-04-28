#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

#define MAX_PATH 100
#define LF 10
#define C_BLCK "\033[30m"
#define C_GREN "\033[32m"
#define C_BLUE "\033[34m"
#define C_WHITE "\033[37m"

char* print_prompt();
void _ls();
void _pwd();
void _rm(char *cmd);
//void _rmdir(char *cmd);
void _cd(char *cmd);
void _mkdir(char *cmd);
void parse_cmd();

void _pwd(){
	char *buffer = (char *)malloc(MAX_PATH);
	char *value = (char *)getcwd(buffer, MAX_PATH);
	printf("%s\n",value);
}

void _rm(char *cmd){
	if(cmd[2]==' '){
		struct stat statbuf;
		
		char *ptr = strchr(cmd,' ');
		char *ptr2 = (char *)malloc(MAX_PATH);
		ptr = ptr + 1;
		for(int i=0;i<strlen(ptr)-1;i++){
			*(ptr2+i) = ptr[i];
		}
		lstat(ptr2,&statbuf);
		if(S_ISDIR(statbuf.st_mode)==1){
			printf("can not remove directory. to remove directory use rmdir");
			return;
		}
		
		if(unlink(ptr2)<0){
			perror("remove failed");
			exit(1);
		}
	}
}


void _ls(void){
	char *buffer = (char *)malloc(MAX_PATH);
	char *value = (char *)getcwd(buffer, MAX_PATH);
	struct  stat buf;
	int count = 0;
	char *dir_name[255];
	
	DIR *dp;
	struct dirent *dir;
	struct stat stat;
	if((dp=opendir(value)) == NULL){
		perror("can not open directory\n");
		exit(EXIT_FAILURE);
	}
	chdir(value);
	while((dir=readdir(dp)) != NULL){
		lstat(dir->d_name,&buf);
		if(S_ISDIR(buf.st_mode) == 1){
			if(!strcmp(dir->d_name,".") || !strcmp(dir->d_name,"..")){
				;
			}
			else{
				printf(C_GREN);//directory
				printf("%s\n",dir->d_name);
				printf(C_WHITE);
				dir_name[count] = dir->d_name;
				count = count + 1;
			}
		}
		else{
			printf(C_BLUE);//file
			printf("%s\n",dir->d_name);
			printf(C_WHITE);
		}
	}

	free(buffer);
	closedir(dp);


}

void _cd(char *cmd){

	char* cdpath;

	if(strcmp(cmd,"cd\n")==0){
		cdpath = NULL;
		char *path = (char*)getenv("HOME");
		if(chdir(path)<0){
			printf("can't get path\n");
		}
	}
	else if(strcmp(cmd,"cd ..\n")==0){

		char *buffer_home = (char *)malloc(MAX_PATH);
		char *buffer_cwd = (char *)malloc(MAX_PATH);

		char *value = (char *)getcwd(buffer_cwd, MAX_PATH);
		char *path = (char*)getenv("HOME");
		
		if((strlen(value) == strlen(path)) || strlen(value) < strlen(path))
		{
			char *ptr = strrchr(value,'/');
			if(ptr !=NULL){
				char *ptr2 = (char *)malloc(MAX_PATH);
				for(int i =0 ;i < strlen(value) - strlen(ptr);i++){
					*(ptr2 + i) = value[i];
				}
				if(ptr2 != NULL){
					if(chdir(ptr2)<0){
						printf("can't get path\n");
					}
				}
				
			}
		}

		else{
			if(chdir(path)<0){
				printf("can't get path\n");
			}
		
			char *value2 = (char *)getcwd(buffer_home, MAX_PATH);

			strcat(value2,"/");

			char *sub_dir = (char *)malloc(MAX_PATH);

			*sub_dir = value[strlen(value2)];
			for(int i=0 ; i< strlen(value) - strlen(value2); i++){
				*(sub_dir + i) = value[i+strlen(value2)];
			}
			char *currentdir = strrchr(sub_dir,'/');

			if(currentdir != NULL){
				char *sub_dir2 = (char *)malloc(MAX_PATH);

				
				for(int i=0 ; i< strlen(sub_dir)-strlen(currentdir); i++){
					*(sub_dir2 + i) = sub_dir[i];
				}

				
				char *ptr = strtok(sub_dir2,"/");
				while(ptr != NULL){
					if(chdir(ptr)<0){
					printf("can't get path\n");
				}
					ptr = strtok(NULL,"/");
				}
			}
			else{
				if(chdir(path)<0){
					printf("can't get path\n");
				}
			}
		}

	}
	else{
		if(cmd[2] == ' '){
			char *ptr = strchr(cmd,' ');

			ptr = ptr + 1;

			char *ptr2 = strchr(ptr,'/');
			if(ptr2 == NULL){
				char *ptr3 = (char *)malloc(MAX_PATH);

				for(int i=0;i<strlen(ptr)-1;i++){
					*(ptr3 + i) = ptr[i];

				}
				if(chdir(ptr3)<0){
					printf("can't get path\n");
				}

			}
			else{
				char *path2 = (char *)malloc(MAX_PATH);
				for(int i=0;i<strlen(ptr)-1;i++){
					*(path2 + i) = ptr[i];

				}
				char *ptr3 = strtok(path2,"/");
				while(ptr3 != NULL){

					if(chdir(ptr3)<0){
						printf("can't get path\n");
					}
					ptr3 = strtok(NULL,"/");
				}
			}

		}
	}
}
static void die(const char *s){
	perror(s);
	exit(1);
}


void make_path(const char *path){
	int result = mkdir(path,0777);
	if(result == 0){
		printf("you make new directory %s\n",path);
		return;
	}
	else if(result == -1){
		if(errno == EEXIST){
			struct stat st;
			if(stat(path,&st) <0) die("stat");
			if(!S_ISDIR(st.st_mode)){
				fprintf(stderr,"file exists but is not a directory : %s\n",path);
				exit(1);
			}
			return;
		}
		else if(errno == ENOENT){
			char *parent_path = strdup(path);
			if(!parent_path) die("strdup");
			char *last = parent_path + strlen(parent_path) -1;
			while(*last == '/' && last != parent_path){
				*last -- = '\0';
			}
			if(strcmp(parent_path,"/") == 0){
				fprintf(stderr,"error: root directory is not a directory\n");
				exit(1);
			}
			char *sep = strrchr(parent_path,'/');
			if(!sep){
				fprintf(stderr,"error: current directory is not a directory\n");
				exit(1);
			}
			else if(sep == parent_path){
				fprintf(stderr,"error:root directory is not a directory\n");
				exit(1);
			}
			*sep = '\0';

			make_path(parent_path);
			if(mkdir(path,0777)<0) die(path);
			return;
		}
		else{
			perror(path);
			exit(1);
		}
	}
}



void _mkdir(char *cmd){
	if(cmd[5] == ' '){
		char *ptr = strchr(cmd,' ');
		char *path = (char *)malloc(MAX_PATH);
		ptr = ptr+1;
		for(int i=0;i<strlen(ptr)-1;i++){
			*(path + i) = ptr[i];
		}
		make_path(path);
	}

}

void parse_cmd(char *cmd){
	char* ptr = NULL;
	int i = 0;
	
	if(strcmp(cmd,"ls\n")==0){
		_ls();
	}
	else if(strncmp(cmd,"cd",2)==0){
		_cd(cmd);
	}
	else if(strncmp(cmd,"mkdir",5)==0){
		_mkdir(cmd);
	}
	else if(strncmp(cmd,"pwd",3)==0){
		_pwd();
	}

	else if(strncmp(cmd,"rm",2)==0){
		_rm(cmd);
	}
	//else if(strncmp(cmd,"rmdir",2)==0){
	//	_rmdir(cmd);
	//}

	
	else{
		printf("your input is wrong\n");
	}
	
}




int main(){
	char c;
	char *cmd = (char*)malloc(MAX_PATH);
	char *buffer = (char *)malloc(MAX_PATH);
	char line[MAX_PATH];

	while((c=getchar()) != EOF){
		if(c==LF){
			char *value = (char *)getcwd(buffer, MAX_PATH);


			if(value != 0){
				fprintf(stdout,"haeleeshell:%s>",buffer);
				if ((cmd = fgets(line,MAX_PATH,stdin)) != NULL){
					if(strcmp(cmd,"exit\n")== 0){
						return 0;
					}

					parse_cmd(cmd);
				}
			}

			
			
		}
	}
	free(cmd);
	free(buffer);
	return 0;
}
