/*
*
*	Author	: Arpit Bhayani
*	E-mail 	: arpit.bhayani@students.iiit.ac.in
*	Roll no.: 201305515
*
*/

/*
*	Objective:
*		To  understand the file system  services  provided by OS.
*	Question:
*		Write a program to simulate GNU  “ls”  command. Options to be implemented:
*			l : Long listing  format
*			R : Recursive
*			a : All
*			d : List  directory entries instead of contents
*			S : Sort by file size
*			t : Sort by modification time
*	Usage:
*		$ ./myls ­lS 
*		$ ./myls ­l ­ta
*		$ ./myls  lt
*		$ ./myls  R  ­lt
*		$ ./myls  l <file/directory>
*		$ ./myls  l ­t <file/directory>
*		$ ./myls  lt > output.txt
*	Output format:
*		Output  should  be  similar  to  the  output  provided  by  “ls” 
*		command. The  output is  in multiple columns  (normal behavior) but 
*		when  the  standard  output  is  not  a  terminal,  the  contents  are 
*		printed one per line.
*		It must resolve symbolic  links  and provide details of where the 
*		link  points with long listing option (­l).
*	
*/

#include "lsheader.h"

int main(int argc , char * argv[]) {

	char c = 0;
	char ** arg = NULL;
	int count_arg = 0;

	options_t * opt = (options_t *) calloc(1 , sizeof(options_t));

	while ( (c = getopt(argc, argv, "aSdltR")) != -1) {
		
		switch (c) {
			case 'a':
				opt->flag_a = 1;
		     		break;
			case 'S':
				opt->flag_S = 1;
				opt->flag_t = 0;
		     		break;
			case 'd':
				opt->flag_d = 1;
		     		break;
			case 'l':
				opt->flag_l = 1;
				break;
			case 't':
				opt->flag_t = 1;
				opt->flag_S = 0;
		     		break;
			case 'R':
				opt->flag_R = 1;
		     		break;
			case '?':
			    	break;
			default:
			    	printf ("?? getopt returned character code 0%o ??\n", c);
		}
    	}
    	if (optind < argc) {
		arg = (char **) calloc ( (argc - optind) , sizeof(char *) );
		count_arg = 0;
		while (optind < argc) {
			arg[count_arg++] = argv[optind++];
		}
    	}
	
	ls(opt , arg , count_arg);

	if( arg != NULL )
		free( arg );
	free(opt);

	return 0;

}
