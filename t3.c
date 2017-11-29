#include "Sorter.h"
#include <ctype.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>


pthread_mutex_t running_mutex;
int numoffiles = 0;
pthread_t * threads;
int index_threads = 0;
static char * outdir_global;
static char * type_global ;

CSVFile * all_files;

void mergeStr(CSVRow* arr,CSVRow* help, int lptr,int rptr,int llimit,int rlimit,int num)
{
    int i=lptr,j=llimit,k=0;
    while(i<=rptr && j<=rlimit) 
    {
	if(strcmp(arr[i].data,arr[j].data)==0)
	{
		if(arr[i].point<arr[j].point)
		{
        	    strcpy(help[k].data,arr[i].data);
        	    help[k].point=arr[i].point;
        	    strcpy(help[k].string_row,arr[i].string_row);
        	    k++;
        	    i++;
		}
        	else
		{
        	    strcpy(help[k].data,arr[j].data);
        	    help[k].point=arr[j].point;
        	    strcpy(help[k].string_row,arr[j].string_row);
        	    k++;
        	    j++;
		}
	}
        else if(strcmp(arr[i].data,arr[j].data)<0)
	{
            strcpy(help[k].data,arr[i].data);
            help[k].point=arr[i].point;
            strcpy(help[k].string_row,arr[i].string_row);
            k++;
            i++;
	}
        else
	{
            strcpy(help[k].data,arr[j].data);
            help[k].point=arr[j].point;
            strcpy(help[k].string_row,arr[j].string_row);
            k++;
            j++;
	}
    }

    while(i<=rptr)
    {
            strcpy(help[k].data,arr[i].data);
            help[k].point=arr[i].point;
            strcpy(help[k].string_row,arr[i].string_row);
            k++;
            i++;
    }
    while(j<=rlimit) 
    {
            strcpy(help[k].data,arr[j].data);
            help[k].point=arr[j].point;
            strcpy(help[k].string_row,arr[j].string_row);
            k++;
            j++;
    }
    for(i=lptr,j=0;i<=rlimit;i++,j++)
    {
	        strcpy(arr[i].data,help[j].data);
		arr[i].point=help[j].point;
		strcpy(arr[i].string_row,help[j].string_row);
    }
}

void sortStr(CSVRow* a,CSVRow *b, int i,int j,int num)
{
    int mid;
    if(i<j)
    {
        mid=(i+j)/2;
        sortStr(a,b,i,mid,num); 
        sortStr(a,b,mid+1,j,num);
        mergeStr(a,b, i,mid,mid+1,j,num); 
    }
}

void mergeInt(CSVRow* arr,CSVRow* help, int lptr,int rptr,int llimit,int rlimit,int num)
{
    int i=lptr,j=llimit,k=0;
    while(i<=rptr && j<=rlimit) 
    {
	if(strtof(arr[i].data,NULL)==strtof(arr[j].data,NULL))
	{
		if(arr[i].point<arr[j].point)
		{
        	    strcpy(help[k].data,arr[i].data);
        	    help[k].point=arr[i].point;
        	    strcpy(help[k].string_row,arr[i].string_row);
        	    k++;
        	    i++;
		}
        	else
		{
        	    strcpy(help[k].data,arr[j].data);
        	    help[k].point=arr[j].point;
        	    strcpy(help[k].string_row,arr[j].string_row);
        	    k++;
        	    j++;
		}
	}
        else if(strtof(arr[i].data,NULL)<strtof(arr[j].data,NULL))
	{
			strcpy(help[k].data,arr[i].data);
            help[k].point=arr[i].point;
            strcpy(help[k].string_row,arr[i].string_row);
            k++;
            i++;
	}
        else
	{
            strcpy(help[k].data,arr[j].data);
            help[k].point=arr[j].point;
            strcpy(help[k].string_row,arr[j].string_row);
            k++;
            j++;
	}
    }
    while(i<=rptr) 
    {
            strcpy(help[k].data,arr[i].data);
            help[k].point=arr[i].point;
            strcpy(help[k].string_row,arr[i].string_row);
            k++;
            i++;
    }
    while(j<=rlimit) 
    {
            strcpy(help[k].data,arr[j].data);
            help[k].point=arr[j].point;
            strcpy(help[k].string_row,arr[j].string_row);
            k++;
            j++;
    }
    for(i=lptr,j=0;i<=rlimit;i++,j++)
    {
        strcpy(arr[i].data,help[j].data);
		arr[i].point=help[j].point;
		strcpy(arr[i].string_row,help[j].string_row);
    }
}

