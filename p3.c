#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEW(x) (x*)malloc(sizeof(x))

typedef struct station station;
typedef struct line line;
typedef struct root root;
typedef struct path path;
	
path *paths[100] = {NULL};
int test = 0;
int source = 0;
station *store = NULL;
station *multiple_lines[100] = {NULL};
station * transit[10] = {NULL};
station *source_station = NULL;
station *dest_station = NULL;
int path_num = 1;
int dir[2] = {0};
int count = 0;
char names[30][40] ={{'\0'}};
int colors[100] = {0};
int update_count = 0;
int previous_color = 0;
int current_color = 0;
int complete_path[100] = {0};
int name_count = 0;
/**
  * Array access for the lines will be as follows
  * green 0
  * yellow 1
  * blue 2
  * silver 3
  * orange 4
  * red 5
  */
root **line_colors = {NULL};

struct  station{
	int num_lines;
	int line_num;
	char *station_name;
  	line *lines[6];
	int transfer_time[6][6];
}__attribute__((packed));					


struct root{
	char *color;
	int num;
	int line_num;
	station *head;
	station *tail;
}__attribute__((packed)) ;


struct __attribute__((packed)) line{
	char *color;
	int line_num;
	//char *station_name;
	int station_num;
	int time;
	int stop_time;
	int num_lines;
	int *transfers;
	int visited;
	station *prev;
	station *next;
};

struct __attribute__((packed)) path{
	int source[2];
	int dest[2];
	int color_path[300];
	int station_path[300];
	int changes;
	int time;
	int previous_stop[2];
	int found;
};


/** 
  * This function creates a new transit node
  * @param parameters to be stored
  * @return pointer to the transit
  */
line * make_line(char *color, int station_num, int time, int stop_time, int num_lines, int line_num, int *transfer_colors){
	
	line *temp = NULL;
	temp = NEW(line); 
	if(temp != NULL){		
		temp->color = malloc(20);
		strncpy(temp->color , color, strlen(color)+1);
		temp->station_num = station_num;		
		temp->time = time;
		temp->stop_time = stop_time;
		temp->line_num = line_num;
		temp->visited = 0;
		temp->next = NULL;
		temp->prev = NULL;
		temp->num_lines = num_lines;
		if(num_lines != 0){
			temp->transfers = malloc(sizeof(int) * num_lines);
			int i = 0;
			for(i = 0; i < num_lines; i++){
				(temp->transfers[i]) = transfer_colors[i];
			}
		}
	}
	return temp;
}	

/**
  * This functions creates a new node
  * @param value to be stored in the node
  * @return pointer to the node created
*/			
station * make_station(char *color, char *name, int station_num, int time, int stop_time,
		       int num_lines, int line_num, int *transfer_colors, int *times){
	
	station *temp;
	temp = NEW(station);
	if(temp != NULL){
	
		temp->num_lines = num_lines;
		temp->line_num = line_num;
	label1:	temp->station_name = malloc(30);
		if(temp->station_name == NULL)
			goto label1;
		strcpy(temp->station_name,name);
		//temp->lines = malloc(sizeof(line) * (num_lines + 1));

		if(temp->lines[line_num] == NULL)
			(temp->lines[line_num]) = make_line(color,station_num,time,stop_time,
		       		  num_lines,line_num,transfer_colors);

		if(num_lines != 0 && times != NULL){
			int i = 0;
			for(i = 0; i < 6; i++){
				temp->transfer_time[line_num][i] = times[i];
			}
		}
	}
	return temp;
}


/**
  * This functions creates a new root
  * @return pointer to the root created
*/			
root * make_root(int line_num){
	root *r;
	r = NEW(root);
	if(r != NULL){
		r->num = 0;
		r->line_num = line_num;
		r->head = NULL;
		r->tail = NULL;
	}
	return r;
}

