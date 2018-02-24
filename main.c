#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libgen.h>
#define MAX 500




char* read_line(){
//printf("READ_LINE_BEGIN\n");

char * line= readline("");

if(strlen(line)>0)
        add_history(line);

//printf("READ_LINE_END\n");
return line;
}

int split_line(char**args, char *line){
//printf("SPLIT_LINE_BEGIN\n");

int i=0;
char *temp;
temp = (char*) malloc(MAX);
strcpy(temp,line);
*(args+i) = strtok(temp," ");

if(*(args+i) == NULL)
    return 1;

while(*(args+i) != NULL){

    i++;
    *(args+i) = strtok(NULL," ");

}

//printf("SPLIT_LINE_END\n");
return 1;
}


int read_splitted_line(char**args, char **line){
    //printf("READ_SPLITTED_LINE_BEGINN\n");

    strcpy(*line,read_line());
    split_line(args,*line);

   // printf("READ_SPLITTED_LINE_END\n");
    return 1;

}

int cd(char *p){



    char *current = (char*) malloc (MAX);


    //strcat(current,path);
    if(!chdir(p)){
        getcwd(current,MAX);
        printf("PATH: %s",current);
        strcat(current,"/");
        return 0;
    }

    else
    if(strcmp(p,"../")!=0)
       {
        perror("No such file\n");
        return 1;

    }



}

int isFile(const char*path){

struct stat path_stat;

if(stat(path,&path_stat))
    {
    perror("Stat error");
    return 5;
    }


if(S_ISREG(path_stat.st_mode))
    return 0;

if(S_ISDIR(path_stat.st_mode))
    return 1;

if(stat(path,&path_stat))
    return 2;

    return 3;

}


int remove_folder(const char*path,int iFound, int vFound,char *filename){
    int a = -1;
    char *buf;
    int length = strlen(path);
    char temp[200];
    DIR* file;

    if(iFound == 1)
        {
        printf("rm: descend into directory '%s' ?\n",filename);
        read(0,temp,5);
        temp[2] = 0;

        if(strcmp(temp,"no")==0)
        return 0;
        }
    file = opendir(path);

    if(file){

    struct dirent *p;

    a =0;
    while(!a && (p = readdir(file))){

        int b = -1;
        int l ;

        if(!strcmp(p->d_name,".")|| !strcmp(p->d_name,".."))
            continue;

        l = length + strlen(p->d_name) +2;
        buf = malloc(l);

        if(buf){

            struct stat statbuf;
            snprintf(buf,l,"%s/%s",path,p->d_name);
            if(!stat(buf,&statbuf)){

                if(S_ISDIR(statbuf.st_mode))
                    b = remove_folder(buf,iFound,vFound,p->d_name);


                else

        {

        //!Caz1- nu are flaguri

        if(vFound == 0 && iFound == 0)
            {
                b = unlink(buf);
            }

        else //!Caz2 - are numai flagul i;
        if(iFound == 1 && vFound == 0){

            printf("rm- remove regular file '%s/%s' Answer with yes/no\n",filename,p->d_name);

            read(0,temp,5);
            temp[3] = 0;
            if(strcmp(temp,"yes")==0)
               {
                    b = unlink(buf);
                }

            else return 0;

        }

            else //!Caz3 - are numai flagul v

                if(iFound == 0 && vFound == 1){

            b = unlink(buf);
            printf("removed '%s/%s' \n",filename,p->d_name);

        }

            else //!Caz4 - are flagurile v si i

                if(iFound ==1 && vFound ==1){

            printf("rm- remove regular file '%s/%s' Answer with yes/no\n",filename,p->d_name);
            read(0,temp,5);
            temp[3] = 0;

            if(strcmp(temp,"yes")==0)
               {

                b = unlink(buf);
                printf("removed '%s/%s' \n",filename,p->d_name);

                }
            else return 0;

        }


                }


        }

        free(buf);
        }

        a = b;
    }
    closedir(file);



    }

    if(!a)

        if(iFound == 0 && vFound ==0)
        a = rmdir(path);

        else
        if(iFound ==1 && vFound ==0)
        {

            printf("rm: remove directory '%s'?\n",filename);
            read(0,temp,5);
            temp[3] = 0;
            if(strcmp(temp,"yes")==0)
               {
                    a = rmdir(path);
                }

            else return 0;


        }
        else
        if(iFound == 0 && vFound ==1 ){

            a = rmdir(path);
            printf("removed directory '%s' \n",filename);

        }

        else
        if(iFound == 1 && vFound == 1){


            printf("rm: remove directory '%s'?\n",filename);
            read(0,temp,5);
            temp[3] = 0;
            if(strcmp(temp,"yes")==0)
               {
                    a = rmdir(path);
                    printf("removed directory '%s' \n",filename);
               }

            else return 0;
        }


    return a;


}