void sortInt(CSVRow* a,CSVRow* b, int i,int j,int num)
{
    int mid;
    if(i<j)
    {
        mid=(i+j)/2;
        sortInt(a,b, i,mid,num); 
        sortInt(a,b, mid+1,j,num); 
        mergeInt(a,b, i,mid,mid+1,j,num); 
    }
}

void callMe(int size,char type,CSVRow* arr, CSVRow* b)
{
	if(type=='i')
	{
		sortInt(arr,b, 1,size-1,size);
	}
	else
	{
		sortStr(arr,b, 1,size-1,size);
	}
	return;
}

void trim(char* str)
{
	char * t = malloc(strlen(str));
	int i =0;
	int j=0;
	for(i=0;i<strlen(str);i++)
	{
		if(isspace(str[i])==0)
		{
			t[j]=str[i];
			j++;
		}
	}
	t[j]='\0';
	strcpy(str,t);
	free(t);
}


void sortCSVFile(char * filename1,char * token1, char * outdir1){
	
	FILE * fp = fopen(filename1, "r");
	int file_count = 0;
	char c = 0;
	int i = 0;
	char * str_file = malloc(10);
	int row_position = 0;
	int j;
	//fprintf(stdout, "%s\n", token);		
	//printf("a1\n");

	c = fgetc(fp);
	while (c != EOF) {
		//printf("%c\n",c);
		str_file = realloc(str_file, (i+1) * sizeof(char));	
		str_file[i] = c;
		if(c == '\n'){
			file_count++;
		}
		i++;
		c = fgetc(fp);
    }
        
	fclose(fp);
	
	str_file[i] = '\0';
	
	
	CSVRow *movies = malloc(file_count * sizeof(CSVRow));
	//token = strtok(str_file, "\n");
	
	for(j = 0; j < file_count; j++){
		movies[j].data = malloc(10000);
		movies[j].point = j;
		movies[j].string_row = malloc(10000);
	}

	CSVRow* help=malloc(sizeof(CSVRow)*file_count*2);    //array used for merging
    for(j =0;j<file_count;j++)
    {
	help[j].data=malloc(10000);
	help[j].point=j;
	help[j].string_row=malloc(10000);
    }

	//CSVRow *tempy = malloc(file_count * sizeof(CSVRow));
	//token = strtok(str_file, "\n");
	
	//for(int j = 0; j < file_count; j++){
	//	tempy[j].data = malloc(1000);
	//	tempy[j].point = j;
	//	tempy[j].string_row = malloc(1000);
	//}

	//printf("a2\n");
	
	int temp = 0;
	int count = 0;
	int index = 0;
	int comma_position_max = 0;
	int p1 = 0;
	int p2 = 0;
	int char_found = 0;
	int comma_number = 0;
	char * check_token = malloc(1000);
	c = 0;

	for(j = 0; j < i; j++){
		if(str_file[j] == '\n'){
			//printf("a\n");
			strncpy(movies[count].string_row, str_file+temp,j-temp+1);
			movies[count].string_row[j-temp+1] = '\0';
			if (count == 0){
				c = movies[count].string_row[index];
				for(index = 0; index<strlen(movies[count].string_row) ; index++){
					//fprintf(stdout, "%c\n", c);
					c = movies[count].string_row[index];
					if(c == ',' || movies[count].string_row[index+1] == '\n'){
						if( movies[count].string_row[index+1] == '\n'){
							index++;
						}
						comma_position_max++;
						if(index == p1 || index == p1+1){
							check_token = "\0";
						}
						else{
							strncpy(check_token, movies[count].string_row+p1,index-p1);
							check_token[index-p1] =  '\0';
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
						}
						if(strcmp(check_token,token1) == 0){
							char_found = 1;
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
							break;
						}
						p1 = index+1;
						if( movies[count].string_row[index+1] == '\n'){
							index--;
						}

					}
					//printf("%d\n %c",char_found, c);
				}
				if(char_found == 0){
					fprintf(stderr, "ERROR: <Selected item was not found in parameters>\n");
					
					free(check_token);
					free(movies);
					free(token1);
					free(str_file);
			
					return;
				}
				//fprintf(stdout,"%d : %d\n",char_found , comma_position_max);
				//fprintf(stdout, "[%s] : [%s] \n",token, check_token);
				strcpy(movies[count].data, check_token);
				movies[count].data[strlen(check_token)+1] = '\0';
			}
			else{
				//fprintf(stdout, "%d \n ", count);
				comma_number = 0;
				index = 0;
				p1 = 0;
				c = movies[count].string_row[index];
				for(index = 0; index<strlen(movies[count].string_row); index++){
					//fprintf(stdout, "%c\n", c);
					c = movies[count].string_row[index];
					if(c == ',' && index+1 != strlen(movies[count].string_row) && movies[count].string_row[index+1] == '"'){
							
						comma_number++;
						if((index == p1) && (comma_number == comma_position_max)){
							//movies[count].data = "0\0";
							break;
						}
						else if(comma_number == comma_position_max){
							strncpy(movies[count].data, movies[count].string_row+p1,index-p1);
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
							//movies[count].data[index-p1] = '\0';
							//fprintf(stdout, "%d: %s\n",count, movies[count].data);	
							trim(movies[count].data);
							break;
						}
						p1 = index+1;
						
						index = index+2;
						int x;
						for(x = 0; c != '"'; index++){
							c = movies[count].string_row[index];
						}	
						
						c = movies[count].string_row[index];
						//fprintf(stdout,"%c\n" , c);
						comma_number++;
						if((index == p1) && (comma_number == comma_position_max)){
							//movies[count].data = "0\0";
							break;
						}
						else if(comma_number == comma_position_max){
							strncpy(movies[count].data, movies[count].string_row+p1,index-p1);
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
							//movies[count].data[index-p1] = '\0';
							//fprintf(stdout, "%d: %s\n",count, movies[count].data);	
							trim(movies[count].data);
								break;
						}
						p1 = index+1;
						index++;
						c = movies[count].string_row[index];
	
					}
					if(c == ',' || movies[count].string_row[index+1] == '\n'){

						if( movies[count].string_row[index+1] == '\n'){
							index++;
						}

						comma_number++;
						if((index == p1) && (comma_number == comma_position_max)){
							//movies[count].data = "NULL";
							break;
						}
						else if(comma_number == comma_position_max){
							strncpy(movies[count].data, movies[count].string_row+p1,index-p1);
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
							//movies[count].data[index-p1] = '\0';
							//fprintf(stdout, "%d: %s\n",count, movies[count].data);	
							trim(movies[count].data);
							break;
						}
						p1 = index+1;
						if( movies[count].string_row[index+1] == '\n'){
							index--;
						}

					}
				}
			}
			
			temp = j+1;
			count++;
		}
	}
	
	//printf("a3\n");
	
	char type = 'i';
	int k;
	for(j = 1; j < file_count; j++){
		for(k = 0; movies[j].data[k] != '\0'; k++){
			if(!(isdigit(movies[j].data[k]))){
				if(movies[j].data[k] != '.' || movies[j].data[k] != '-'){
					type = 's';	
				}
			}
		}
	}
	//printf("%d \n", type);
	//mergesort(movies,1,file_count-1,file_count);
	callMe(file_count,type,movies,help);
	//printf("heyo\n");
	//printf("\n");
	//char * filename = malloc(1000);
	
	//char * tempp = malloc(1000);
	
	//printf("a4\n");
	
	char * out_filename = malloc(100);

	sprintf(out_filename, "%s/AllFiles-sorted-%s.csv", outdir1, token1);

	pthread_mutex_lock(&running_mutex);

	FILE * pFile = fopen (out_filename,"a");
	
	if (pFile!=NULL){
		//fprintf(pFile,"\n%s",test_string);
		
		fprintf(pFile, "\n");
		for(j = 1; j < file_count; j++){
			fprintf(pFile, "%s", movies[j].string_row);
		}
		fprintf(pFile, "\b");
	

	}
	
	pthread_mutex_unlock(&running_mutex);

	free(out_filename);
	fclose(pFile);
	
	

	//printf("a6\n");
	for(j = 0; j < file_count; j++){
		free(movies[j].data);
		free(help[j].data);
		//movies[j].point = j;
		free(movies[j].string_row);
		free(help[j].string_row);
	}

	//printf("a7\n");
	free(check_token);
	free(movies);
	free(help);
	free(token1);
	free(outdir1);
	free(str_file);
	
	return;
} 