/**
  * This functions creates a new path
  * @return pointer to the root created
*/			
path * make_path(){
	path *temp_path = NEW(path);

	if(temp_path == NULL)
		return NULL;	

	int i = 0;
	int j = 0;
	if(source_station->num_lines > 1 && dest_station->num_lines > 1){
		for(i = 0; i < 6; i++){
			if((source_station->lines[i] != NULL) && 
			   (dest_station->lines[i] != NULL)){
				j = i;
				goto here;
			}
			i = 0;
			j = 0;
		}	
	}	    
	else if((source_station->num_lines > 1) &&
		(source_station->lines[dest_station->line_num] != NULL)){
		i = dest_station->line_num;
		j = i;
	}
	else if((dest_station->num_lines > 1) &&
		(dest_station->lines[source_station->line_num] != NULL)){
		i = source_station->line_num;
		j = i;
	}
	else{
		if( i == 0 && j == 0){
			i = source_station->line_num;
			j = dest_station->line_num;
		}
	}
	
here:	temp_path->source[0] = source_station->lines[i]->line_num;
	temp_path->source[1] = source_station->lines[i]->station_num;
	
	temp_path->dest[0] = dest_station->lines[j]->line_num;
	temp_path->dest[1] = dest_station->lines[j]->station_num;

	memset(temp_path->color_path,0,300);
	memset(temp_path->station_path,0,300);
	temp_path->color_path[0] = temp_path->source[0];
	temp_path->station_path[0] = temp_path->source[1];
	temp_path->changes = 0;
	temp_path->time = 0;
	memset(temp_path->previous_stop,0,2);
	temp_path->found = 0;
	
	return temp_path;
}
	


int insert_at_head(root *r, station *temp_station, char *color, char *name, int station_num, 
                   int time, int stop_time, int num_lines, int line_num, int *transfer_colors, int *times)
{ 
  station *temp = NULL;
  int i = 0;
	
  if(temp_station != NULL){
	temp = temp_station;
	for(i = 0; i < num_lines; i ++){
		if(temp->lines[i] == NULL){
			make_line(color,station_num,time,stop_time,
		       		  num_lines,line_num,transfer_colors);
			break;
		}
	}
  }
			
  else
	temp = make_station(color,name,station_num,time,stop_time,num_lines,line_num,transfer_colors,times);
  
  if (temp == NULL) return -1;  // fail, cannot create new NODE

   if(r = NULL){
	r = make_root(line_num);
    	if (r == NULL) return -1;   // fail, cannot create ROOT
     }
  (r->num)++;

  if (r->num == 1)              // if previously the list is empty
    	r->tail = temp;
  else{
	int j = 0;
	for(j = 0; j < r->head->num_lines ; j++){
		if(r->head->lines[j]->line_num == line_num)
			break;
	}
	r->head->lines[j]->prev = temp;

  	temp->lines[i]->next = r->head;
  	r->head = temp;
     }
  return 0;
}


/**
  * This function inserts nodes at the end of the linked list
  * @param r points to the root of the list 
  * @param value is the data that is to be stored in the node
  * @return 0/1 success/failure
  */
station* insert_at_tail(root *r, station *temp_station, char *color, char *name, int station_num, 
		        int time, int stop_time,int num_lines,int line_num, int *transfer_colors, int *times){
  	station *temp = NULL;
	int i = 0;
	
 	 if(temp_station != NULL){
			if((temp_station->lines[line_num] == NULL) && (strcmp(temp_station->station_name,name) == 0)){
				temp_station->lines[line_num] = make_line(color,station_num,time,stop_time,
			       		 		           	  num_lines,line_num, transfer_colors);
				//	break;
			}
  	}
			
 	 else{
		temp_station = make_station(color,name,station_num,time,stop_time,
					    num_lines,line_num,transfer_colors,times);
	}
	temp = temp_station;
 		
	if(temp == NULL) return NULL;
	
   	if(r == NULL){
		r = make_root(line_num);
    		if (r == NULL) return NULL;   // fail, cannot create ROOT
    	 }
	
	(r->num)++;

	if(r->num == 1){	
		r->head = r->tail = temp;
	}
	else{
		r->tail->lines[line_num]->next = temp;
		temp->lines[line_num]->prev = r->tail;
		r->tail = temp;
	}
	return temp;
}

