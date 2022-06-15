#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <mpi.h>
#include <queue>
using namespace std;


int main(int argc, char* argv[]) {
    int sizeArray = 4;

    for (int i = 0; i < argc; ++i) {
        if (strncmp(argv[i], "--size-array", 12) == 0) {
            sizeArray = atoi(argv[i + 1]);
            break;
        }
    }

    MPI_Init(NULL, NULL);
    int rank, total;
    // Возвращает номер процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Возвращает число процессов
    MPI_Comm_size(MPI_COMM_WORLD, &total);
    cout << "Rank: " << rank << endl;
    if (rank == 0) {
        srand(time(NULL));
        vector<int> values;
        for (int i = 0; i < sizeArray; ++i) {
            values.push_back(rand() % 100);
        }
        cout << "Values: ";
        for (int i = 0; i < sizeArray; ++i) {
            cout << values[i] << " ";
        }
        cout << endl;
        vector<int> answers;
        int current_pos = 0;
        int current_rank = 1;
        int flag = 1;
        do {
            for (int i = 1; i < total; ++i) {
                if (current_pos == values.size()){
                    values.clear();
                    break;
                }
                if (current_pos == values.size() - 1){
                    answers.push_back(values[current_pos]);
                    values.clear();
                    current_pos = 0;
                    break;
                }
                int nums[2];
                nums[0] = values[current_pos];
                nums[1] = values[current_pos + 1];
                MPI_Send(&flag, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
                MPI_Send(&nums, 2, MPI_INT, i, 1, MPI_COMM_WORLD);
                cout << "Send " << nums[0] << " " << nums[1] << " to " << i << " rank" << endl;
                current_rank = i + 1;
                current_pos += 2;
            }
            for (int i = 1; i < current_rank; ++i){
                MPI_Status status;
                int val;
                MPI_Recv(&val, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
                cout << "Recieved from " << i << " number " << val << endl;
                answers.push_back(val);
            }
            if (answers.size() == 1) {
                cout << "RESULT: " << answers[0] << endl;
                break;
            }
            else{
                if (values.size() == 0 || values.size() == 1){
                    values = answers;
                    answers.clear();
                    current_pos = 0;
                }
            }
        } while (true);
        flag = 0;
        for (int i = 1; i < total; ++i){
            MPI_Send(&flag, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        }
    }
    else {
        int input[2];
        int result;
        int have_value;
        MPI_Status status;
        while (true) {
            MPI_Recv(&have_value, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            if (!have_value)
                break;
            MPI_Recv(&input, 2, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
            result = input[0] + input[1];
            MPI_Send(&result, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);

        }
    }
    MPI_Finalize();
}