void rm(char **args){

int i=1;
int rFound=0,vFound=0,iFound=0;
char * filename;
char *path = (char*) malloc(MAX);
char temp[5];
filename = (char*) malloc(MAX);

while(*(args+i) != NULL){

    if(!strcmp(*(args+i),"-r") || !strcmp(*(args+i),"-R"))
        rFound = 1;
    else
    if(!strcmp(*(args+i),"-i"))
        iFound = 1;
    else
    if(!strcmp(*(args+i),"-v"))
        vFound = 1;
    else {
          strcpy(filename,*(args+i));
          break;
          }
    i++;
    }

//    printf("Vfound is : %d\n",vFound);
//    printf("Rfound is : %d\n",rFound);
//    printf("Ifound is : %d\n",iFound);
//    printf("filename is : %s\n",filename);

//! 1 pt fisier folder, 0 pt fisier normal;
    getcwd(path,MAX);
    strcat(path,"/");
    strcat(path,filename);
    //printf("%s\n",path);

   // printf("Is file returneaza %d",isFile(path));
     //fisier normal
    // -R nu conteaza la fisierul normal
    if(isFile(path)==0)
        {


        //!Caz1- nu are flaguri

        if(vFound == 0 && iFound == 0)
            {
            unlink(filename);
            }

        else //!Caz2 - are numai flagul i;
        if(iFound == 1 && vFound == 0){

            printf("rm- remove regular file? Answer with yes/no\n");

            read(0,temp,5);
            temp[3] = 0;
            if(strcmp(temp,"yes")==0)
               {

                unlink(filename);

               }

        }

        else //!Caz3 - are numai flagul v
        if(iFound == 0 && vFound == 1){

            unlink(filename);
            printf("removed '%s' \n",filename);

        }

        else //!Caz4 - are flagurile v si i
        if(iFound ==1 && vFound ==1){

            printf("rm- remove regular file? Answer with yes/no\n");
            read(0,temp,5);
            temp[3] = 0;

            if(strcmp(temp,"yes")==0)
               {

                unlink(filename);
                printf("removed '%s' \n",filename);

                }

        }

        }
    else{

    strcat(path,"/");
    //folder
    //! Sigur are flagul R
     if(isFile(path) == 1)
        {

            if(rFound == 0)
                printf("rm: cannot remove '%s': Is a directory",filename);
            //!Caz 1 - nu are flaguri
            else
                remove_folder(path,iFound,vFound,filename);



        }
           //! In caz ca nu exista
    else if(isFile(path)==2){
         printf("rm: cannot remove '%s': No such file or directory\n",filename);
    }}




    exit(0);


}

