#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>


#define MAX_PATH_LEN 257

bool success = false;

struct section_header {
    char sect_name[18];
    char sect_type;
    int sect_offset;
    int sect_size;
};


void list(char *dirName)
{
	DIR* dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];

    dir = opendir(dirName);
	if (dir == 0) {
		printf("ERROR\ninvalid directory path");
		exit(4);
	    }   
    else
    {
       printf("SUCCESS\n");
    }
    
	while ((dirEntry=readdir(dir)) != 0) {
        if(strcmp(dirEntry->d_name,".") == 0 || strcmp(dirEntry->d_name,"..") == 0 )
            continue;
        sprintf(name, "%s/%s", dirName, dirEntry->d_name);
		
		lstat (name, &inode);

	    printf("%s\n", name);
	    }
    
	closedir(dir);
}

void recursive_list(char *dir_name)
{
    DIR *dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[257];
    dir = opendir(dir_name);
    if(dir == 0)
    {
        printf("ERROR\ninvalid directory path");
        exit(4);
    } 
    if(success == false)
    {
        success = true;
        printf("SUCCESS\n");
    }

    while((dirEntry = readdir(dir)) != 0)
    {   
        sprintf(name, "%s/%s", dir_name, dirEntry->d_name);
    
        lstat (name, &inode);

        if (S_ISDIR(inode.st_mode))
        {
            if(strcmp(dirEntry->d_name,".") != 0 && strcmp(dirEntry->d_name,"..") != 0)
            {
                sprintf(name, "%s/%s", dir_name, dirEntry->d_name);
    
                lstat (name, &inode);

                printf("%s\n", name);
                success = true;
                recursive_list(name);
            }
        }
        if (S_ISREG(inode.st_mode))
        {
            printf("%s\n", name);
        }
    }
    closedir(dir);
}

void name_starts_with(char* dirName, char* prefix)
{
	DIR* dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];

    dir = opendir(dirName);
	if (dir == 0) {
		printf("ERROR\ninvalid directory path");
		exit(4);
	    }   
    else
    {
       printf("SUCCESS\n");
    }
    
	while ((dirEntry=readdir(dir)) != 0)
    {
        if(strcmp(dirEntry->d_name,".") == 0 || strcmp(dirEntry->d_name,"..") == 0 )
            continue;
        sprintf(name, "%s/%s", dirName, dirEntry->d_name);
		
		lstat (name, &inode);
        if (S_ISDIR(inode.st_mode) || S_ISREG(inode.st_mode))
            if(strncmp(dirEntry->d_name, prefix, strlen(prefix)) == 0)
	            printf("%s\n", name);
	}
    
	closedir(dir);
}

void check_permission(char* dirName)
{
	DIR* dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[MAX_PATH_LEN];

    dir = opendir(dirName);
	if (dir == 0) {
		printf("ERROR\ninvalid directory path");
		exit(4);
	    }   
    else
    {
       printf("SUCCESS\n");
    }
    
	while ((dirEntry=readdir(dir)) != 0)
    {
        if(strcmp(dirEntry->d_name,".") == 0 || strcmp(dirEntry->d_name,"..") == 0 )
            continue;
        sprintf(name, "%s/%s", dirName, dirEntry->d_name);
		
		lstat (name, &inode);
        if (S_ISDIR(inode.st_mode) || S_ISREG(inode.st_mode))
            if (inode.st_mode & S_IWUSR)
	            printf("%s\n", name);
	}
    
	closedir(dir);
}

int valid(char magic, int version, int nr_sections)
{   
    if(magic != 'x'){
        printf("ERROR\n wrong magic");
        return 0;
    }
    if(version < 105 || version > 198){
        printf("ERROR\n wrong version");
        return 0;
    }
    if(nr_sections < 5 || nr_sections > 16){
        printf("ERROR\n wrong sect_nr");
        return 0;
    }
    return 1;
}

