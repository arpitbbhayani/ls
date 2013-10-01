#include "lsheader.h"

int (compar)(const void * a , const void * b ) {

	long int c = ((fileinfo_t *)a)->f_size;
	long int d = ((fileinfo_t *)b)->f_size;

	return d - c;
}

int (compa)(const void * a , const void * b ) {

	long long int c = ((fileinfo_t *)a)->tick;
	long long int d = ((fileinfo_t *)b)->tick;
	return d - c;

}

void set_c_size( columns_t * col , int nlink , char * user , char * group , long int size) {
	int len;

	len = 0;
	while ( nlink != 0 ) {
		len ++;
		nlink /= 10;
	}

	col->nlink_c = (len > col->nlink_c) ? len : col -> nlink_c ;

	len = 0;
	while ( size != 0 ) {
		len ++;
		size /= 10;
	}
	col->size_c = (len > col->size_c) ? len : col -> size_c;

	len = 0;
	while ( user[len++] );
	col->user_c = (len > col->user_c) ? len - 1 : col -> user_c;

	len = 0;
	while ( group[len++] );
	col->group_c = (len > col->group_c) ? len - 1 : col -> group_c;

	return;

}

int (filter)(const struct dirent * direntry) {
	return (*direntry).d_name[0] != '.';
}

int (compare)(const struct dirent ** e1, const struct dirent ** e2) {
	char * s1 = (char*) (**e1).d_name;
	char * s2 = (char*) (**e2).d_name;
	char * string1 = (char *) calloc(strlen(s1) + 1 , sizeof(char));
	char * string2 = (char *) calloc(strlen(s2) + 1, sizeof(char));

	char * oldS1 = string1 , *oldS2 = string2;

	int result = 0;

	while(*s1) {
		if( *s1 == '.' ) {
			s1++;
			continue;
		}
		if( *s1 >= 'A' && *s1 <= 'Z')
			*string1 = *s1 + ('a'-'A');
		else
			*string1=*s1;
		s1++;
		string1++;
	}

	while(*s2) {
		if( *s2 == '.' ) {
			s2++;
			continue;
		}
		if( *s2 >= 'A' && *s2 <= 'Z')
			*string2 = *s2 + ('a'-'A');
		else
			*string2 = *s2;
		if( *string2 == '.' )
			*string2 = 'a';
		s2++;
		string2++;
	}
	(*string1) = (*string2) = 0;
	result = strcmp(oldS1 , oldS2);

	free(oldS1);
	free(oldS2);
	
	return result;

}

void reset_col ( columns_t * col ) {
	col -> nlink_c = 0;
	col -> user_c = 0;
	col -> group_c = 0;
	col -> size_c = 0;
}
