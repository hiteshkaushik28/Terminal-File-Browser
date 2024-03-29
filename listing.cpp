/*                               
    AUTHOR: HITESH KAUSHIK 
    ROLL NO: 2018201057 
    COPYRIGHT (C) HITESH KAUSHIK PG-1
*/

/*ls module file */

#include "listing.h"
#include "keypress.h"

vector<string> tokens;
vector<string> files;
stack<string> enter;
stack<string> lft;
stack<string> rght;
struct winsize w;
static string temp;
static char abst[PATH_MAX];
static int lines;


//clear and reset terminal
void blank()
{
    printf("\033c" );
}

//reset cursor to top left position
void resetcursor()
{
    printf("\033[2;1H" );
}


//get directory information    
int getdir (char *arr)
{
	blank();
	files.clear();
    int count=0;
    
    DIR *dp;
    struct dirent *dirp;

    /* file stat, username and groupname */
    struct stat fileStat; 
    struct passwd *pw;
    struct group  *gr;
    char str[PATH_MAX];

    //strcpy(path,"/bin");
	//try to open specified directory 
    if((dp  = opendir(arr)) == NULL) 
    {
        cout << "Error(" << errno << ") opening " << arr << endl;
        return errno;
    }

    //pushing all file names into vector
    while ((dirp = readdir(dp)) != NULL) 
    {
       count++;
       files.push_back(string(dirp->d_name));       
    }

    //sort file name in lex order
    sort(files.begin(),files.end());
        
    printf("%c[2K", 27);
    printf("\033[30;");
    printf("47m");
    printf("PERMISSIONS\tOWNER\tGROUP\tSIZE\tDATE MODIFIED\tNAME   ");
    printf("\033[0m");
    printf("\n");
    

    vector<string>::iterator itr;
    for(itr=files.begin();itr!=files.end();++itr)  

    {   
       strcpy(str,arr);
       strcat(str,"/");
       strcat(str,(*itr).c_str());
       if (!stat(str, &fileStat)) 
        { 
            //convert uid and gid to uname,gname
            pw=getpwuid(fileStat.st_uid);
            gr=getgrgid(fileStat.st_gid);

            //print permissions
            printf((S_ISDIR(fileStat.st_mode))  ? "d" : "-"),printf((fileStat.st_mode & S_IRUSR) ? "r" : "-");
            
            printf((fileStat.st_mode & S_IWUSR) ? "w" : "-");
            printf((fileStat.st_mode & S_IXUSR) ? "x" : "-");
            printf((fileStat.st_mode & S_IRGRP) ? "r" : "-");
            printf((fileStat.st_mode & S_IWGRP) ? "w" : "-");
            printf((fileStat.st_mode & S_IXGRP) ? "x" : "-");
            printf((fileStat.st_mode & S_IROTH) ? "r" : "-");
            printf((fileStat.st_mode & S_IWOTH) ? "w" : "-");
            printf((fileStat.st_mode & S_IXOTH) ? "x" : "-");

            //output file properties
            cout<<"\t"<<(string(pw->pw_name)).substr(0,7)<<"\t"<<(string(gr->gr_name)).substr(0,7);
            printf("\t%d",(int)fileStat.st_size);
            cout<<"\t"<<string(ctime(&fileStat.st_mtime)).substr(4,12);
            printf("\t%.20s",(string(*itr)).c_str());

        } 

        else
        {
            char c;
            blank();
            perror("Method failed to load stats");
            c=getchar();
        }
      
        printf("\n");
    }

//resetcursor();
return count;
}