void parse(char* dir_name)
{   
    int header_size = 0, version = 0;
    short int nr_sections = 0;
    char magic;

    int fd = open(dir_name, O_RDONLY);
    if(fd < 0)
        //error message
        printf("file cannot open");
    
    read(fd, &magic, 1);
    read(fd, &header_size, 2);
    read(fd, &version, 4);
    read(fd, &nr_sections, 1);

    //struct section_header section[nr_sections];
    struct section_header *section;
    section= malloc(nr_sections * sizeof(struct section_header)); 
    if(valid(magic,version,nr_sections) == 1)
    {
        for(int i = 0; i < nr_sections; i++)
        {
            //section1: <NAME_1> <TYPE_1> <SIZE_1>
            read(fd, section[i].sect_name, 17);
            read(fd, &section[i].sect_type, 1);
            read(fd, &section[i].sect_offset, 4);
            read(fd, &section[i].sect_size, 4);
            section[i].sect_name[17] = '\0'; //each string must be finished with a '\0'

            if(section[i].sect_type != 46 && section[i].sect_type != 27){
                printf("ERROR\nwrong sect_types");
                free(section);
                exit(0);
            }
        }
        printf("SUCCESS\n");
        printf("version=%d\n", version);
        printf("nr_sections=%d\n", nr_sections);
        for(int i = 0; i < nr_sections; i++)
            printf("section%d: %s %d %d\n", i+1, section[i].sect_name, section[i].sect_type, section[i].sect_size); 
    }
    free(section);
}

int valid_find_all(char *dir_name){
    int header_size = 0, version = 0;
    short int nr_sections = 0;
    char magic;

    int fd = open(dir_name, O_RDONLY);
    if(fd < 0)
        //error message
        printf("file cannot open");
    
    read(fd, &magic, 1);
    read(fd, &header_size, 2);
    read(fd, &version, 4);
    read(fd, &nr_sections, 1);

    //struct section_header section[nr_sections];
    struct section_header *section;
    section= malloc(nr_sections * sizeof(struct section_header)); 
    
    if((magic == 'x') && (version >= 105 && version <= 198) && (nr_sections >= 5 && nr_sections <= 16))
    {    
        for(int i = 0; i < nr_sections; i++)
        {
            //section1: <NAME_1> <TYPE_1> <SIZE_1>
            read(fd, section[i].sect_name, 17);
            read(fd, &section[i].sect_type, 1);
            read(fd, &section[i].sect_offset, 4);
            read(fd, &section[i].sect_size, 4);
            section[i].sect_name[17] = '\0'; //each string must be finished with a '\0'

            if(section[i].sect_type != 46 && section[i].sect_type != 27){
                free(section);
                return 0;
            }
            if(section[i].sect_size > 1284){
                free(section);
                return 0;
            }
        }
    free(section);
    return 1;
    }
    free(section);
    return 0;
}

void find_all(char *dir_name)
{
    DIR *dir;
    struct dirent *dirEntry;
    struct stat inode;
    char name[257];

    dir = opendir(dir_name);
    if(dir == 0)
    {
        printf("ERROR\ninvalid directory path");
        exit(4);
    } 
    if(success == false)
    {
        success = true;
        printf("SUCCESS\n");
    }

    while((dirEntry = readdir(dir)) != 0)
    {
        sprintf(name, "%s/%s", dir_name, dirEntry->d_name);
    
        lstat (name, &inode);
        if(strcmp(dirEntry->d_name,".") != 0 && strcmp(dirEntry->d_name,"..") != 0)
        {
            if (S_ISDIR(inode.st_mode))
            {
                success = true;
                find_all(name);
            }
            if(valid_find_all(name) == 1)
            {
                sprintf(name, "%s/%s", dir_name, dirEntry->d_name);

                lstat (name, &inode);

                if (S_ISREG(inode.st_mode))
                    printf("%s\n", name);
            }
        }

    }
    closedir(dir);
}

void reverse(char *str, int t) 
{   
    char* final = (char*) malloc(t * sizeof(char));
    int n = 0, size = 0;
    if(str[strlen(str)-1] == '\n')
        size = strlen(str)-2;
    else
        size = strlen(str)-1;
    
    for (int i=size; i>=0; i--) 
     {   *(final+n) = str[i];
        n++;}
    *(final+n)='\0';
    printf("%s", final);
    free(final);
}
 