/**
  * This function displays the linked list
  */
void display_list(root *r)
{ station * temp;

  if (r != NULL)        // this is safer than (r.num != 0), why?
                        // if r==NULL, there will be a segmentation fault
                        //      when you try to use r.num
  { 	
   temp = r->head;
    do 
    { printf("%3s ", (temp->station_name));
      int  i = 0;
      for(i = 0; i < 6; i++){
		if((temp->lines[i] != NULL) && (temp->lines[i]->line_num == r->line_num))
			break;
	}
      temp = temp->lines[i]->next;
    } while (temp != NULL);
  }
  printf("\n");
}

/**
  * This function frees the memory from the list
  * @param r1 points to the root of the list to be freed
  */
/*
void clear_memory(root *r){
	station *temp, *temp1;
	temp = r->head;
	temp1 = r->head->next;
	do{
		free(temp);
		temp = temp1;
		temp1 = temp1->next;
	}while(temp1 != NULL);
	free(temp1);
}
*/
/**
  * This function compares colors with the input
  * to find the number assigned to it
  * @param color the input color
  * @return the number corresponding to that color
  */
int find_line_color(char *color){
	
	if(strcmp(color,"green") == 0)
		return 0;
	else if(strcmp(color,"yellow") == 0)
		return 1;
	else if(strcmp(color,"blue") == 0)
		return 2;
	else if(strcmp(color,"silver") == 0)
		return 3;
	else if(strcmp(color,"orange") == 0)
		return 4;
	else if(strcmp(color,"red") == 0)
		return 5;

}

/**
  * This function extracts the transfer stations
  * and their transfer times from the string input
  */
void tokenize(char * buffer,int num_lines, int *transfer_colors, int *times, int start){
	char * token = strtok(buffer, " ");
	int  j = 0;
	int  i = 0;
	int limit = 0;
	int color = 0;
	if(start)
		limit = 2 ;
	else
		limit = 3 ;

	for(j = 0; j < (limit + num_lines); j++){
		//printf("j %d, j-limit %d\n",j,j-limit+num_lines+1);
		if(j >= limit){
			token = strtok(NULL, " ");
			color = find_line_color(token);
			transfer_colors[i] = color;
			i++;
			//printf("color %s %d %d \n",token,color,i);
			token = strtok(NULL, " ");

			if(token != NULL)
				times[color] = atoi(token);
			else
				times[color] = atoi(buffer);
			
			//printf("time %s %d \n",token,times[color]);
			continue;
		}
		token = strtok(NULL, " ");
	}
	for(j = 0;j < num_lines; j ++){
//	printf("Color %d Time %d\n",transfer_colors[j],times[transfer_colors[j]]);
	}
}

/**
  * This funtions intialzes the array that stores
  * the roots of all the lines
  */
void make_roots(){
	line_colors = calloc(6,sizeof(root));
	int i = 0;
  	for(i = 0; i < 6; i++){
		line_colors[i] = make_root(i);
		if(line_colors[i] == NULL)
			exit(1);
	}
}


/**
  * Identifies transit stations and stores them
  */
void check_for_transit(station *temp, int *colors, int *times, int line_num){
	int i = 0;
	int j = 0;
	if(strcmp(temp->station_name,"Pentagon") == 0)	
		goto here;
	else if(strcmp(temp->station_name,"King_St_Old_Town") == 0)			
		goto here;
	else if(strcmp(temp->station_name,"East_Falls_Church") == 0)
		goto here;
	else if(strcmp(temp->station_name,"Fort_Totten") == 0)
		goto here;
	else if(strcmp(temp->station_name,"Gallery_Place") == 0)
		goto here;
	else if(strcmp(temp->station_name,"Stadium_Armory") == 0)
		goto here;
	else if(strcmp(temp->station_name,"Rosslyn") == 0)
		goto here;
	else if(temp->num_lines > 2)
		goto here;
	else
		return;
	
here:   for(i = 0; i < 10; i++){
		if((transit[i] != NULL) && (strcmp(transit[i]->station_name,temp->station_name) == 0))
			goto there;
			//return;
	}
	for(i = 0; i < 10; i++){
		if(transit[i] == NULL){
			transit[i] = temp;
				break;
		}
	}
there:	
	for(j = 0;j < temp->num_lines; j++){
		transit[i]->transfer_time[line_num][colors[j]] = times[colors[j]];
	//	printf("time %d %d\n",colors[j],times[colors[j]]);
	}
	
}

