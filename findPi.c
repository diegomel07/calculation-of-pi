#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>


double calculatePi(long start, long iterations)
{
    double pi = 0.0;
    // leibniz series
    for (int i = start; i < iterations; i++)
    {   
        
        double term = (i % 2 == 0) ? 1.0 : -1.0; // The alternating sign
        term /= (2 * i + 1); // The term in the series
        pi += 4.0 * term; // Accumulate the sum
    }

    return pi;

}

FILE *adminFile(char *method)
{
    char filename[] = "childpart.txt";
    FILE *file = fopen(filename, method);

    if (file == NULL)
    {
        printf("Failed to open \n");
        return NULL;
    }

    return file;
}

void childProcess(const long ITERS_start, const long ITERS_end)
{
    // instructions for the child
    double pi = calculatePi(ITERS_start, ITERS_end);

    // results are written to a file
    FILE *fp = adminFile("w");
    fprintf(fp, "%f", pi);
    fclose(fp);
    exit(0);

}

void using2Processes(const long ITERS)
{
    double pi = 0.0;
    int status;

    // we create the child process
    pid_t pid = fork();


    if (pid == 0)
    {

        childProcess(ITERS/2, ITERS);
        
    } else {

        // instructions for the father
        
        pi = calculatePi(0, ITERS/2);
        
        wait(&status);

        // read the results of the child
        FILE *fp = adminFile("r");

        double childPart;

        if (fscanf(fp, "%lf", &childPart) == 1)
        {
            pi += childPart;
        }
        printf("%f", pi);
        fclose(fp);
    
    }

}

void using1Processes(const long ITERS)
{
    double pi = calculatePi(0, ITERS);

    printf("%f", pi);
}


void using4Processes(const long ITERS)
{
    double pi = 0;
    int status;

    pid_t pid = fork();


    if (pid == 0)
    {

        pid_t pid2 = fork();

        if (pid2 == 0)
        {

            childProcess((ITERS/4)*3, ITERS);

        } else {
            
            // instructions for the father
            pi = calculatePi((ITERS/2), (ITERS/4)*3);
            
            wait(&status);

            // read the results of the child
            FILE *fp = adminFile("r");

            double childPart;

            if (fscanf(fp, "%lf", &childPart) == 1)
            {
                pi += childPart;
            }
            
            // write the new result
            fclose(fp);
            fp = adminFile("w");
            fprintf(fp, "%f", pi);
            fclose(fp);
            exit(0);

        }


    } else {

        pid_t pid3 = fork();

        if (pid3 == 0)
        {

            childProcess((ITERS/4), ITERS/2);

        } else {
            
            // instructions for the father
            pi = calculatePi(0, ITERS/4);
            
            wait(&status);

            // read the results of the child
            FILE *fp = adminFile("r");

            double childPart;

            if (fscanf(fp, "%lf", &childPart) == 1)
            {
                pi += childPart;
            }
            
            printf("%f", pi);
            fclose(fp);

        }

    }
}

void timeSpent(void (*func)(const long), const long ITERS, char *numberProcesses)
{
    clock_t start_time, end_time;
    double cpu_time_used;

    start_time = clock();
    func(ITERS);
    end_time = clock();

    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken by %s process: %f seconds\n", numberProcesses, cpu_time_used);


}

int main()
{
    const long ITERS = 1e09;
    
    timeSpent(using1Processes, ITERS, "1");
    timeSpent(using2Processes, ITERS, "2");
    timeSpent(using4Processes, ITERS, "4");
   
    return 0;
}