char* split_path(char* splitted[20], char* path,int pos){

    char *temp = (char*) malloc(MAX);
    char *temp2 = (char*) malloc(MAX);
    char* special = (char*) malloc(MAX);
    int length= 0;
    strcpy(temp,path);
    strcpy(special, "");
    if(pos!=0)
    {
    *(temp+pos-1) = '\0';
    strcpy(special, path+pos);
    }
    int i = 0;

    *(splitted+i) = strtok(temp,"/");
    length = length + strlen(*(splitted+i));

    while(*(splitted+i) != NULL){

        i++;
        *(splitted+i) = strtok(NULL,"/");

//
//        if(*(path+length+i+1) == '{')
//            {
//            i++;
//            *(splitted+i) = strtok(NULL,"}");
//            break;
//            }



}

        return special;




}

int count_slashes(char *s){

    int count =0;
    for(int i=0; i<strlen(s);i++)
        if(*(s+i) == '/')
            count++;
    return count;
}

int count_comas(char *s){

    int count =0;
    for(int i=0; i<strlen(s);i++)
        if(*(s+i) == ',')
            count++;
    return count;
}

int count_spaces(char *s){

    int count =0;
    for(int i=0; i<strlen(s);i++)
        if(*(s+i) == ' ')
            count++;
    return count;
}




void split_by_comas(char*special_splitted_by_comas[20],char* special){

    char *temp = (char*) malloc(MAX);
    strcpy(temp,special);
    int i = 0;

    *(special_splitted_by_comas+i) = strtok(temp,",");

    while(*(special_splitted_by_comas+i) != NULL){

        i++;
        *(special_splitted_by_comas+i) = strtok(NULL,",");


}



}

void split_by_slashes(char*special_splitted_by_slashes[20],char* special){

    char *temp = (char*) malloc(MAX);
    strcpy(temp,special);
    int i = 0;

        *(special_splitted_by_slashes+i) = strtok(temp,"/");

    while(*(special_splitted_by_slashes+i) != NULL){

        i++;
        *(special_splitted_by_slashes+i) = strtok(NULL,"/");


}


}



