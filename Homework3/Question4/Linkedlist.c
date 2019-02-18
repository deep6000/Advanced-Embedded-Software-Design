
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/list_sort.h>
#include <linux/errno.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Deepesh Sonigra");
MODULE_DESCRIPTION("Linked List ");

static char *animals[50]={ "dog","cat", "elephant", "mouse", "frog", "spider", "shark" ,"elephant", "mouse", "frog","spider" ,"dog","cat", 				   "elephant", "mouse", "frog", "fish", "toad", "spider", "shark" ,
		           "frog", "fish", "toad", "spider", "mouse", "dog" , "cat", "fish",  "frog", "spider","elephant", "mouse", "frog","spider" ,"dog","cat", 				   "elephant", "mouse", "frog", "fish","toad", "spider",  "mouse","spider", "shark" ,"elephant", "mouse","spider", "shark", "dog"};

static char *AnimalType = "NULL";
int ArrayCount  = sizeof(animals)/sizeof(animals[0]);
static int CountGreaterThan = 0;
struct timespec time;
unsigned long start_time = 0;
unsigned long end_time = 0;

module_param(AnimalType,charp,0644); 
module_param(CountGreaterThan,int,0644); 



struct UniqueEcosystem
{
	char *UniqueAnimal;  
	int AnimalCount; 
	struct list_head Unique_list; 
};


struct FilteredEcosystem
{
	char *FilteredAnimals; 
	int AnimalFCount; 
	struct list_head Filtered_list; 
};

//List Initialization
LIST_HEAD(NoDuplicate_list);
LIST_HEAD(MyFiltered_list);

