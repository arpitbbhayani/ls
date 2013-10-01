#include "lsheader.h"

#define SIZE_N 1024
#define SIZE_P 16
#define SIZE_T 16

char * 	getperm	(char * , struct stat filestat);
int    	getlinks(struct stat filestat);
char * 	getuser	(struct stat filestat);
char *	getgroup(struct stat filestat);
int	getsize	(struct stat filestat);
char *	getdate	(char * , struct stat filestat);
char *	getname	(char * , char * , char , int);

int ls( options_t * opt , char ** arg , int count_arg ) {

	int i = 0;
	int count_arg_f = 0 , count_arg_d = 0;
	char ** arg_d = NULL , ** arg_f = NULL;

	struct stat fileStat;

	columns_t * col = (columns_t *) calloc(1 , sizeof(columns_t));

	arg_d = (char **) calloc ( count_arg , sizeof(char *) );
	arg_f = (char **) calloc ( count_arg , sizeof(char *) );

	if( opt -> flag_d == 0 && count_arg == 0 ) {

		_ls(opt , "." , col);

		free(col);
		free(arg_d);
		free(arg_f);
		return 0;
	}
	else if ( opt -> flag_d == 1 && count_arg == 0 ) {
		free(arg_d);
		
		arg_d = (char **) calloc ( 1 , sizeof(char *) );
		arg_d[0] = (char *) calloc ( SIZE_P , sizeof(char) );
		strcpy(arg_d[0] , ".");

		___ls(opt , arg_d , 1 , col);

		free(col);
		free(arg_d[0]);
		free(arg_d);
		free(arg_f);
		return 0;
	}

	for ( i = 0 ; i < count_arg ; i++ ) {
		reset_col(col);
		if ( lstat( arg[i] , &fileStat ) ) {
			perror("ls: cannot access ");
			break;
		}

		if( S_ISDIR(fileStat.st_mode) ) {
			arg_d[count_arg_d++] = arg[i];
		}
		else {
			arg_f[count_arg_f++] = arg[i];
		}

	}

	if ( opt -> flag_d == 1 ) {
		___ls(opt , arg , count_arg , col);
	}
	else {
		if( count_arg_f != 0 ) {
			___ls(opt , arg_f , count_arg_f  , col);
		}

		for ( i = 0 ; i < count_arg_d ; i++ ) {
			printf("%s:\n" , arg_d[i]);
			_ls(opt , arg_d[i] , col);
		}
	}

	free(col);
	free(arg_d);
	free(arg_f);
	return 0;
}
 
int _ls( options_t * opt , char * path , columns_t * col ) {

	struct dirent **namelist = NULL;
	struct fileinfo_t *fileinfo = NULL;
	struct stat fileStat;
	char * cwd = NULL;
	char ** list_d = NULL;
	int n = 0 , i = 0 , count_d = 0 ;
	int total = 0;

	if( opt->flag_a == 0 )
		n = scandir(path, &namelist, filter , compare );
	else
		n = scandir(path, &namelist, NULL , compare );

	list_d = (char **) calloc ( n , sizeof(char *) );

	cwd   = (char *) calloc(2048 , sizeof(char) );

	if ( getcwd ( cwd , 2048 ) == NULL)
		perror("getcwd() : ");

	if( chdir(path) );
		//perror("Error");

	if (n < 0)
		perror("ls: cannot open directory ");
	else {
		fileinfo = (fileinfo_t *) calloc(n , sizeof(fileinfo_t));
		for (i = 0 ; i < n ; i++) {
			if( lstat( namelist[i]->d_name , &fileStat ) < 0) {
				perror(namelist[i] -> d_name);
				return -1;
			}

			fileinfo[i].perm = (char *) calloc( SIZE_P , sizeof(char) );
			fileinfo[i].time = (char *) calloc( SIZE_T , sizeof(char));
			fileinfo[i].name = (char *) calloc( SIZE_N , sizeof(char));

			fileinfo[i].perm   = getperm ( fileinfo[i].perm , fileStat );
			fileinfo[i].name   = getname ( fileinfo[i].name , namelist[i]->d_name , fileinfo[i].perm[0] , opt->flag_l);
			fileinfo[i].n_link = getlinks( fileStat );
			fileinfo[i].user   = getuser ( fileStat );
			fileinfo[i].group  = getgroup( fileStat );
			fileinfo[i].f_size = getsize ( fileStat );
			fileinfo[i].time   = getdate ( fileinfo[i].time , fileStat );
			fileinfo[i].tick   = (long long int) fileStat.st_mtime;

			total += fileStat.st_blocks;

			set_c_size( col , fileinfo[i].n_link , fileinfo[i].user , fileinfo[i].group , fileinfo[i].f_size );

			if ( fileinfo[i].perm[0] == 'd' ) {
				list_d[count_d] = (char *) malloc( SIZE_N );
				strcpy( list_d[count_d] , fileinfo[i].name );
				count_d ++;
			}

		}
		printdata(fileinfo , n , opt , col , total/2);

		if( opt -> flag_R == 1 ) {
			
			for( i = 0 ; i < count_d ; i++ ) {

				if( strcmp("." , list_d[i]) && strcmp(".." , list_d[i]) ) {

					char ** temp_d = (char **) calloc ( 1 , sizeof(char *) );
					temp_d[0] = (char *) malloc ( SIZE_P * sizeof( char *) );
					strcpy(temp_d[0] , list_d[i]);
	
					ls( opt , temp_d , 1 );
	
					free(temp_d[0]);
					free(temp_d);
				}
			}
		}

	}

	
	if( chdir(cwd) )
		perror("Error");
	
	for (i = 0 ; i < n ; i++) {
		free(namelist[i]);
		free(fileinfo[i].perm);
		free(fileinfo[i].time);
		free(fileinfo[i].name);
	}
	for (i = 0 ; i < count_d ; i++) {
		free(list_d[i]);
	}
	free(namelist);
	free(fileinfo);
	free(list_d);	
	free(cwd);
	return 0;
}