void make_directory(char **args){

int i=1;
char *path = (char*) malloc(MAX);
int pFound = 0, vFound = 0, mFound = 0;
char*splitted[20];
char* mode = (char*) malloc(10);
char* special = (char*) malloc(MAX);
char*special_splitted_by_comas[20];
char*special_splitted_by_slashes[20];
mode_t default_mode = 0775;
int count =0,count2 = 0;
char* filename = (char*) malloc(MAX);

while(*(args+i) != NULL){

    if(!strcmp(*(args+i),"-m"))
        {
        mFound = 1;
        i++;
        strcpy(mode,*(args+i));

        char** end;
        default_mode = strtol(mode,end,8);
        }
    else
    if(!strcmp(*(args+i),"-p"))
        pFound = 1;
    else
    if(!strcmp(*(args+i),"-v"))
        vFound = 1;
    else strcpy(filename,*(args+i));

    i++;
    }

    //! Count number of "/"
    for(i=0 ; i<strlen(filename) ; i++){

        if (*(filename+i) == '/')
            count++;

        if(*(filename+i) == '{')

            {
                count2 = i;
                break;
            }

    }

    for(i=0; i<count; i++)
        *(splitted+i) = (char*) malloc(MAX);

    strcpy(special,split_path(splitted,filename,count2));
    strcpy(special,special+1);
    *(special+strlen(special)-1)='\0';

    int copy = count;
    if(count2 == 0)
        copy++;

//    for(i = 0; i<copy;i++)
//        printf("%s\n",*(splitted+i));
//
       //printf("Special : %s\n",special);






    getcwd(path,MAX);
    strcat(path,"/");


    if(count == 0){

        strcat(path,filename);
        mkdir(path,default_mode);
        if(vFound == 1)
            printf("mkdir: created directory '%s'\n",filename);


    }



    else if(count != 0 && pFound == 0){


        if(!strcmp(special,"")){

            for(i=0 ; i<copy; i++){
            strcat(path,*(splitted+i));
            strcat(path,"/");

            if(i == copy-1){

                mkdir(path,default_mode);
                if(vFound==1){
                    printf("mkdir: created directory '%s'\n",filename);
            }}

            if(isFile(path)==2)
                {
                printf("mkdir: cannot create directory '%s': No such file or directory\n",filename);
                break;
                }



            }
            }

        else{
        //!Build path
        for(i=0 ; i<copy; i++){
            strcat(path,*(splitted+i));
            strcat(path,"/");

            if(isFile(path)==2)
                {
                printf("mkdir: cannot create directory '%s': No such file or directory\n",filename);
                break;
                }


            }

            char *temp_path = (char*) malloc (MAX);

            int aux = count_comas(special);
            for(i=0; i<aux; i++){
                *(special_splitted_by_comas+i)= (char*) malloc(MAX);

            //! Desparte acoladele dupa virgula si daca nu exista slashuri la vreun caz fa fisierul cu fct asta.
            split_by_comas(special_splitted_by_comas,special);




//            for(i=0;i<aux+1;i++)
//                printf("Siruri: %s\n",*(special_splitted_by_comas+i));

            //! Desparte acoladele despartie, dupa slash si fa fisierul

            for(i=0;i<aux+1;i++){

                strcpy(temp_path,path);

                if(strlen(*(special_splitted_by_comas+i))==1){
                        strcat(temp_path,*(special_splitted_by_comas+i));
                        strcat(temp_path,"/");
                        mkdir(temp_path,default_mode);

                        if(vFound==1){
                        printf("mkdir: created directory '%s'\n",*(special_splitted_by_comas+i));

                        }

                }

                int aux2 = count_slashes(*(special_splitted_by_comas+i));
                for(int j=0; j<aux2; j++){
                    *(special_splitted_by_slashes) = (char*) malloc(MAX);

                split_by_slashes(special_splitted_by_slashes,*(special_splitted_by_comas+i));

//                    for(int j=0; j<aux2+1; j++){
//                        printf("Siruri: %s\n",*(special_splitted_by_slashes+j));}

               for(int j=0; j<aux2+1;j++){

                    strcat(temp_path,*(special_splitted_by_slashes+j));
                    strcat(temp_path,"/");

                   // printf("PATH: %s\n", temp_path);

                     if(j == aux2){

                    mkdir(temp_path,default_mode);

                    if(vFound==1){
                    printf("mkdir: created directory '%s'\n",*(special_splitted_by_slashes+j));

                    }}

                    if(isFile(temp_path)==2)

                        {
                            printf("mkdir: cannot create directory '%s': No such file or directory\n",filename);
                            break;
                        }

               }



                }

            }
            }




        }//else


        } // if flags

        else if(count!=0 && pFound == 1){


        if(!strcmp(special,"")){

            for(i=0 ; i<copy; i++){
            strcat(path,*(splitted+i));
            strcat(path,"/");

            mkdir(path,default_mode);

            if(vFound==1){
                    printf("mkdir: created directory '%s'\n",*(splitted+i));
                }

                }
            }

        else{

        for(i=0 ; i<copy; i++){

             strcat(path,*(splitted+i));
             strcat(path,"/");

             mkdir(path,default_mode);

            if(vFound==1){
                    printf("mkdir: created directory '%s'\n",*(splitted+i));
                }

                }

            char *temp_path = (char*) malloc (MAX);

            int aux = count_comas(special);
            for(i=0; i<aux; i++){
                *(special_splitted_by_comas+i)= (char*) malloc(MAX);

            //! Desparte acoladele dupa virgula si daca nu exista slashuri la vreun caz fa fisierul cu fct asta.
            split_by_comas(special_splitted_by_comas,special);




//            for(i=0;i<aux+1;i++)
//                printf("Siruri: %s\n",*(special_splitted_by_comas+i));

            //! Desparte acoladele despartie, dupa slash si fa fisierul

            for(i=0;i<aux+1;i++){

                strcpy(temp_path,path);

                if(strlen(*(special_splitted_by_comas+i))==1){
                        strcat(temp_path,*(special_splitted_by_comas+i));
                        strcat(temp_path,"/");
                        mkdir(temp_path,default_mode);

                        if(vFound==1){
                        printf("mkdir: created directory '%s'\n",*(special_splitted_by_comas+i));

                        }

                }

                int aux2 = count_slashes(*(special_splitted_by_comas+i));
                for(int j=0; j<aux2; j++){
                    *(special_splitted_by_slashes) = (char*) malloc(MAX);

                split_by_slashes(special_splitted_by_slashes,*(special_splitted_by_comas+i));

//                    for(int j=0; j<aux2+1; j++){
//                        printf("Siruri: %s\n",*(special_splitted_by_slashes+j));}

               for(int j=0; j<aux2+1;j++){

                    strcat(temp_path,*(special_splitted_by_slashes+j));
                    strcat(temp_path,"/");

                   // printf("PATH: %s\n", temp_path);

                    mkdir(temp_path,default_mode);

                    if(vFound==1){
                    printf("mkdir: created directory '%s'\n",*(special_splitted_by_slashes+j));

                    }


               }



                }

            }
            }




        }//else





        } //else flags

        exit(0);


        } //functie