struct arg_struct {  //Struct to allow for multiple arguments in threads

	char * file_path;
	char * out_dir;
	char * sort_type;

} args;

int isCSV(const char* name) { //Check if the file is a csv
	char* temp=strdup(name);
	char* ext=strrchr(temp,'.');
	if(ext!=NULL&&strcmp(ext,".csv")==0)
	{
		free(temp);
		return 1;
	}
	free(temp);
	return 0;
} 


void file_test(char * filename, char * out_dir, char * sort_type) { //Test mutex locks using file output

	
	sortCSVFile(filename ,sort_type , out_dir);

	
	/*
	char * out_filename = malloc(100);
	
	sprintf(out_filename, "%s/AllFiles-sorted-%s.csv", out_dir, sort_type);
	
	FILE * pFile = fopen(filename, "r");
	
	char * test_string = malloc(101);
	if(pFile != NULL) {
		fgets(test_string, 100, pFile);
		fgets(test_string, 100, pFile);
	}

	

	fclose(pFile);

	pthread_mutex_lock(&running_mutex);

	pFile = fopen (out_filename,"a");
	
	if (pFile!=NULL){
		//fprintf(pFile,"\n%s",test_string);
		fclose (pFile);
	}
	
	pthread_mutex_unlock(&running_mutex);
*/	
	//free(test_string);
	//free(out_filename);
	//free(filename);
	//free(out_dir);
	//free(sort_type);
}