static int __init AnimalDataSortInit(void)
{ 
	
	struct FilteredEcosystem *ptrfilter1, *ptrfilter2;
	struct list_head *pos2;
	struct UniqueEcosystem *obj, *my_obj,*my_obj1;
	struct list_head *pos;
	int i,j;
	int count_animal =1; 
	int UniqueCount= 0;
	char *TempVar;
	printk(KERN_ALERT "Entering Sorting Module\n");
	printk("This is Implemented Using Linked List Data Structure\n");
	char *SortedArray[50];
	int CountArray[50];

	start_time = 0;
	end_time = 0;

start_time = (time.tv_nsec)/1000;
	for(i=0;i<ArrayCount-1;i++)
	{
		for(j=0;j<ArrayCount-i-1;j++)
		{
			if(strcmp(animals[j],animals[j+1]) < 0)
			{
				TempVar = animals[j];
				animals[j] = animals[j+1];
				animals[j+1] = TempVar;
			}
		}
	}

/*counting and removing duplicates*/
	for(i=0;i<ArrayCount-1 ;i++)
    	{

        	if(strcmp(animals[i],animals[i+1]) == 0)
        	{
        		count_animal++;
		printk("Duplicate Entry: %s\n",animals[i]);
        	}

        	else
        	{
        		CountArray[UniqueCount]= count_animal;
        		SortedArray[UniqueCount]= animals[i];
        		UniqueCount++;
        		count_animal=1;
        	}

    }
        CountArray[UniqueCount]= count_animal;
        SortedArray[UniqueCount]= animals[i];
	UniqueCount++;
	
	for(i=0; i < UniqueCount; i++)
	{
		obj = (struct UniqueEcosystem *)kmalloc(sizeof(struct UniqueEcosystem), GFP_KERNEL); //allocate memory
		if(obj == NULL)
		{
				printk(KERN_ERR "Error allocating structure\n");
				return -ENOMEM; //Return "Cannot allocate memory" on error
		}
		obj->UniqueAnimal = SortedArray[i];  
		obj->AnimalCount = CountArray[i];
		
		list_add(&obj->Unique_list, &NoDuplicate_list); 
				
	}

	printk("Animals Name List\n");	
	
	list_for_each(pos,&NoDuplicate_list)
	{
		my_obj = list_entry(pos, struct UniqueEcosystem, Unique_list);
		printk("Animal: [%s] \t\t Count: [%d]\n",my_obj->UniqueAnimal,my_obj->AnimalCount);
	}

	printk("The Size of Ecosystem 1 is  [%d]", UniqueCount);	

	printk(" Dynamically Allocated Memory for nodes in Ecosystem 1 is : [%zu Bytes]\n", (UniqueCount * sizeof(struct UniqueEcosystem)));



	if((strcmp(AnimalType,"NULL") == 0) && CountGreaterThan == 0)
	{
		
	
		printk("Printing All Animal List as no filter selected\n");
		
		
		list_for_each_entry(my_obj,&NoDuplicate_list,Unique_list)
		{

			ptrfilter1 = (struct FilteredEcosystem *)kmalloc(sizeof(struct FilteredEcosystem), GFP_KERNEL); 
			if(ptrfilter1 == NULL)
			{
				printk(KERN_ERR "Error allocating structure");
				return -ENOMEM;
			}
			ptrfilter1->FilteredAnimals = my_obj->UniqueAnimal;
			ptrfilter1->AnimalFCount = my_obj->AnimalCount;
 			list_add(&ptrfilter1->Filtered_list, &MyFiltered_list);
			 
		}

		list_for_each(pos2,&MyFiltered_list)
		{
			ptrfilter2 = list_entry(pos2, struct FilteredEcosystem, Filtered_list);
			printk("Animal: %s \t\t Count : [%d] \n",ptrfilter2->FilteredAnimals,ptrfilter2->AnimalFCount);
		}
	
		printk("Size of Ecosystem 2 is : %d", UniqueCount);	
	
		printk("Memory Allocated to nodes in Ecosystem 2 is : {%zu bytes]\n", (UniqueCount * sizeof(struct FilteredEcosystem)));
	}

/* Filter  Animal Type */
	else if(CountGreaterThan == 0)
	{
		int node_filterat= 0;	
		printk(" Filter Chosen : Animal Type = %s.\n", AnimalType);
		list_for_each_entry(my_obj,&NoDuplicate_list,Unique_list)
		{
			if(strcmp(AnimalType,my_obj->UniqueAnimal) == 0) 
			{
				/* Allocate memory */
				ptrfilter1 = (struct FilteredEcosystem *)kmalloc(sizeof(struct FilteredEcosystem), GFP_KERNEL);
				if(ptrfilter1 == NULL)
				{
					printk(KERN_ERR "Error allocating structure");
					return -ENOMEM;
				}
				ptrfilter1->FilteredAnimals = my_obj->UniqueAnimal;
				ptrfilter1->AnimalFCount = my_obj->AnimalCount;
				list_add(&ptrfilter1->Filtered_list, &MyFiltered_list); //Add filtered animals to the second list
				node_filterat++; //Track number of nodes allocated
				printk("Animal Selected: [%s] \t\t Count: [%d]\n",ptrfilter1->FilteredAnimals,my_obj->AnimalCount);
				break;
			}
		}
		
		printk("Size of Ecosystem Animal Type: %d", node_filterat);	

		printk("Memory Allocated in Animal Type Filter: [%zu bytes]\n", (node_filterat * sizeof(struct FilteredEcosystem)));
	}


/* Filter  Count Type */
	else if(strcmp(AnimalType,"NULL") == 0)
	{
		int node_filterct= 0;	
		printk(" Filter Chosen : Count Greater Than =[ %d].\n", CountGreaterThan);
		list_for_each_entry(my_obj1,&NoDuplicate_list,Unique_list)
		{
			
			if( my_obj1->AnimalCount > CountGreaterThan) 
			{
				/* Allocate memory */
				ptrfilter1 = (struct FilteredEcosystem *)kmalloc(sizeof(struct FilteredEcosystem), GFP_KERNEL);
				if(ptrfilter1 == NULL)
				{
					printk(KERN_ERR "Error allocating structure");
					return -ENOMEM;
				}
				ptrfilter1->FilteredAnimals = my_obj1->UniqueAnimal;
				ptrfilter1->AnimalFCount = my_obj1->AnimalCount;
				list_add(&ptrfilter1->Filtered_list, &MyFiltered_list); 
				node_filterct++; 
				printk("Animal: [%s] \t\t Count: [%d]\n",ptrfilter1->FilteredAnimals,my_obj1->AnimalCount);
				break;
			}
		}
		
		printk("Size of Ecosystem Count Greater Than [%d] : [%d]",CountGreaterThan, node_filterct);	

		printk("Memory Allocated in Animal Type Filter:[ %zu bytes]\n", (node_filterct * sizeof(struct FilteredEcosystem)));
	}

	/* Filter  Animal type and count greater than */
	else
	{
		int node_filteract = 0;
		printk("Filter Choosen  Count Greater Than  [%d] and Animal Type = %s.\n", CountGreaterThan, AnimalType);
		
		 
		list_for_each_entry(my_obj,&NoDuplicate_list,Unique_list)
		{
			if(my_obj->AnimalCount> CountGreaterThan && (strcmp(my_obj->UniqueAnimal, AnimalType) == 0))
			{
				/* Allocate memory */
				ptrfilter1 = (struct FilteredEcosystem *)kmalloc(sizeof(struct FilteredEcosystem), GFP_KERNEL);
				if(ptrfilter1 == NULL)
				{
					printk(KERN_ERR "Error allocating structure");
					return -ENOMEM;
				}
				ptrfilter1->FilteredAnimals = my_obj->UniqueAnimal;
				list_add(&ptrfilter1->Filtered_list, &MyFiltered_list); 
				node_filteract++;
				printk("Animals :[%s]/t/t Count:  [%d]\n",ptrfilter1->FilteredAnimals,my_obj->AnimalCount);
				break;
			}
		}
		
		printk("Ecosystem Size Both Filters [%d]", node_filteract);	

		printk("Memory Allocated : [%zu bytes]", (node_filteract * sizeof(struct FilteredEcosystem)));
	}

	 getnstimeofday(&time);
	 end_time = (time.tv_nsec)/1000;
 printk("Init  Module Time Taken :[%lu us]",end_time-start_time);
		
	return 0;
}