/**
  * Compares names to find the source and destination
  */
void check_for_source_dest(station *temp, char *source, char *dest){
	
	if(strcmp(temp->station_name,source) == 0)
		source_station = temp;
		
	else if(strcmp(temp->station_name,dest) == 0)
		dest_station = temp;
}
/**
  * This funtions finds direction to move from
  * the source towards the destination, checks 
  * if its in the same line and where to go to
  * the next or prev station
  */
int get_direction(line *l,path *temp){
		if(l->station_num > temp->dest[1]){
			return 0;
		}
		else if(l->station_num < temp->dest[1]){
			return 1;
		}
}
		
/**
  * This function is used to find the transit
  * station that is closest to the source
  */
station * find_closest_transit(line *l,path *temp,int to_next){
	int i = 0;
	int smallest = 0;
	int flag = 0;
	for(i = 0; i < 10; i++){
		//printf("Trnas %s\n",transit[i]->station_name);
		if((to_next == 0) && (transit[i] != NULL) && (transit[i]->lines[l->line_num] != NULL) &&
		   (transit[i]->lines[l->line_num]->station_num < l->station_num)){
			if(smallest == 0){
				//printf(" Stat col1 %s %d %d %d\n",transit[i]->station_name,l->line_num,smallest,i);
				smallest = l->station_num - transit[i]->lines[l->line_num]->station_num ;
				flag = i;
				continue;
			}
			//	printf(" Stat col1 %s %d %d %d\n",transit[i]->station_name,l->line_num,smallest,i);
		
			if((l->station_num - transit[i]->lines[l->line_num]->station_num) < smallest){
				smallest = l->station_num - transit[i]->lines[l->line_num]->station_num ;
				flag = i;
			}
				
		}
		else if((to_next == 1) && (transit[i] != NULL) && (transit[i]->lines[l->line_num] != NULL) &&
			(transit[i]->lines[l->line_num]->station_num > l->station_num)){
			//if(transit[i]->lines[l->line_num]->station_num > l->station_num)
			if(smallest == 0){
				smallest = (transit[i]->lines[l->line_num]->station_num) - l->station_num;
				flag = i;
			//	printf(" Stat col2 %s %d %d %d\n",transit[i]->station_name,l->line_num,smallest,i);
			//	printf("Staton line %d %d\n",transit[i]->lines[l->line_num]->station_num,l->station_num);
				continue;
			}
		
			if((transit[i]->lines[l->line_num]->station_num - l->station_num) < smallest){
				smallest = transit[i]->lines[l->line_num]->station_num - l->station_num;
				flag = i;
			}
			//	printf(" Stat col2 %s %d %d %d\n",transit[i]->station_name,l->line_num,smallest,i);
				
		}	
	}
	if( smallest == 0)
		return NULL;
	//printf("transit %s\n",transit[flag]->station_name);
	return transit[flag];		
}
/**
  * This function update the color and stations
  */
/*void fill_path(station *temp,path *p){
	int i = 0;
	while(i < 100){
		if(color_path[i] != 0 && station_path[i] !=0)
			i++;
		else
			break;
	}
	
	p->_path[i] = temp->
}dest_station->lines[source_station->line_num] 
*/

/**dest_station->lines[source_station->line_num] 
  * This function intiates the search for the dest 
  * from the source
  */