int ___ls( options_t * opt , char ** files , int count_arg , columns_t * col) {

	struct fileinfo_t *fileinfo = NULL;
	struct stat fileStat;
	char * cwd = NULL;
	int i = 0;

	reset_col( col );

	fileinfo = (fileinfo_t *) calloc(count_arg , sizeof(fileinfo_t));
	for (i = 0 ; i < count_arg ; i++) {
		if(lstat( files[i] , &fileStat )) {
			perror( files[i] );
			return -1;
		}

		fileinfo[i].perm = (char *) calloc( SIZE_P , sizeof(char) );
		fileinfo[i].time = (char *) calloc( SIZE_T , sizeof(char));
		fileinfo[i].name = (char *) calloc( SIZE_N , sizeof(char));

		fileinfo[i].perm   = getperm ( fileinfo[i].perm , fileStat );
		fileinfo[i].name   = getname ( fileinfo[i].name , files[i] , fileinfo[i].perm[0] , opt->flag_l );
		fileinfo[i].n_link = getlinks( fileStat );
		fileinfo[i].user   = getuser ( fileStat );
		fileinfo[i].group  = getgroup( fileStat );
		fileinfo[i].f_size = getsize ( fileStat );
		fileinfo[i].time   = getdate ( fileinfo[i].time , fileStat );
		fileinfo[i].tick   = (long long int) fileStat.st_mtime;

		set_c_size( col , fileinfo[i].n_link , fileinfo[i].user , fileinfo[i].group , fileinfo[i].f_size );

	}
	printdata(fileinfo , count_arg , opt , col , -1);

	
	for (i = 0 ; i < count_arg ; i++) {
		free(fileinfo[i].perm);
		free(fileinfo[i].time);
		free(fileinfo[i].name);
	}
	free(fileinfo);	
	free(cwd);
	return 0;
}

int __ls( options_t * opt , char * path , columns_t * col) {

	struct dirent **namelist = NULL;
	struct fileinfo_t *fileinfo = NULL;
	struct stat fileStat;
	int i = 0;

	fileinfo = (fileinfo_t *) calloc(1 , sizeof(fileinfo_t));
	if(lstat( path , &fileStat )) {
		perror(namelist[i] -> d_name);
		return -1;
	}
	fileinfo[i].perm = (char *) calloc( SIZE_P , sizeof(char) );
	fileinfo[i].time = (char *) calloc( SIZE_T , sizeof(char));
	fileinfo[i].name = (char *) calloc( SIZE_N , sizeof(char));

	fileinfo[i].perm   = getperm ( fileinfo[i].perm , fileStat );
	fileinfo[i].name   = getname ( fileinfo[i].name , path , fileinfo[i].perm[0] , opt->flag_l);
	fileinfo[i].n_link = getlinks( fileStat );
	fileinfo[i].user   = getuser ( fileStat );
	fileinfo[i].group  = getgroup( fileStat );
	fileinfo[i].f_size = getsize ( fileStat );
	fileinfo[i].time   = getdate ( fileinfo[i].time , fileStat );
	fileinfo[i].tick   = (long long int) fileStat.st_mtime;

	printdata(fileinfo , 1 , opt , col , -1);
	
	free(fileinfo[0].perm);
	free(fileinfo[0].time);
	free(fileinfo[0].name);
	free(fileinfo);	
	return 0;
}

