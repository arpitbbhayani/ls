/*
*	Author	: Arpit Bhayani
*	E-mail 	: arpit.bhayani@students.iiit.ac.in
*	Roll no.: 201305515
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/ioctl.h>

/*
*	Enum type_t is datatype enum which is used for getting the format specifying string
*	ex : for string it will return "%xs" where x is the number like 10 so function will return %10s
*/
typedef enum {
	INT , LONG_INT , STRING
} type_t;

/*
*	The columns_t structure is used for column adjustment is ls -l option.
*	The values in this strucure will be the maximum width of the corresponding columns.
*	This data structure can ue used to print column in well indented way.
*/
typedef struct columns_t {
	int nlink_c;
	int user_c;
	int group_c;
	int size_c;
} columns_t;

/*
*	The options_t structure is used to store various options passed.
*	For each option passed, the corresponding flag is set.
*/
typedef struct options_t {
	int flag_l;
	int flag_R;
	int flag_a;
	int flag_d;
	int flag_S;
	int flag_t;
} options_t;

/*
*	The fileinfo_t structure is used for storing the detailed info about the file
*	It stores file size , clock ticks , number of hard links , permissions , user , group ,
*	time and name of the file.
*/
typedef struct fileinfo_t {
	long int f_size;
	long long int tick;
	int 	n_link;
	char * 	perm;
	char * 	user;
	char * 	group;
	char * 	time;
	char * 	name;
} fileinfo_t;

/*
*
*/
int ls   ( options_t * , char ** , int );
int _ls  ( options_t * , char * , columns_t * );
int __ls ( options_t * , char * , columns_t *);
int ___ls( options_t * , char ** , int , columns_t * );

int (filter) (const struct dirent *);
int (compare)(const struct dirent ** , const struct dirent ** );
int (compar)(const void *, const void *);
int (compa)(const void *, const void *);

void printdata(struct fileinfo_t * , int n , options_t * , columns_t * , int);

void reset_col( columns_t *  );
void set_c_size( columns_t * , int , char * , char * , long int);
