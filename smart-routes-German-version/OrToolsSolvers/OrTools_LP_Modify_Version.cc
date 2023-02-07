#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <fstream>
#include "../or-tools/ortools/linear_solver/linear_solver.h"

namespace operations_research
{
    void mipModel(int N, int p, int Q, std::string solver_name, std::string dist_file, std::string cap_file)
    {
        unsigned int start_time = clock();
        
        // Чтение из файла весов грузов

        std::vector<int> d(N+1, 0);
        std::vector<int> del_point;
        std::ifstream cap_in(cap_file);
        std::string line_cap;
        int idx;
        if (cap_in.is_open())
        {
            idx = 1;
            while (getline(cap_in, line_cap))
            {
                d[idx] = atof(line_cap.c_str());
                if(d[idx] > Q){
                    d[idx] = 0;
                    del_point.push_back(idx);
                }
                idx++;
            }
        }

        cap_in.close();
        
        // Чтение из файла матрицы расстояний
        
        std::ifstream in(dist_file);
        std::vector<std::vector<double>> dist_arr(N + 1, std::vector<double>(N + 1, 0));
        std::string line;
        if (in.is_open())
        {
            while (getline(in, line))
            {
                dist_arr[line[0]-'0'][line[2]-'0'] = atof(line.substr(5).c_str());
            }
        }

        in.close();

        //Вычисление расстояний по обновленной формуле из статьи
        std::vector<std::vector<double>> new_matrix(N + 1, std::vector<double>(N + 1, 0));
        for (int i = 1; i < N + 1; i++)
        {
            if(std::find(del_point.begin(), del_point.end(), i) == del_point.end())
            {
                for (int j = 1; j < N + 1; j++)
                {
                    if (i != j && std::find(del_point.begin(), del_point.end(), j) == del_point.end())
                    {
                        new_matrix[i][j] = dist_arr[i][0] + dist_arr[0][j] - dist_arr[i][j];
                        new_matrix[j][i] = new_matrix[i][j];
                    }
                }
            }
        }

        // Создание модели
        std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver(solver_name));
        if (!solver)
        {
            LOG(WARNING) << "SCIP solver unavailable.";
            return;
        }

        std::vector<std::vector<const MPVariable*>> x(N + 1, std::vector<const MPVariable*>(N + 1, 0));
        std::vector<const MPVariable*> y(N + 1, 0);

        int count1 = 0;
        for (int i = 0; i < N + 1; i++)
        {
            for (int j = 0; j < N + 1; j++)
            {
                x[i][j] = solver->MakeIntVar(0, 1, "");
                count1++;    
            }
        }

        int count2 = 0;
        for (int i = 1; i < N+1; i++)
        {
            y[i] = solver->MakeIntVar(1, 42, "");
            count2++;
        }

        // 9
        int count3 = 0;
        LinearExpr constraint9;
        for (int j = 1; j < N + 1; j++)
        {
            if(std::find(del_point.begin(), del_point.end(), j) == del_point.end())
            {
                constraint9 += x[0][j];
                count3++;
            }
        }
        solver->MakeRowConstraint(constraint9 == p);

        // 10
        int count4 = 0;
        for(int j = 1; j < N+1; j++)
        {
            if(std::find(del_point.begin(), del_point.end(), j) == del_point.end())
            {
                LinearExpr constraint10;
                for(int i = 0; i < N+1; i++)
                {
                    if(i != j && std::find(del_point.begin(), del_point.end(), i) == del_point.end())
                    {
                        constraint10 += x[i][j];
                        count4++;
                    }
                }
                solver->MakeRowConstraint(constraint10 == 1);
            }
        }

        // 11
        int count5 = 0;
        for(int i = 1; i < N+1; i++)
        {
            if(std::find(del_point.begin(), del_point.end(), i) == del_point.end())
            {
                LinearExpr constraint11;
                for(int j = 1; j < N+1; j++)
                {
                    if(i != j && std::find(del_point.begin(), del_point.end(), j) == del_point.end())
                    {
                        constraint11 += x[i][j];
                        count5++;
                    }
                }
                solver->MakeRowConstraint(constraint11 <= 1);
            }
        }

        // 12
        int count6 = 0;
        const double infinity = solver->infinity();
        for(int i = 1; i < N+1; i++)
        {
            if(std::find(del_point.begin(), del_point.end(), i) == del_point.end())
            {
                for(int j = 1; j < N+1; j++)
                {
                    if(i != j && std::find(del_point.begin(), del_point.end(), j) == del_point.end())
                    {
                        int sum_demand = d[j] + Q;
                        // -inf <= y[i] * 1 + y[j] * (-1) + x[i][j] * d[j] + x[i][j] * Q <= Q
                        MPConstraint* constraint12 = solver->MakeRowConstraint(-infinity, Q, "");
                        constraint12->SetCoefficient(y[i], 1);
                        constraint12->SetCoefficient(y[j], -1);
                        constraint12->SetCoefficient(x[i][j], sum_demand);
                        count6++;
                    }
                }
            }
        }

        // 13
        int count7 = 0;
        for(int i = 1; i < N+1; i++)
        {
            if(std::find(del_point.begin(), del_point.end(), i) == del_point.end())
            {
                MPConstraint* constraint13 = solver->MakeRowConstraint(d[i], Q, "");
                constraint13->SetCoefficient(y[i], 1);
                count7++;
            }
        }

        // Maximize function
        int count8 = 0;
        MPObjective *const objective = solver->MutableObjective();
        for (int i = 1; i < N + 1; i++)
        {
            if(std::find(del_point.begin(), del_point.end(), i) == del_point.end())
            {
                for (int j = 1; j < N + 1; j++)
                {
                    if (i != j && std::find(del_point.begin(), del_point.end(), j) == del_point.end())
                    {
                        objective->SetCoefficient(x[i][j], new_matrix[i][j]);
                        count8++;
                    }
                }
            }
        }
        objective->SetMaximization();

        // Печать и запись в файл решения
        const MPSolver::ResultStatus result_status = solver->Solve();
        if (result_status != MPSolver::OPTIMAL)
        {
            LOG(FATAL) << "The problem does not have an optimal solution.";
        }
        LOG(INFO) << "Solution:";
        LOG(INFO) << "Optimal objective value = " << objective->Value();

        std::ofstream file_res("result.txt", std::ios_base::out | std::ios_base::trunc);

        for (int i = 0; i < N + 1; i++)
        {
            if(std::find(del_point.begin(), del_point.end(), i) == del_point.end())
            {
                for (int j = 0; j < N + 1; j++)
                {
                    if (std::find(del_point.begin(), del_point.end(), j) == del_point.end() && x[i][j]->solution_value() > 0.9)
                    {
                        file_res << i << " " << j << std::endl;
                        LOG(INFO) << "x[" << i << "]"
                                << "["  << j << "] = " << x[i][j]->solution_value();
                    }
                }
            }
        }
        file_res.close();
        
        unsigned int end_time = clock();
        unsigned int search_time = end_time - start_time;
        std::cout << "Время работы: " << search_time / 1000000.0 << "секунд" << std::endl;

    }
}