int dir_enter(int pos)
{
    char file[PATH_MAX],dump[PATH_MAX];
    getcwd(dump, sizeof(dump));
    struct stat check;
    strcpy(file,dump);
    strcat(file,"/");
    strcat(file,(files[pos]).c_str());
    stat(file,&check);
    pid_t pid;

    if(!(S_ISDIR(check.st_mode)))
    {
        pid=fork();
        if(pid==0)
        {
            execl("/usr/bin/xdg-open","xdg-open",file, (char *)0);
            
        }

        return -1;
    }

    else
    {
      
    if(enter.size() == 1)
    {
        if(files[pos] == ".")
        {
            return -1;
        }
        else if(files[pos] == "..")
        {
            resetcursor();
            return -2;
        }
        else
        {   
            temp = enter.top();
            lft.push(temp);
            temp+="/";
            temp+=files[pos].c_str();
            enter.push(temp);
            strcpy(abst,temp.c_str());
            lines = getdir(abst);
        }

     }

     else
     {
        if(files[pos] == ".")
        {
            return -1;
        }

        else if(files[pos] == "..")
        {
            enter.pop();
            temp = enter.top();
            strcpy(abst,temp.c_str());
            lines = getdir(abst);

        }

        else
        {
            lft.push(temp);
            temp = enter.top();
            temp+="/";
            temp+=files[pos].c_str();
            enter.push(temp);
            strcpy(abst,temp.c_str());
            lines = getdir(abst);
        }

     }
      return lines;
    }
   
}

int left_enter()
{
  rght.push(enter.top());  
  temp = lft.top();
  lft.pop();
  if(enter.size() > 1)
    enter.pop();
  strcpy(abst,temp.c_str());
  lines = getdir(abst);
  return lines; 
}

int right_enter()
{
  lft.push(enter.top());
  enter.push(rght.top());
  temp = rght.top();
  rght.pop();
  strcpy(abst,temp.c_str());
  lines = getdir(abst);
  return lines; 
}

int backspace()
{
    lft.push(enter.top());
    enter.pop();
    temp = enter.top();
    strcpy(abst,temp.c_str());
    lines = getdir(abst);
    return lines; 
}

void command_prompt()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    printf("\033[%d;1H",w.ws_row);
    printf("%c[2K", 27);
    printf("\033[30;");
    printf("47m");
    printf("Command Line: ");
    printf("\033[0m");
    printf(" ");
    fflush(stdin);   
}

int command_mode()
{
   int ret = -1;
   command_prompt(); 
   char input[256],*tok;
    
    int ch,i=0;
    
    while((ch = kbget()) != ESCAPE)
    {
        fflush(stdin);
        if(ch == BACKSPACE)
        {
            if(i>0)
            {
                --i;
                const char delbuf[] = "\b \b";
                write(STDOUT_FILENO, delbuf, strlen(delbuf));
            }
        }


        else if (ch == ENTER)
        {
            fflush(stdin);
            i=0;
            tokens.clear();
            command_prompt();
            tok = strtok(input, " ");
            while(tok != NULL)
            {
                tokens.push_back(tok);
                tok = strtok(NULL," ");
            }

            ret = command_process();
            command_prompt();
        }
        else
        {
            input[i++] = char(ch);
            printf("%c",ch);
        }
    }

    printf("%c[2K", 27);
    return ret;
}