int check_neighbours(int color,int station_num,station *temp,path *p,int to_next){
	if(to_next == 1){
		if(temp != NULL){
			if((p->dest[1] > station_num) && (p->dest[1] <= temp->lines[color]->station_num)){
				p->found = 1;
				return 0;
			}
		}
		else if(temp == NULL){
			if((p->dest[1] > station_num)){
				p->found = 1;
				return 0;
			}
		}
	}
	else if(to_next == 0){
		if(temp != NULL){
			if((p->dest[1] < station_num) && (p->dest[1] >= temp->lines[color]->station_num)){
				p->found = 1;
				return 0;
			}
		}
		else if(temp == NULL){
			if((p->dest[1] < station_num)){
				p->found = 1;
				return 0;
			}
		}
	
	}	
	else
		return 1;
	
}


/**
  * This function intiates the search for the dest 
  * from the source
  */
void update_path(path *p, station *start, station *stop, int color, int station_num, int to_next){
	int current_time =  start->lines[color]->time;
	int i = 1;
	station *prev_stop = NULL;
	//printf("here st  %d\n",start->lines[color]->stop_time);
	//printf("here t %d\n",start->lines[color]->time);
	//int stop_num = stop->lines[color]->station_num;
	if(to_next == 0){
		do{	update_count++;
			//printf("on the way %s\n",start->station_name);
			p->time += (current_time - start->lines[color]->time);
			current_time = start->lines[color]->time;			
			//printf("Current Time %d, Time%d\n",current_time,p->time);
			if(i == 1){
				i = 0;
				start = start->lines[color]->prev;
				continue;
			}
			p->time += start->lines[color]->stop_time;
		//	printf("Stop Time  %d\n",start->lines[color]->stop_time);
		//	printf("on the way %s %d \n",start->station_name,start->lines[color]->station_num);
			prev_stop = start;
			start = start->lines[color]->prev;
			//printf("on the way %s %d\n",start->station_name,start->lines[color]->station_num);
		//	printf("Current Time %d, Time %d\n",current_time,p->time);
		}while((start != NULL) && 
		       (start->lines[color]->station_num >= stop->lines[color]->station_num));
		
		if((prev_stop != NULL)){
			p->time -= prev_stop->lines[color]->stop_time;
		}
	}
	else if(to_next == 1){
		do{	update_count++;
			p->time += (start->lines[color]->time - current_time);
			current_time = start->lines[color]->time;			
		//	printf("on the way %s %d \n",start->station_name,start->lines[color]->station_num);
			//printf("Current Time %d, Time %d\n",current_time,p->time);
			if(i == 1){
				i = 0;
				start = start->lines[color]->next;
				//printf("here\n");
				continue;
			}
			p->time += start->lines[color]->stop_time;
		//	printf("Stop Time  %d\n",start->lines[color]->stop_time);
			prev_stop = start;
			start = start->lines[color]->next;
		//	printf("Current Time %d, Time %d\n",current_time,p->time);
			//printf("on the way %s %d \n",start->station_name,start->lines[color]->station_num);
		}while((start != NULL) && (start->lines[color]->station_num <= stop->lines[color]->station_num));

		if((prev_stop != NULL)){
			p->time -= prev_stop->lines[color]->stop_time;
		}
	}
	
	//p->color_path[p->changes] = color;
	//p->station_path[p->changes] = stop->lines[color]->station_num;
	//p->changes++;
	p->previous_stop[0] = color;
	p->previous_stop[1] = station_num;
}		
		
/**
  * This function creates a copy of the path
  * in the global paths variable
  */
path * duplicate_paths(path *p){
	int i = 0;
	for(i = 0; i < 100; i++){
		if(paths[i] == NULL)
			break;
	}
	paths[i] = NEW(path);
	memcpy(paths[i],p,sizeof(path));
	return paths[i];
}

/**
  * This function intiates the search for the dest 
  * from the source
  */