void * display_info2(void * arguments) { //Test out if thread works by printing out the csv file name

	struct arg_struct *args = (struct arg_struct *)arguments;

	if( isCSV(args->file_path)){
		fprintf(stdout, "%s \n" , args->file_path);
		file_test(args->file_path, args->out_dir, args->sort_type);
	}

}


int display_info_threaded(const char *fpath, const struct stat *sb, int tflag) { //Function to be run by nftw

	struct arg_struct * args2 = malloc( sizeof(args2));
	args2->file_path = strdup(fpath);
	args2->out_dir = strdup(outdir_global);
	args2->sort_type = strdup(type_global);

	pthread_create(&threads[index_threads++], NULL, &display_info2,(void *) args2);
	return 0;
}


int count_files(const char *fpath, const struct stat *sb, int tflag) { //Check how many files there are to malloc

	++numoffiles;
	return 0;
}

	
int main(int argc, char *argv[]) {
	
	
	pthread_mutex_init(&running_mutex, NULL);
	char * in_dir = malloc(1000);
	outdir_global = malloc(1000);
	type_global  = malloc(1000);
	
	strcpy(in_dir, "./\0");
	strcpy(outdir_global, "./\0");
	

	if(argc != 3 && argc != 5 && argc != 7){
		fprintf(stderr, "<ERROR> : Incorrect number of arguments, for more information , please use  $ ./sorter -h \n");
		return 0;
	}

	short type_found = 0;

	int i;
	for(i = 1; i < argc; i++){
		
		if(strcmp(argv[i],"-c") == 0){
			if((i % 2) == 0){
				fprintf(stderr, "<ERROR> : Incorrect format, for more information , please use  $ ./sorter -h \n");
				return 0;
			}
			strcpy(type_global, argv[i+1]);
			type_found = 1;
		}

		if(strcmp(argv[i], "-d") == 0){
			if((i % 2) == 0){
				fprintf(stderr, "<ERROR> : Incorrect format, for more information , please use  $ ./sorter -h \n");
				return 0;
			}

			strcpy(in_dir, argv[i+1]);
		}

		if(strcmp(argv[i], "-o") == 0){
			if((i % 2) == 0){
				fprintf(stderr, "<ERROR> : Incorrect format, for more information , please use  $ ./sorter -h \n");
				return 0;
			}

			strcpy(outdir_global, argv[i+1]);
		}
	}
	
	char * out_filename = malloc(100);
	
	sprintf(out_filename, "%s/AllFiles-sorted-%s.csv", outdir_global, type_global);

	FILE * pFile;
	pFile = fopen (out_filename,"w");
	
	if (pFile!=NULL){
		fprintf(pFile, "color,director_name,num_critic_for_reviews,duration,director_facebook_likes,actor_3_facebook_likes,actor_2_name,actor_1_facebook_likes,gross,genres,actor_1_name,movie_title,num_voted_users,cast_total_facebook_likes,actor_3_name,facenumber_in_poster,plot_keywords,movie_imdb_link,num_user_for_reviews,language,country,content_rating,budget,title_year,actor_2_facebook_likes,imdb_score,aspect_ratio,movie_facebook_likes" );
		fclose (pFile);
	}

	free(out_filename);
	
	FILE *file;
	file = fopen(in_dir, "r");

	if (file == NULL){
		fprintf(stderr, "<ERROR> : Input directory not found or can't be accessed. \n");
		free(type_global);
		free(outdir_global);
		free(in_dir);
		return 0;
	}
	
	fclose(file);
	file = fopen(outdir_global, "r");

	if (file == NULL){
		fprintf(stderr, "<ERROR> : Output directory not found or can't be accessed. \n");
		free(type_global);
		free(outdir_global);
		free(in_dir);
		return 0;	
	}

	fclose(file);
					

	if (type_found == 0){
		fprintf(stderr, "<ERROR> : Incorrect format expected a -c flag: \n For more information , please use  $ ./sorter -h \n");
		free(type_global);
		free(outdir_global);
		free(in_dir);
		return 0;
	}
	
	int x = ftw(in_dir, count_files, 0);
    
	numoffiles++;
	threads = malloc(sizeof(pthread_t) * numoffiles);
	all_files = malloc(sizeof(CSVFile) * numoffiles);

	if(threads == NULL){
		fprintf(stderr,"<ERROR> : Too many expected threads, out of memory");
	}

	printf("type : %s \nin_dir : %s \noutdir : %s \n\n", type_global, in_dir, outdir_global);	//testing print

   	if (ftw(in_dir, display_info_threaded, 0) == -1) {
        fprintf(stderr, "<ERROR> : Error occured during the file tree walk");
		free(type_global);
		free(outdir_global);
		free(in_dir);
		exit(EXIT_FAILURE);
    }
  		
	for( index_threads = 0; index_threads <  numoffiles; index_threads++) {	
		pthread_join(threads[index_threads], NULL);	
	}
	
	pthread_mutex_destroy(&running_mutex);

  	printf("\n"); //extra new line for space 
	
	free(type_global);
	free(outdir_global);
	free(in_dir);
	exit(EXIT_SUCCESS);
}