static void AnimalDataSortExit(void)
{
	
	int node_uc = 0;
	int node_fc = 0;
	struct list_head *temp1, *temp2;
	struct UniqueEcosystem *tmpue;
	struct FilteredEcosystem *tmpfe;
	

	start_time = 0;
	end_time = 0;
	getnstimeofday(&time);
	start_time = (time.tv_nsec)/1000;

	list_for_each_safe( temp1, temp2, &NoDuplicate_list ) 
	{

    	tmpue = list_entry( temp1, struct UniqueEcosystem , Unique_list );
    	list_del( temp1 ); 
    	kfree( tmpue );	
	node_uc++;
  	}

	printk("Memory Freed from Unique Ecosystem : [%zu bytes]\n", node_uc * sizeof(struct UniqueEcosystem)); 

	list_for_each_safe( temp1, temp2, &MyFiltered_list ) {
    	
    	tmpfe = list_entry( temp1, struct FilteredEcosystem, Filtered_list );
    	list_del( temp1 ); 
    	kfree( tmpfe ); 
	node_fc++;
  	}
	 getnstimeofday(&time);
	 end_time = (time.tv_nsec)/1000;
	

	printk("Memory Freed from Filtered Ecosystem: [%zu bytes]\n", node_fc * sizeof(struct FilteredEcosystem));
	printk("Exiting Module");
 printk("Exit Module Time Taken :%lu us",end_time-start_time);
}
module_init(AnimalDataSortInit);
module_exit(AnimalDataSortExit); 