path * find_path(station *temp,path *p){	
//	test++;
//	if(test == 20)
		//exit(0);
	int i = 0;
	int j = 0;
	int to_next = 0;
	char line[10];
	int color = 0;
	int station_num = 0;
	station *closest_transit = NULL;
	station *next_transit = NULL;
	station *prev_transit = NULL;
//	printf("Name %s\n",temp->station_name);
	count+= 2;	
	if(strcmp(temp->station_name,source_station->station_name) == 0){
		source = 1;
		complete_path[0] = p->source[0];
	}
	//names[count] = temp;	

	if(p == NULL)
		p = make_path();
	for(i = 0; i < 6; i++){
		if((temp->lines[i] != NULL) && (temp->lines[i]->visited != 1)){
	//	printf("I %d  first check %s %d\n",i,temp->station_name, temp->lines[i]->line_num );
		//if(temp->lines[i] != NULL){
			color = temp->lines[i]->line_num;
			station_num = temp->lines[i]->station_num;
			if(color == p->dest[0]){
				colors[count] = color;
				temp->lines[i]->visited = 1;
				//printf("Set Visited %s %d\n",temp->station_name, temp->lines[i]->visited);
				to_next = get_direction(temp->lines[i],p);
			there:	closest_transit = find_closest_transit(temp->lines[i],p,to_next);
				check_neighbours(color,station_num,closest_transit,p,to_next);
				//	printf("p time %d\n",p->time);
					p->time += temp->transfer_time[p->previous_stop[0]][i];
				//	printf("p time %d %d\n",p->time,closest_transit->transfer_time[color][i]);
				//printf("to next color %d dir %d\n",to_next[0],to_next[1]);
			//	if(closest_transit != NULL)
			//		printf("Same color %s, %d\n",closest_transit->station_name,to_next);
				if(p->found == 1){
					update_path(p,temp,dest_station,color,station_num,to_next);
					if((count != 2) && (i == complete_path[count-3])){
						complete_path[count-2]+= update_count-1;
						complete_path[count-1] = to_next;
						count-= 3;
					}
					else{
						complete_path[count] = i;
						complete_path[count+1] = update_count-1;
						complete_path[count+2] = to_next;
						strcpy(names[name_count],temp->station_name);
						name_count++;

					}
					count++;
				//	printf("udatre_count %d\n",update_count);
					update_count = 0;	
					goto here;
				}
				p->changes++;
			//	update_path(p,temp,dest_station,color,station_num,to_next);
				update_path(p,temp,closest_transit,color,station_num,to_next);
				p->time += closest_transit->lines[color]->stop_time;
				if((count != 2) && (i == complete_path[count-3])){
				//	printf("color %d %d %d %d\n",i,complete_path[count],count-3,complete_path[count-3]);
					complete_path[count-2]+= update_count-1;
					complete_path[count-1] = to_next;
					count-= 3;
				}else{
					complete_path[count] = i;
					complete_path[count+1] = update_count-1;
					complete_path[count+2] = to_next;
					strcpy(names[name_count],temp->station_name);
					name_count++;
				}
				count++;
				//	printf("udatre_count %d\n",update_count);
				update_count = 0;	
				find_path(closest_transit,p);
				//printf("changes %d path_num %d\n",p->changes,path_num);
				//if(path_num != p->changes)
						goto here;
				
			}
		}
	}
	for(i = 0; i < 6; i++){
		if((temp->lines[i] != NULL) && (temp->lines[i]->visited != 1)){
		//	printf("I %d  second check %s %d\n",i,temp->station_name, temp->lines[i]->line_num );
			color = temp->lines[i]->line_num;
			station_num = temp->lines[i]->station_num;
				next_transit = find_closest_transit(temp->lines[i],p,1);
				prev_transit = find_closest_transit(temp->lines[i],p,0);
				temp->lines[i]->visited = 1;
				
				if((next_transit != NULL) &&
				   (next_transit->lines[i]->visited != 1)){
				colors[count] = color;
					p->changes++;
					p->time += temp->transfer_time[p->previous_stop[0]][i];
					update_path(p,temp,next_transit,color,station_num,1);
					if((count != 2) && (i == complete_path[count-3])){
			//	printf("color %d %d %d %d\n",i,complete_path[count],count-3,complete_path[count-3]);
						complete_path[count-2]+= update_count-1;
						complete_path[count-1] = 1;
						count-= 3;
					}
					else{
						complete_path[count] = i;
						complete_path[count+1] = update_count-1;
						complete_path[count+2] = 1;
						strcpy(names[name_count],temp->station_name);
						name_count++;
					}
					count++;
			//		printf("udatre_count %d\n",update_count);
					update_count = 0;	
					p->time += next_transit->lines[color]->stop_time;
					//path *p_temp = duplicate_paths(p);
				//	printf("Next transit %s, %d\n",next_transit->station_name,color);
				//	printf("p time %d\n",p->time);
				//	printf("p time %d %d\n",p->time,temp->transfer_time[p->previous_stop[0]][i]);
					find_path(next_transit,p);
					break;
			//		printf("Diff color after recursion with next transit\n");
				}
				else if((prev_transit != NULL) &&
				   (prev_transit->lines[i]->visited != 1)){
				colors[count] = color;
					p->changes++;
					p->time += temp->transfer_time[p->previous_stop[0]][i];
					//printf("Prev transit %d\n",p->previous_stop[1]);
					//printf("Next transit %d, %d\n",temp->station_name,color);
					update_path(p,temp,prev_transit,color,station_num,0);
					if((count != 2) && (i == complete_path[count-3])){
				//	printf("color %d %d\n",i,count);
						complete_path[count-2]+= update_count-1;
						complete_path[count-1] = 0;
						count-= 3;
					}
					else{
						complete_path[count] = i;
						complete_path[count+1] = update_count-1;
						complete_path[count+2] = 0;
						strcpy(names[name_count],temp->station_name);
						name_count++;
					}
					count++;
			//		printf("udatre_count %d\n",update_count);
					update_count = 0;	
					p->time += prev_transit->lines[color]->stop_time;
			//		printf("Next transit %s, %d\n",prev_transit->station_name,color);
					//path *p_temp = duplicate_paths(p);
			//		printf("p time %d\n",p->time);
					//p->time += prev_transit->transfer_time[color][i];
					//printf("p time %d %d\n",p->time,prev_transit->transfer_time[color][i]);
					find_path(prev_transit,p);
					break;
			//		printf("Diff color after recursion with prev transit\n");
				}
			}
		}
		//path_num++;		
		//source = transit[
	
	
/*	for(i = 0; i <100;i++){
		if(paths[i] != NULL && paths[i]->found == 1){
			printf("time %d %d\n",paths[i]->time,i);	
			break;
		}
	}
*/
here://	printf(" path %d is done \n",path_num);
	path_num++;
	return paths[0];	
}