int command_process()
{
    char temp1[1000],temp2[1000],temp3[PATH_MAX];
   
    int l = tokens.size();
    int t = 0;

    if(strcmp((tokens[0]).c_str(),"copy") == 0)
    {
        for(unsigned int i=1;i<(l-1);++i)
        {
            getcwd(temp1, sizeof(temp1));
            strcat(temp1,"/");
            strcat(temp1,(tokens[i].c_str()));

            getcwd(temp2, sizeof(temp2));
            strcat(temp2,"/");
            strcat(temp2,(tokens[l-1].c_str()));
            enter.push(string(temp2));
            strcpy(temp3,temp2);
            strcat(temp2,"/");
            strcat(temp2,(tokens[i].c_str()));     
            cp(temp1,temp2);
            t = getdir(temp3);

        }
    }
    else if (strcmp((tokens[0]).c_str(),"goto") == 0)
    {
        getcwd(temp1, sizeof(temp1));
        strcat(temp1,"/");
        strcat(temp1,(tokens[1].c_str()));
        t = getdir(temp1);
        command_prompt();
    }
    else if (strcmp((tokens[0]).c_str(),"rename") == 0)
    {
        strcpy(temp1,(enter.top()).c_str());
        strcat(temp1,"/");
        strcat(temp1,(tokens[1].c_str()));

        strcpy(temp2,(enter.top()).c_str());
        strcat(temp2,"/");
        strcat(temp2,(tokens[2].c_str()));

        renamefile(temp1,temp2);
        strcpy(temp3,(enter.top()).c_str());
        t = getdir(temp3);
    }

    else if (strcmp((tokens[0]).c_str(),"deletefile") == 0)
    {
        strcpy(temp1,(enter.top()).c_str());
        strcat(temp1,"/");
        strcat(temp1,(tokens[1].c_str()));
        strcat(temp1,"\0");
        removefile(temp1);
        strcpy(temp3,(enter.top()).c_str());
        strcat(temp3,"\0");
        t = getdir(temp3);
    }

    else if (strcmp((tokens[0]).c_str(),"create_dir") == 0)
    {
        strcpy(temp1,(enter.top()).c_str());
        strcat(temp1,"/");
        strcat(temp1,(tokens[1].c_str()));
        strcat(temp1,"\0");
        createdir(temp1,0700);
        strcpy(temp3,(enter.top()).c_str());
        t = getdir(temp3);
    }

    else if (strcmp((tokens[0]).c_str(),"movefile") == 0)
    {
       for(unsigned int i=1;i<(l-1);++i)
        {
            strcpy(temp1,(enter.top()).c_str());   
            strcat(temp1,"/");
            strcat(temp1,(tokens[i].c_str())); 
            strcat(temp1,"\0");

            getcwd(temp2, sizeof(temp2));
            strcat(temp2,"/");
            strcat(temp2,(tokens[l-1].c_str()));
            strcpy(temp3,temp2);
            strcat(temp3,"\0");
            strcat(temp2,"/");
            strcat(temp2,(tokens[i].c_str()));   
            strcat(temp2,"\0");         
            cp(temp1,temp2);
            removefile(temp1);
            t = getdir(temp3);

        }
    }

    else if (strcmp((tokens[0]).c_str(),"createfile") == 0)
    {
        
        strcpy(temp1,(enter.top()).c_str());
        strcpy(temp3,temp1);
        strcat(temp3,"\0");
        strcat(temp1,"/");
        strcat(temp1,(tokens[1].c_str()));
        strcat(temp1,"\0");
        createfile(temp1);
        t = getdir(temp3);
    }

    else
    {
        char c;
        blank();
        cout<<"Invalid command!!! Press any key to continue";
        fflush(stdin);
        c = getchar();
        strcpy(temp1,(enter.top()).c_str());
         t = getdir(temp1);
    }

    return t;
}

/* create file */
void createfile(char first[])
{
    /*file can only be created in present directory to handle permission issues*/
    fopen(first,"w+");
}

/* create directory */
void createdir(char first[],int perm)
{
    mkdir(first,perm);
}

/* remove file */
void removefile(char first[])
{
    remove(first);
}


/* rename file */
void renamefile(char first[],char second[])
{
    rename(first,second);
}


/* copy file */
void cp(char *source,char *destination)
{
 
  int in_fd, out_fd, n_chars;
  char buf[1024];
 
  /* open files */
  if( (in_fd=open(source, O_RDONLY)) == -1 )
  {
    err("Cannot open ", source);
  }
 
 
  if( (out_fd=creat(destination, 0644)) == -1 )
  {
    err("Cannot create ", destination);
  }
 
 
  /* copy files */
  while( (n_chars = read(in_fd, buf, 1024)) > 0 )
  {
    if( write(out_fd, buf, n_chars) != n_chars )
    {
      err("Write error to ", destination);
    }
 
 
    if( n_chars == -1 )
    {
      err("Read error from ", source);
    }
  }
 
     /* close files */
    if( close(in_fd) == -1 || close(out_fd) == -1 )
    {
      err("Error closing files", "");
    }
 
}
 
 
  void err(char *s1, char *s2)
  {
    fprintf(stderr, "Error: %s ", s1);
    perror(s2);
    exit(1);
  }
