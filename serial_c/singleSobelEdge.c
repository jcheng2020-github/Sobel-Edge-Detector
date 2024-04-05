//Lab 4 Part 1: Sobel Edge Detection
//Junfu Cheng UFID: 77844387
//University of Florida

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

/*   ttype: type to use for representing time */
typedef double ttype;
ttype tdiff(struct timespec a, struct timespec b)
/* Find the time difference. */
{
  ttype dt = (( b.tv_sec - a.tv_sec ) + ( b.tv_nsec - a.tv_nsec ) / 1E9);
  return dt;
}

struct timespec now()
/* Return the current time. */
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t;
}

#define MATRIX_SIZE 5000

//clock_t begin, end;
struct timespec begin, end;//junfu.cheng revised
double time_spent;

int* mask_operation (int *recv_buff, int N, int *send_count, int task_id , int *g_send_count);

int main(int argc, char* argv[])
{
    int image[MATRIX_SIZE][MATRIX_SIZE];
    int* resultImg;
    
    FILE *file;
    int c;
	char c1;
	char strbuff[100] = {0};
    char buffer [10];
    
    int num_tasks = 1;;
    int *rbuff;
    int N = MATRIX_SIZE;
    int *send_count;
    int task_id = 0;
    int *g_send_count;
    int *updated_values;
    
    send_count = (int*)malloc(num_tasks*sizeof(int));
    // calculate send counts and displacements
    for(int i = 0; i < num_tasks; i ++)
    {
        send_count[i] = (i < (N-2)%num_tasks) ? (N-2)/num_tasks + 1 : (N-2)/num_tasks;
        
    }
    for(int i = 0; i < num_tasks; i ++)
    {
        send_count[i] = send_count[i] + 2;
        
        send_count[i] = send_count[i] * N;
        
    }
    g_send_count  = (int*)malloc(num_tasks*sizeof(int));
    for(int i = 0; i < num_tasks; i++)
    {
        g_send_count[i] = (N-2) * (send_count[i]/N - 2);

    }
    
    
    
    
    file = fopen("input.txt", "r");
	if (file)
	{
        int i = 0, j = 0;
		while((c=getc(file))!=EOF)
		{
            //printf("%d\n",c);
			c1 = (char)c;

            if(c != 9 && c != 13 && c != 10)
            {
			strncat(strbuff,&c1,1);
            }
			//printf("%s\n", strbuff);
            
            if(c == 9 || c == 13 || c == 10)
            {
                if(c == 9)
                {
                    image[i][j] = atoi(strbuff);
                    j++;
                }
                if(c == 13 )
                {
                    image[i][j] = atoi(strbuff);
                    i++;
                    j = 0;
                }
                strbuff[0] = '\0';
            }
		}
		fclose(file);
	}
    
    begin = now();
    
    rbuff = image[0];
    
    
    updated_values = mask_operation (rbuff, N, send_count, task_id, g_send_count);
    
    
    
    resultImg = updated_values;
    
    end = now();
    time_spent = tdiff(begin, end);
        printf ("\n");
        printf("total time (time_spent): %.8f sec\n", time_spent);
        printf ("\n");
		printf("Junfu Cheng UFID: 77844387\nUniversity of Florida\n\n");
        
        
    
    for(int i = 0; i < MATRIX_SIZE - 2; i++)
    {
        for(int j = 0; j < MATRIX_SIZE - 2; j++)
        {
            printf("%d\t",*(resultImg+(MATRIX_SIZE - 2)*i+j));
        }
        printf("\n");
    }
    
    

    
    file = fopen("processed_matrix.txt", "w");
     
    strbuff[0] = '\0';  
    for(int i = 0; i < MATRIX_SIZE - 2 ; i++)
    {
        
        for(int j = 0; j < MATRIX_SIZE - 2 ; j++)
        {
            snprintf (buffer,10,"%d",*(resultImg+(MATRIX_SIZE - 2)*i+j));
            strcat(strbuff, buffer);
            c1 = 9;
            strncat(strbuff,&c1,1);
            // adds content to the file
            fprintf(file, "%s", strbuff);
            strbuff[0] = '\0'; 
        }
        c1 = 13;
        strncat(strbuff,&c1,1);
        c1 = 10;
        strncat(strbuff,&c1,1);
        // adds content to the file
        fprintf(file, "%s", strbuff);
        strbuff[0] = '\0';  
    }
 
    // closes the file pointed by file
    fclose(file);
    free(send_count);
    free(g_send_count);
    free(resultImg);
    
    return 0;
}

int* mask_operation (int *recv_buff, int N, int *send_count, int task_id, int* g_send_count)
{
        printf("For rank %d, part of the matrix that were sent to it", task_id);
    for(int i = 0; i < send_count[task_id]; i++)
    {
        if(i%N==0)
        {
            printf("\n");
        }
        printf("%d\t", recv_buff[i]);
    }
    printf("\n");
    
    
    int *updated_values;
    int num_elements,row_elements;
    int row, column;
    row_elements = N - 2;
    num_elements = g_send_count[task_id];
    printf("\nrow_elements = %d\n",row_elements);
    printf("\nnum_elements = %d\n",num_elements);
    updated_values = malloc(num_elements * sizeof(int));
    for(int i = 0; i < num_elements; i ++)
    {
        row = i/row_elements + 1;
        column = i%row_elements + 1;
        //Use two 3X3 Sobel masks (horizontal and vertical contrast)
        int horizontal_contrast;
        int vertical_contrast;
        horizontal_contrast = ((-1) * recv_buff[(row - 1)*N + (column - 1)]
                            + 0 * recv_buff[(row - 1)*N + (column)]
                            + 1 * recv_buff[(row - 1)*N + (column + 1)]
                            + (-2) * recv_buff[(row)*N + (column - 1)]
                            + 0 * recv_buff[(row)*N + (column)]
                            + 2 * recv_buff[(row)*N + (column + 1)]
                            + (-1) * recv_buff[(row + 1)*N + (column - 1)]
                            + 0 * recv_buff[(row + 1)*N + (column)]
                            + 1 * recv_buff[(row + 1)*N + (column + 1)]);
                            
        if( horizontal_contrast < 0)
        {
            horizontal_contrast = - horizontal_contrast;
        }
                            
        vertical_contrast = ( 1 * recv_buff[(row - 1)*N + (column - 1)]
                            + 2 * recv_buff[(row - 1)*N + (column)]
                            + 1 * recv_buff[(row - 1)*N + (column + 1)]
                            + 0 * recv_buff[(row)*N + (column - 1)]
                            + 0 * recv_buff[(row)*N + (column)]
                            + 0 * recv_buff[(row)*N + (column + 1)]
                            + (-1) * recv_buff[(row + 1)*N + (column - 1)]
                            + (-2) * recv_buff[(row + 1)*N + (column)]
                            + (-1) * recv_buff[(row + 1)*N + (column + 1)]);
        if( vertical_contrast < 0)
        {
            vertical_contrast = - vertical_contrast;
        }
                            
        updated_values[i] = horizontal_contrast + vertical_contrast;
    }
    
    
    printf("For rank %d, updated values after computation", task_id);
    for(int i = 0; i < num_elements; i ++)
    {
        if(i%row_elements == 0)
        {
            printf("\n");
        }
        printf("%d\t", updated_values[i]);
    }
    printf("\n");
    printf("\n");
    return updated_values;
}