#include<stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
	int dest;
	int link;
	int metric;
	int flag;
}TABLE;


FILE* fp_org;
FILE* fp_cmp;

TABLE origin[10]={0};
TABLE compare[10]={0};
TABLE*  point_origin= origin;
TABLE*  point_compare= compare;
int lowmetric=0;
int lowdest=0;
int tableFlag=0;
TABLE* lowpt;
void ReadNInsert(FILE* fp,TABLE* tablept){
	char s[100];
	char* token;
	while((fgets(s,100,fp))!=NULL)
	{
		token=strtok(s," ");
		(tablept)->dest = atoi(token) ; 
		(tablept)->link = atoi(strtok(NULL," "));
		(tablept)->metric = atoi(strtok(NULL," "));
		(tablept)++;
	}
	fclose(fp);

}

void calculate(TABLE* origin,TABLE* compare,int* path)
{
	int* path_p = path;
	int check=0;
	int comparemetric = 0;
	TABLE* origin_p = origin;
	for(; compare->dest!=0 ; compare++,origin=origin_p,path=path_p,check=0)
	{
		printf("compare dest:%d\n",compare->dest);
		for(;*path!= 0;path++)
		{	
			if(compare->dest==*path)
			{	
				printf("path value is :%d\n",*path);
				printf("check become 1\n ");
				check=1; //already this node is checked
				break;
			}
		}
		if(check==1)
			continue;
		else
		{
			for( ;origin->dest!= compare->dest; origin++)
			{	
				if(origin->dest==0)
					break;
			}
			if(origin->dest==0)
			{
				origin->dest = compare->dest;
				origin->link = lowdest;
				origin->metric= compare->metric+ lowmetric;
				continue;
			}
			else
			{
				comparemetric = compare->metric + lowmetric;
				printf("cmoparemetric:%d, origin metric:%d ",comparemetric,origin->metric);	
				if(origin->metric > comparemetric)
					{
						origin->link= lowdest;
						origin->metric= comparemetric;
						printf("metric is updated\n");
					}
			}	
	
				

		}
			
	}


}

void printTable(char* s,TABLE* table)
{
	TABLE* immpt;
	 printf("----------------------------%s_table---------------------------------\n",s);
	 for(immpt=table; immpt->dest != 0 ; immpt++){
                printf("flag:%d , dest: %d, link: %d , metric: %d\n",immpt->flag,immpt->dest,immpt->link,immpt->metric);
        }
        printf("---------------------------------------------------------------------------\n");

}

void findShortest()
{
	TABLE* immpt;
        for (lowmetric=0,lowdest=0,immpt = origin; immpt ->dest !=0 ; immpt++)   // until end of file
        {
                if(immpt->flag ==1)
                        continue;
                if (lowmetric == 0)             // the first time fo comparision --> initialize low dest and metric
                {
                        lowpt=immpt;
                        lowmetric = immpt->metric;
                        lowdest = immpt->dest;
                }
                 if (lowmetric > immpt->metric)  // if there is dest which has lower metric then change value
                {
                        lowpt=immpt;
                        lowmetric = immpt->metric;
                        lowdest = immpt->dest;
                }
        }
	lowpt->flag = 1;
}

int main(int argc,char* argv[])
{
	int nodeNum=atoi(argv[1]);
	printf("nodeNum:%d\n",nodeNum);
	FILE* filearray[7];             //remove when combine the code!
	filearray[1]=fopen("input1.txt","r"); //arbitaray setting 
	filearray[2]=fopen("input2.txt","r");     //arbitarary setting
	filearray[3]=fopen("input3.txt","r");
	filearray[4]=fopen("input4.txt","r");
	filearray[5]=fopen("input5.txt","r");
	filearray[6]=fopen("input6.txt","r");
	fp_org = filearray[1];// fopen("input2.txt","r"); //set this on the main when combine code.


	ReadNInsert(fp_org, point_origin);
	int path[10]={0};
	int*  p_path;
	int i = 0 ; //for literative function
	p_path= path;
	*p_path = 1; // store itself
	p_path ++;
	printTable("initial",origin);
	findShortest();
	printf("low dest : %d \n",lowdest);
 	*p_path = lowdest;
	 p_path++;
	printf("nodeNum:%d\n",nodeNum)	;
	for(i=0;i<(nodeNum-2);i++)
	{
		fp_cmp=filearray[lowdest];
		printf("open file well\n");
		ReadNInsert(fp_cmp,point_compare);
		printTable("origin",origin);
		printTable("compare",compare);
		calculate(origin,compare,path);
        	printTable("updated",origin);
        	memset(&compare,0,sizeof(compare));
        	findShortest();
       		printf("low dest : %d \n",lowdest);
        	*p_path = lowdest;
		p_path++;


	}
	printf("dijstra path:");
        for(int* imm=path;imm<p_path;imm++)
        	printf("%d-",*imm);
	printf("\n");
	printTable("Final",origin);
	tableFlag=1;
	return 0;
}