char * getdate(char *final_time , struct stat fileStat) {
	int i , j , count;
	struct tm * info = localtime(&fileStat.st_mtime);
	char * time = asctime(info);
	i = j = count = 0;
	while(time[i++] != ' ');
	while(count != 2) {
		if(time[i] == ':')
			count++;
		if(count == 2) {
			break;
		}
		final_time[j++] = time[i++];
	}
	final_time[j] = '\0';	
	
	return final_time;
}

int getlinks(struct stat fileStat) {
	return fileStat.st_nlink;
}

int getsize(struct stat fileStat) {
	return fileStat.st_size;
}

char * getuser(struct stat fileStat) {

	struct passwd *pass = getpwuid(fileStat.st_uid);
	return pass->pw_name;
}

char * getgroup(struct stat fileStat) {

	struct group *pass = getgrgid(fileStat.st_gid);
	return pass->gr_name;
}

char * getname(char * name , char * n , char is_link , int flag_l) {

	char * temp = (char *) malloc ( SIZE_N * sizeof(char) );

	if( flag_l == 0 || is_link != 'l' ) {
		strcpy(name , n);
	}
	else {
		int r = readlink( n , temp , SIZE_N );
		if (r < 0) {
			perror("lstat");
			free(temp);
			strcpy(name , n);
			return name;
		}
		temp[r] = '\0';
		sprintf(name , "%s -> %s", n , temp);
	}
	free(temp);
	return name;
}

char * getperm(char * perm , struct stat fileStat) {


	if ( S_ISLNK(fileStat.st_mode) ) {
		perm[0] = 'l';
	}
	else if ( S_ISDIR(fileStat.st_mode) ) {
		perm[0] = 'd';
	}
	else if ( S_ISCHR(fileStat.st_mode) ) {
		perm[0] = 'c';
	}
	else if ( S_ISSOCK(fileStat.st_mode) ) {
		perm[0] = 's';
	}
	else if ( S_ISFIFO(fileStat.st_mode) ) {
		perm[0] = 'p';
	}
	else if ( S_ISBLK(fileStat.st_mode) ) {
		perm[0] = 'b';
	}
	else {
		perm[0] = '-';
	}
	perm[1] = ((fileStat.st_mode & S_IRUSR) ? 'r' : '-');
	perm[2] = ((fileStat.st_mode & S_IWUSR) ? 'w' : '-');
	perm[3] = ((fileStat.st_mode & S_IXUSR) ? 'x' : '-');
	perm[4] = ((fileStat.st_mode & S_IRGRP) ? 'r' : '-');
	perm[5] = ((fileStat.st_mode & S_IWGRP) ? 'w' : '-');
	perm[6] = ((fileStat.st_mode & S_IXGRP) ? 'x' : '-');
	perm[7] = ((fileStat.st_mode & S_IROTH) ? 'r' : '-');
	perm[8] = ((fileStat.st_mode & S_IWOTH) ? 'w' : '-');
	perm[9] = ((fileStat.st_mode & S_IXOTH) ? 'x' : '-');

	if ( fileStat.st_mode & S_ISUID ) {
		perm[3] = 's';
	}
	else if ( fileStat.st_mode & S_IXUSR ) {
		perm[3] = 'x';
	}
	else {
		perm[3] = '-';
	}

	if ( fileStat.st_mode & S_ISGID ) {
		perm[6] = 's';
	}
	else if ( fileStat.st_mode & S_IXGRP ) {
		perm[6] = 'x';
	}
	else {
		perm[6] = '-';
	}

	if ( fileStat.st_mode & S_ISVTX ) {
		perm[9] = 't';
	}
	else if ( fileStat.st_mode & S_IXOTH ) {
		perm[9] = 'x';
	}
	else {
		perm[9] = '-';
	}

	perm[10] = 0;

	return perm;
}