void basename_function(char**args){

    int i=1;
    char* filename = (char*) malloc (MAX);
    char* path = (char*) malloc (MAX);
    char* sufix = (char*) malloc(MAX);
    char*pch;

    if(*(args+2)!=NULL)
        strcpy(sufix,*(args+2));
    else
        strcpy(sufix,"");

    strcpy(filename,*(args+1));

    getcwd(path,MAX);
    strcat(path,filename);
    strcat(path,"/");

    if(strcmp(sufix,""))
    {
    strcpy(path,basename(path));
    pch = strstr(path,sufix);
    *pch = '\0';

    printf("%s\n",path);
    }


    else
    printf("%s\n",basename(path));
    exit(0);



}


int spawn_process(int in, int out, char** args){

      pid_t pid;

      if((pid == fork())==0){

            if(in!=0)
                {
                    dup2(in,0);
                    close(in);
                }

            if(out!=1)
                {
                    dup2(out,1);
                    close(out);
                }

            return execvp(*args,args);


      }

      return pid;



}

int check_for_pipes(char**args){

    int counter =0;
    int i = 0;

    while(*(args+i)!=NULL)

    {
    if(!strcmp(*(args+i),"|"))
        counter ++;
    i++;
    }
return counter;
}

int check_for_redirection(char**args){

    int counter =0;
    int i = 0;

    while(*(args+i)!=NULL)

    {
    if(!strcmp(*(args+i),">") || !strcmp(*(args+i),"<"))
        counter ++;
        i++;
    }
return counter;
}


void split_by_pipes(char *dups[], char**args){

    int i = 0 ;
    int j = 0;

    while(*(args+i)!=NULL){

        if(strcmp(*(args+i),"|"))
            {
            strcat(dups[j],*(args+i));
            strcat(dups[j]," ");
            }
        else
            j++;

        i++;
    }



}

char split_by_redirection(char* dups[],char**args){

    int i = 0 ;
    int j = 0;
    char c;

    while(*(args+i)!=NULL){

        if(strcmp(*(args+i),"<") && strcmp(*(args+i),">"))
            {
            strcat(dups[j],*(args+i));
            strcat(dups[j]," ");
            }
        else
            {
            j++;
            c = **(args+i);
            }

        i++;
    }
return c;
}

