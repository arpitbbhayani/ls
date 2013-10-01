#include "lsheader.h"

char * getcolorstring( char * , char * , char , int);
char * getprintstring( char * , type_t , void *);

int findmax(int * , int , int); 

void printdata(struct fileinfo_t * fileinfo , int n , options_t * opt , columns_t * col , int total) {

	int i = 0 , num_col = 0;
	struct winsize w;
	char * print_str = (char *) calloc( 1024 , sizeof(char));
	int * array_i = (int *) calloc( n , sizeof(int) );
	int isTerminal = 0;

	if ( isatty( fileno(stdout) ) ) {
		isTerminal = 1;
	}

	ioctl(0, TIOCGWINSZ, &w);

	if( opt -> flag_S ) {
		qsort(fileinfo , n , sizeof(fileinfo_t) , compar );
	}
	else if( opt -> flag_t ) {
		qsort(fileinfo , n , sizeof(fileinfo_t) , compa );
	}

	if( opt->flag_l == 1 ) {
		if( total != -1 )
			printf("total %d\n" , total);
		for( i = 0 ; i < n ; i ++ ) {

			printf("%10s " , fileinfo[i].perm); 

			print_str = getprintstring( print_str , INT , &col->nlink_c);
			printf(print_str , fileinfo[i].n_link);

			print_str = getprintstring( print_str , STRING , &col->user_c);
			printf(print_str , fileinfo[i].user); 

			print_str = getprintstring( print_str , STRING , &col->group_c);
			printf(print_str , fileinfo[i].group); 

			print_str = getprintstring( print_str , LONG_INT , &col->size_c);
			printf(print_str , fileinfo[i].f_size); 

			printf("%s " , fileinfo[i].time); 

			print_str = getcolorstring( print_str , fileinfo[i].perm , isTerminal , opt->flag_l);
			printf(print_str , 0 , fileinfo[i].name);
			printf("\n");

			if( isTerminal ) printf("\e[0;39m");

		}
		printf("\n");


	}
	else {
		num_col = w.ws_col;

		int n_r = 1 , flag_col = 0 , j = 0 , sum = 0;
		int * length = (int *) malloc( n * sizeof(int) );
		int * max_length = (int *) malloc( n * sizeof(int) );

		for ( i = 0 ; i < n ; i++ ) {
			length[i] = strlen(fileinfo[i].name) + 2;
		}

		while ( flag_col != 1 ) {

			for(i = 0 ; i < n/n_r ; i++ ) {
				max_length[i] = findmax(length , i , n_r);
			}

			sum = 0;
			for( i = 0 ; i < n/n_r ; i++ ) {
				sum += max_length[i];
			}
			sum += n/n_r;
			if( sum > num_col ) {
				n_r ++;
			}
			else {
				flag_col = 1;
			}
		}

		for(i = 0 ; i < n ; i++ ) {
			array_i[i] = -1;
		}

		for( i = 0 ; i <= n_r ; i++ ) {
			for( j = 0 ; j <= n/n_r ; j++ ) {
				if( (j * n_r + i) < n && array_i[j * n_r + i] == -1 ) {
					print_str = getcolorstring( print_str , fileinfo[j * n_r + i].perm , isTerminal , opt->flag_l);
					printf(print_str , -max_length[j] , fileinfo[ j * n_r + i ].name);
					array_i[j*n_r + i] = 0;
				}
			}
			printf("\n");
		}

		free(length);
		free(max_length);

	}
	free(print_str);
	free(array_i);
}

int findmax(int *array , int col , int row ) {

	int i = 0 , max = array[col * row];
	for(i = 0 ; i < row ; i++ ) {
		if( array[col * row + i] > max ) {
			max = array[col*row + i];
		}
	}

	return max;

}
 
char * getprintstring ( char *print_str , type_t type , void * value) {
	switch ( type ) {
		case INT:
			sprintf(print_str , "%%%dd " , *((int*) value) );
			return print_str;
			break;
		case LONG_INT:
			sprintf(print_str , "%%%dld " , *((int*) value) );
			return print_str;
			break;
		case STRING:
			sprintf(print_str , "%%%ds " , *((int*) value) );
			return print_str;
			break;
	}
	return NULL;
}

char * getcolorstring ( char * print_str , char * perm , char isTerminal , int flag_l) {

	if( isTerminal == 0 ) {
		if(flag_l == 1) strcpy(print_str , "%*s");
		else		strcpy(print_str , "%*s\n");
		return print_str;
	}

	if (perm[0] == 'd') {
		sprintf(print_str , "\e[1;34m%%*s\e[0;39m");
	}
	else if (perm[0] == 'l') {
		sprintf(print_str , "\e[1;36m%%*s\e[0;39m");
	}
	else if (perm[0] == 'p') {
		sprintf(print_str , "\e[1;33;40m%%*s\e[0;39m");
	}
	else if( perm[3] == 'x' || perm[6] == 'x' || perm[9] == 'x' ) {
		sprintf(print_str , "\e[1;92m%%*s\e[0;39m");
	}
	else {
		sprintf(print_str , "\e[0;39m%%*s\e[0;39m");
	}
	//printf("For file with permission[0] %c : echoing print_str = %s\n" , perm[0] , print_str);
	return print_str;
}