// This funstions finds the color corresponding to the number
void fetch_color(char *line,int value){
		int i = 0;
		if(value == 0)
			strcpy(line,"green");
		else if(value == 1)
			strcpy(line,"yellow");
		else if(value == 2)
			strcpy(line,"blue");
		else if(value == 3)
			strcpy(line,"silver");
		
		else if(value == 4)
			strcpy(line,"orange");
		else if(value == 5)
			strcpy(line,"red");
		
}

// this function gets the line direction
void direction(char *dir,int color, int to_next){
		if(to_next)
			strcpy(dir,line_colors[color]->tail->station_name);
	
		else
			strcpy(dir,line_colors[color]->head->station_name);
}		

/**
  * This function prints the outout in the desired format
  */
void store_output(FILE *file, int time){
	char start[100];
	int  i = 0;
	char line[10] = {'\0'};
	char dir[30];
	int j = 1;
	fprintf(file,"start from %s station\n",names[0]);
	for(i = 2; i < 30; i++){
			if(strcmp(names[j],"")==0){
				strcpy(names[j],dest_station->station_name);
			}
			fetch_color(line,complete_path[i]);
			direction(dir,complete_path[i],complete_path[i+2]);
			fprintf(file,"take %s line towards %s for %d stations to %s\n",
				line,dir,complete_path[i+1],names[j]);
			j++;
			if(complete_path[i+3] == 0 && complete_path[i+4] == 0)
				break;		
			fetch_color(line,complete_path[i+3]);
			fprintf(file,"transfer to %s line\n",line);
			i+= 2;
		}
	fprintf(file,"arrive %s station in %d minutes %d seconds\n",dest_station->station_name,time/60,time%60);
}
				