void execute_with_pipes(char** command){

    char * temp[256];
    char * pipe_com[256];
    int i;
    pid_t pid;
    int in, fd[2];

    for(i = 0 ;i < check_for_pipes(command)+1; i++)
        {
        pipe_com[i] = (char*) malloc(MAX);
        strcpy(pipe_com[i],"");
        }

    split_by_pipes(pipe_com,command);


//    for(i=0;i<count_spaces(command);i++){
//
//        printf("TEMP: %s\n",temp[i]);
//
//    }

//    for(i=0;i<check_for_pipes(command)+1;i++){
//        printf("PIPE_COM: %s\n",pipe_com[i]);
//
//      }
    in = STDIN_FILENO;
    for(i=0; i < check_for_pipes(command); i++){

        if(pipe(fd))
            perror("Error with pipe");

        else{

            pid = fork();

        if(pid == -1)
            perror("Error with fork");

        else if(pid==0){  //!Child

            close(fd[0]);
            split_line(temp,pipe_com[i]);

        //if(in!=STDIN_FILENO)

        if(dup2(in,STDIN_FILENO)!= -1)
            close(in);

        //if(fd[1]!=STDOUT_FILENO)

        if(dup2(fd[1],STDOUT_FILENO)!= -1)
            close(fd[1]);


        if(!strcmp(temp[0],"basename"))
        {
        basename_function(temp);
        }
        else if(!strcmp(temp[0],"mkdir"))
        {
        make_directory(temp);
        }
        else if(!strcmp(temp[0],"rm"))
        {
        rm(temp);
        }
        else
        execvp(temp[0],temp);

        }

        else //!parent
        {

        //assert(pid>0);
        close(fd[1]);
        close(in);
        in = fd[0]; //! de aici citeste urmatoarea
        //wait(0);
        }
        }

    }


    split_line(temp,pipe_com[i]);

    if(in!=STDIN_FILENO)
    if(dup2(in,STDIN_FILENO)!= -1)
            close(in);


    execvp(temp[0],temp);

}

void remove_strings(int pos1, int pos2, char**args){

    int i = pos1;

    while(i!=pos2){

        args[i] = NULL;
        pos1--;
    }

}

int get_size(char **args){

int i = 0;

while(*(args+i)!=NULL)
    {

        i++;
    }
return i;

}

int count_redirections(char *string){

    int count = 0;
    for(int i=0 ; i<strlen(string); i++){

        if(*(string+i) == '<' || *(string+i)=='>')
        count++;

    }


    return count;
}


