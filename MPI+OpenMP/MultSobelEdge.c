//Lab 4 Part 2: Sobel Edge Detection with MPI and OpenMP
//Junfu Cheng UFID: 77844387
//University of Florida

#include "mpi.h"
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
#define MASTER 0	/* task_id of first task*/
#define FROM_MASTER 1	/* setting a message type*/
#define FROM_WORKER 2	/* setting a message type*/

//clock_t begin, end;
struct timespec begin, end;//junfu.cheng revised
double time_spent;

int* distribute_data (int *A, int N, int* send_count, int* displs, int task_id);
int* mask_operation (int *recv_buff, int N, int *send_count, int task_id , int *g_send_count);
void collect_results (int *updated_buff, int N, int *Ap, int *g_send_count, int *g_displs, int task_id);

int main(int argc, char* argv[])
{
    int image[MATRIX_SIZE][MATRIX_SIZE];
    int* resultImg;
    int N = MATRIX_SIZE;
    
    FILE *file;
    int c;
	char c1;
	char strbuff[100] = {0};
    char buffer [10];
    
    int num_tasks;
    int task_id;
    int *rbuff;
    int *updated_values;
    int *send_count;
    int *displs;
    int *g_send_count;
    int *g_displs;

    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
	MPI_Comm_size(MPI_COMM_WORLD, &num_tasks);

    
    send_count = (int*)malloc(num_tasks*sizeof(int));
    displs = (int*)malloc(num_tasks*sizeof(int));
    // calculate send counts and displacements
    for(int i = 0; i < num_tasks; i ++)
    {
        send_count[i] = (i < (N-2)%num_tasks) ? (N-2)/num_tasks + 1 : (N-2)/num_tasks;
        displs[i] = (i == 0) ? 1 : displs[i-1] + send_count[i-1];
    }
    for(int i = 0; i < num_tasks; i ++)
    {
        send_count[i] = send_count[i] + 2;
        displs[i] = displs[i] - 1;
        send_count[i] = send_count[i] * N;
        displs[i] = displs[i] * N;
    }
    g_send_count  = (int*)malloc(num_tasks*sizeof(int));
    g_displs = (int*)malloc(num_tasks*sizeof(int));
    for(int i = 0; i < num_tasks; i++)
    {
        g_send_count[i] = (N-2) * (send_count[i]/N - 2);
        g_displs[i] = (i == 0) ? 0 : (g_displs[i - 1] + g_send_count[i - 1]);
    }
    
    if(task_id == MASTER)
    {
        printf("send_counts[0]-send_count[%d] for the scatterv() function\n", num_tasks - 1);
        for (int i = 0; i < num_tasks; i ++)
        {
            printf("%d\t", send_count[i]);
        }
        printf("\n");
        printf("displs[0]-displs[%d] for the scatterv() function\n", num_tasks - 1);
        for (int i = 0; i < num_tasks; i ++)
        {
            printf("%d\t", displs[i]);
        }
        printf("\n");
        printf("send_counts[0]-send_count[%d] for the gatherv() function\n", num_tasks - 1);
        for (int i = 0; i < num_tasks; i ++)
        {
            printf("%d\t", g_send_count[i]);
        }
        printf("\n");
        printf("displs[0]-displs[%d] for the gatherv() function\n", num_tasks - 1);
        for (int i = 0; i < num_tasks; i ++)
        {
            printf("%d\t", g_displs[i]);
        }
        printf("\n");
        resultImg = (int*)malloc( (N-2) * (N-2) * sizeof(int));
    
    
    
    
    
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

    }
    
    rbuff = distribute_data( image[0], N, send_count, displs, task_id);
    
    updated_values = mask_operation (rbuff, N, send_count, task_id, g_send_count);
    
    collect_results (updated_values, N, resultImg, g_send_count, g_displs, task_id);
    
    
    if(task_id == MASTER)
	{
        
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
        free(resultImg);
    }
    MPI_Finalize();
    free(rbuff);
    free(send_count);
    free(displs);
    free(updated_values);
    free(g_send_count);
    free(g_displs);
    
    return 0;
}

int* distribute_data (int *A, int N, int* send_count, int* displs, int task_id)
{
    int *rbuff;
    rbuff = (int*)malloc(send_count[task_id] * sizeof(int));
    // divide the data among processes as described by sendcounts and displs
    MPI_Scatterv(A, send_count, displs, MPI_INT, rbuff, send_count[task_id], MPI_INT, MASTER, MPI_COMM_WORLD);
    return rbuff;
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
    int nthreads, tid;
    row_elements = N - 2;
    num_elements = g_send_count[task_id];
    printf("\nrow_elements = %d\n",row_elements);
    printf("\nnum_elements = %d\n",num_elements);
    updated_values = malloc(num_elements * sizeof(int));
    #pragma omp parallel shared (recv_buff, N, updated_values, num_elements, row_elements, g_send_count, nthreads) private ( tid,  row, column)
    {
        #pragma omp for
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

void collect_results (int *updated_buff, int N, int *Ap, int *g_send_count, int *g_displs, int task_id)
{
    MPI_Gatherv(updated_buff, g_send_count[task_id], MPI_INT, Ap, g_send_count, g_displs, MPI_INT, MASTER, MPI_COMM_WORLD); 
}