void extract(char *path, int section, int line)
{   
    bool sect = false;
    bool ln = false;
    int nr_line = 0;

    int header_size = 0, version = 0;
    short int nr_sections = 0;
    char magic;

    int fd = open(path, O_RDONLY);
    if(fd < 0)
        //error message
        printf("file cannot open");
    printf("SUCCESS\n");
    read(fd, &magic, 1);
    read(fd, &header_size, 2);
    read(fd, &version, 4);
    read(fd, &nr_sections, 1);

    //struct section_header sectionn[nr_sections];
    struct section_header *sectionn;
    sectionn = (struct section_header*)malloc(nr_sections * sizeof(struct section_header)); 
    
    if((magic == 'x') && (version >= 105 && version <= 198) && (nr_sections >= 5 && nr_sections <= 16))
    {    
        for(int i = 0; i < nr_sections; i++)
        {   
            if(section - 1 == i)
                sect = true;

            read(fd, sectionn[i].sect_name, 17);
            read(fd, &sectionn[i].sect_type, 1);
            read(fd, &sectionn[i].sect_offset, 4);
            read(fd, &sectionn[i].sect_size, 4);
            sectionn[i].sect_name[17] = '\0'; //each string must be finished with a '\0'

            if(sectionn[i].sect_type != 46 && sectionn[i].sect_type != 27)
                exit(0);
        }
        int t = 0; 
        int size = sectionn[section - 1].sect_size + 1;
        char *phrase = (char*)malloc(size * sizeof(char));
        char *word = (char*)malloc(size * sizeof(char));
        lseek(fd, sectionn[section - 1].sect_offset, SEEK_SET);
        read(fd, phrase, sectionn[section - 1].sect_size);
        int c = sectionn[section - 1].sect_size;
        for(int i = 0; i < c; i++)
        {   
            *(word+t) = *(phrase+i);
            t++;
            if((*(phrase+i) == '\n') || (i == (c-1)))
            {   nr_line++;
                if(nr_line == line)
                {
                    ln = true;
                    reverse(word,t);
                }else
                    {t = 0;
                        for(int k = 0; k < i; k++)
                        *(word+k) = 0;
                    }
            }
            
        }
        free(sectionn);
        if(sect == false)
            printf("ERROR\nsection");
        if(ln == false)
            printf("ERROR\nline");
        free(phrase);
        free(word);
        
    }
    free(sectionn);
    close(fd);
}

int main(int argc, char **argv){
    
    char* command = "";
    char path[200];
    char prefix[10] = "";
    int section,line;
    if(argc >= 2)
    {
        if(strcmp(argv[1], "variant") == 0)
        {
            printf("26063\n");
        }else{
            bool isRec = false;
            bool isPrefix = false;
            bool checkPermission = false;

            for(int i = 0; i < argc; i++){
                if(strncmp(argv[i], "path=", strlen("path=")) == 0)
                {   
                    char* str = &argv[i][5];
                    strcpy(path, str);
                }
                if(strcmp(argv[i], "list") == 0)
                    command = "list";
                if(strcmp(argv[i], "recursive") == 0)
                    isRec = true;
                if(strncmp(argv[i], "name_starts_with=", strlen("name_starts_with=")) == 0)
                {
                    isPrefix = true;
                    char* str = &argv[i][17];
                    strcpy(prefix, str);
                }
                if(strcmp(argv[i], "has_perm_write") == 0)
                    checkPermission = true;
                if(strcmp(argv[i], "parse") == 0)
                    command = "parse";
                if(strcmp(argv[i], "findall") == 0)
                    command = "findall";
                if(strcmp(argv[i], "extract") == 0)
                    command = "extract";
                if(strncmp(argv[i], "section=", strlen("section=")) == 0)
                {   
                    char* str = &argv[i][8];
                    section = atoi(str);
                } 
                if(strncmp(argv[i], "line=", strlen("line=")) == 0)
                {   
                    char* str = &argv[i][5];
                    line = atoi(str);
                }   
                
            }
            if(strcmp(command, "list") == 0)
            {   
                if(isRec == true)
                    recursive_list(path);
                else
                    { 
                        if(isPrefix == true)
                            name_starts_with(path, prefix);
                        else
                            { 
                                if(checkPermission == true)
                                    check_permission(path);
                                else
                                    list(path);
                            }
                    }   
            }
            if(strcmp(command, "parse") == 0)
                parse(path);
            if(strcmp(command, "findall") == 0)
                find_all(path);
            if(strcmp(command, "extract") == 0)
                extract(path, section, line);
        }   
    }
    return 0;
}