void execute_with_pipes_and_redirection(char **command){



    char * temp[256];
    char * pipe_com[256];
    char * redirect[256];
    char * redirect_splitted[256];
    int i;
    char c;
    pid_t pid;
    int in, fd[2];
    int size;
    mode_t mode = 0600;
    int is_pipes = 0;

    for(i = 0 ;i < check_for_pipes(command)+1; i++)
        {
        pipe_com[i] = (char*) malloc(MAX);
        strcpy(pipe_com[i],"");
        }

    split_by_pipes(pipe_com,command);


//    for(i=0;i<count_spaces(command);i++){
//
//        printf("TEMP: %s\n",temp[i]);
//
//    }

//    for(i=0;i<check_for_pipes(command)+1;i++){
//        printf("PIPE_COM: %s\n",pipe_com[i]);
//
//      }
    in = STDIN_FILENO;
    for(i=0; i < check_for_pipes(command); i++){

        if(pipe(fd))
            perror("Error with pipe");

        else{

            pid = fork();

        if(pid == -1)
            perror("Error with fork");

        else if(pid==0){  //!Child

            close(fd[0]);
            split_line(temp,pipe_com[i]);

        //if(in!=STDIN_FILENO)

            if(count_redirections(pipe_com[i]) > 0){

                size = get_size(temp);

                dup2(in,STDIN_FILENO);
                for(int i = 0 ; i < size; i++){

                redirect[i] = (char*) malloc(MAX);
                strcpy(redirect[i],"");
                }

                c = split_by_redirection(redirect,temp);

                //! redirect[0] = comanda
                //! redirect[1] = filename

                 for(int i=0; i < count_spaces(redirect[0])+1;i++){

                        redirect_splitted[i] = (char*) malloc (MAX);

                }

                split_line(redirect_splitted,redirect[0]);

                //! CAND II OUT ">"
                if(c == '>'){
                int descriptor = creat(redirect[1],mode);

                dup2(descriptor,STDOUT_FILENO);
                close(descriptor);

                if(!strcmp(redirect_splitted[0],"mkdir"))
                    make_directory(redirect_splitted);
                else
                if(!strcmp(redirect_splitted[0],"basename"))
                    basename_function(redirect_splitted);
                else
                if(!strcmp(redirect_splitted[0],"rm"))
                    rm(redirect_splitted);
                else
                if(execvp(*redirect_splitted,redirect_splitted)==-1)
                    perror("Invalid command");


                is_pipes = 1;
                }

                //! CAND II IN "<"
                else{

                int descriptor = open(redirect[1],mode);

                if(dup2(fd[1],STDOUT_FILENO)!= -1)
                close(fd[1]);
                dup2(descriptor,STDIN_FILENO);
                close(descriptor);



                if(execvp(*redirect_splitted,redirect_splitted)== -1)
                    perror("Invalid command");
                }


            }else{

                if(dup2(in,STDIN_FILENO)!= -1)
                close(in);

                if(dup2(fd[1],STDOUT_FILENO)!= -1)
                close(fd[1]);

                if(!strcmp(temp[0],"mkdir"))
                    make_directory(temp);
                else
                if(!strcmp(temp[0],"basename"))
                    basename_function(temp);
                else
                if(!strcmp(temp[0],"rm"))
                    rm(temp);
                else
                execvp(temp[0],temp);
            }

        }

        else //!parent
        {

        //assert(pid>0);
        close(fd[1]);
        close(in);

        if(is_pipes == 1){

            in = STDIN_FILENO;
            is_pipes = 0;
        }else
            in = fd[0]; //! de aici citeste urmatoarea

        }
    }
    }

    ///baga ultima comanda

    split_line(temp,pipe_com[i]);

        //if(in!=STDIN_FILENO)

            if(count_redirections(pipe_com[i]) > 0){

                size = get_size(temp);

                dup2(in,STDIN_FILENO);
                for(int i = 0 ; i < size; i++){

                redirect[i] = (char*) malloc(MAX);
                strcpy(redirect[i],"");
                }

                c = split_by_redirection(redirect,temp);


                //! redirect[0] = comanda
                //! redirect[1] = filename

                 for(int i=0; i < count_spaces(redirect[0])+1;i++){

                        redirect_splitted[i] = (char*) malloc (MAX);

                }

                split_line(redirect_splitted,redirect[0]);

                //! CAND II OUT ">"
                if(c == '>'){
                int descriptor = creat(redirect[1],mode);

                dup2(descriptor,STDOUT_FILENO);
                close(descriptor);


                if(!strcmp(redirect_splitted[0],"mkdir"))
                    make_directory(redirect_splitted);
                else
                if(!strcmp(redirect_splitted[0],"basename"))
                    basename_function(redirect_splitted);
                else
                if(!strcmp(redirect_splitted[0],"rm"))
                    rm(redirect_splitted);
                else
                if(execvp(*redirect_splitted,redirect_splitted)==-1)
                    perror("Invalid command");
                is_pipes = 1;
                }

                //! CAND II IN "<"
                else{

                int descriptor = open(redirect[1],mode);

                if(dup2(fd[1],STDOUT_FILENO)!= -1)
                close(fd[1]);
                dup2(descriptor,STDIN_FILENO);
                close(descriptor);

                if(execvp(*redirect_splitted,redirect_splitted)== -1)
                    perror("Invalid command");
                }


            }else{

                if(dup2(in,STDIN_FILENO)!= -1)
                close(in);

                if(dup2(fd[1],STDOUT_FILENO)!= -1)
                close(fd[1]);

                if(!strcmp(temp[0],"mkdir"))
                    make_directory(temp);
                else
                if(!strcmp(temp[0],"basename"))
                    basename_function(temp);
                else
                if(!strcmp(temp[0],"rm"))
                    rm(temp);
                else
                execvp(temp[0],temp);
            }

}