/**
  * This functions gets the inputs from the user,
  * reads the file and helps fill data
  */
int main(int argc, char *argv[]){
	
	FILE *file = fopen("metro.txt","r");
	make_roots();
	char buffer[1000] = {0};
	int result = 0;
	char color[10] = {'\0'};
	char name[30] = {'\0'};
	int station_num = 1;
	int num = 0;
	int num_lines = 0;
	int line_num = 0;
	int time = 0;
 	int stop_time = 0;
	int *transfer_colors = NULL; 
	int times[6] = {0};
	int count = 0;
	char source_name[30] = "" ;
	char dest_name[30] = "";	
	station *current_station = NULL;
	path *optimal_path = NULL;;

	printf("Enter the Source Station:\n");
	scanf("%s",source_name);
	printf("Enter the Destination Station:\n");
	scanf("%s",dest_name);
	
	while(fgets(buffer,100,file) != NULL){
		if(strcmp(buffer,"\n") == 0){
			count = 0;
			station_num = 1 ;
			continue;
		}
		if(count == 0){
			result = sscanf(buffer,"%s (%d)",color,&num);
 			line_num = find_line_color(color);
			count = num;
			continue;
		}		

		int i = 0;
		result = sscanf(buffer,"%s %d %d %d", name, &num_lines, &time, &stop_time);

		transfer_colors = calloc(num_lines,sizeof(int));

		if(num_lines != 0){
			int start = 0;
			if(count == num){
				start = 1;
			}

			tokenize(buffer,num_lines,transfer_colors,times,start);

			for( i = 0; i < 100; i++){

				if(multiple_lines[i] != NULL){

					if(strcmp(multiple_lines[i]->station_name,name) == 0){
						break;
					}
				}
				else if(multiple_lines[i] == NULL){
					break;
				}
				
			}
			multiple_lines[i] = insert_at_tail(line_colors[line_num],multiple_lines[i],color,name,station_num,
							   time,stop_time,num_lines,line_num,transfer_colors,times);

			current_station = multiple_lines[i];	
			check_for_transit(multiple_lines[i],transfer_colors,times,line_num);
		}
		else{	
			current_station = insert_at_tail(line_colors[line_num],NULL,color,name,station_num,
				       			 time,stop_time,num_lines,line_num,transfer_colors,times);
		}
		
		check_for_source_dest(current_station,source_name,dest_name);		
		station_num++;	
		memset(times,0,6);
		count--;
	}
		//display_list(line_colors[4]);
		int i = 0;
		int j = 0;
		for(i = 0; i < 6; i++){
			if(transit[0] != NULL){
			for(j = 0; j < 6; j++){
			//	printf("Transit time %d \n",transit[0]->transfer_time[i][j]);	
			}
		}
		}
		if(source_station == NULL || dest_station == NULL){
			printf("Please enter the correct station names\n");
			exit(1);
		}
		paths[0] = make_path();	
		find_path(source_station,paths[0]);
		int path_no = 0;
		i = 0;
		int temp = paths[0]->time;
		for(i = 0;i <100; i++){
			if((paths[i] != NULL) && (temp <= paths[i]->time) && (paths[i]->found == 1)){
				path_no = i;
				temp = paths[i]->time;
		}
		}
		//printf("time %d %d\n",temp,path_no);	
		FILE *file1 = fopen(argv[1],"w");
		store_output(file1,temp);	
	
	
	fclose(file);
	fclose(file1);
	return 0;
}