void execute_with_redirection(char **command){

    pid_t pid;
    int fd0,fd1,i,in=0,out=0;
    char input[64],output[64];
    char *temp[256];
    char *buf[256];
    char* filename = (char*) malloc (MAX);
    int size = get_size(command);
    int pos;
    char *path;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
   // printf("SIZE: %d\n",size);
    char c;


    for(int i = 0 ; i < size; i++){

        temp[i] = (char*) malloc(MAX);
        strcpy(temp[i],"");
    }

    c = split_by_redirection(temp,command);
    //printf("CHAR: %c \n",c);

    //! filename temp[1];
    //! command temp[0];

//        for(int i = 0 ; i < 2; i++){
//
//            printf("TEMP: %s\n",temp[i]);
//    }


    for(int i=0; i < count_spaces(temp[0])+1;i++){

        buf[i] = (char*) malloc (MAX);

    }

    split_line(buf,temp[0]);

    pid = fork();

    if (pid<0){
        perror("fork");
    }
    else if(pid == 0) //! child
    {

        if(c == '<'){

            fd0 = open(temp[1],O_RDONLY);
            dup2(fd0,STDIN_FILENO);
            close(fd0);

        if(!strcmp(buf[0],"mkdir"))
            make_directory(buf);
        else
        if(!strcmp(buf[0],"basename"))
            basename_function(buf);
        else
        if(!strcmp(buf[0],"rm"))
            rm(buf);
        else
        if(execvp(buf[0],buf)==-1)
            perror("Invalid command");

        }

        else

        if(c == '>'){
            fd1 = creat(temp[1],mode);
            dup2(fd1,STDOUT_FILENO);
            close(fd1);


        if(!strcmp(buf[0],"mkdir"))
            make_directory(buf);
        else

        if(!strcmp(buf[0],"basename"))
            basename_function(buf);

        else
        if(!strcmp(buf[0],"rm"))
            rm(buf);

        else
        if(execvp(buf[0],buf)==-1)
            perror("Invalid command");

        }

    }
    else
        wait(0);






}

int main()
{
    //printf("MAIN BEGIN\n");
    char **args;
    char *line;
    char **duplicates_split[20];
    char *duplicates[20];
    int i;
    args = (char**) malloc(500);
    line = (char*) malloc(500);
    pid_t p;

   while(1){

    printf("$ ");
    read_splitted_line(args,&line);

    if(strlen(line)==0)
        continue;
    if(!strcmp(*args,"exit")){

        printf("Program finished \n");

        return 0;
        }

    if(!strcmp(*args,"cd")){

            cd(*(args+1));
            continue;
        }

    p = fork();

    if( p < 0 )

   {

    printf("fork error");
    return 1;

   }

    if(p == 0) /// child

   {

        if(!strcmp(*args,"rm")){
            rm(args);
            exit(0);

        }

        else
        if(check_for_pipes(args)!=0 && check_for_redirection(args) == 0)
            {
             execute_with_pipes(args);
             exit(0);
             }
        else
        if(check_for_redirection(args)!=0 && check_for_pipes(args) == 0)
            {
             execute_with_redirection(args);
             exit(0);
             }
        else
        if(check_for_pipes(args)!=0 && check_for_redirection(args)!=0)
            {
            execute_with_pipes_and_redirection(args);
            exit(0);
             }
        else
        if(!strcmp(*args,"mkdir")){

            make_directory(args);
            exit(0);

        }

        else
        if(!strcmp(*args,"basename")){

            basename_function(args);
            exit(0);

        }

        else
        if(execvp(*args,args) == -1){
            perror(" exec ");
        }
   }

   else ///parent
   {

        if(wait(0) == -1){

            perror("wait");
        }

   }

   }


    return 0;
}
