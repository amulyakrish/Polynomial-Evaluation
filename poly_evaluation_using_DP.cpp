#include <iostream>
#include <cmath>
#include <thread>
#include <vector>
#include <chrono>
using namespace std;

const int MAX_DEGREE = 1000;
const int MAX_COEFF = 5;
const int MAX_TERMS = MAX_DEGREE + 1;

//3*x^9+2*x^8-1*x^7-2*x^6+2*x^5-7*x^4+8*x^3+1*x^2-5*x^1+9;
//3+2-1-2+2-7+8+1-5+9 = 10
double coefficients[MAX_TERMS];

// Function to evaluate the polynomial for a given x
double evaluatePolynomial(double x) {
    double result = 0.0;
    for (int i = 0; i <= MAX_DEGREE; ++i) {
        result += coefficients[i] * pow(x, i);
    }
    return result;
}

// Function to evaluate a chunk of the polynomial using dynamic parallelism
void evaluatePolynomialChunk(double x, int start, int end, double& result) {
    result = 0.0;
    for (int i = start; i <= end; ++i) {
        result += coefficients[i] * pow(x, i);
    }
}

// Function to perform parallel polynomial evaluation
double parallelEvaluatePolynomial(double x, int numThreads) {
    vector<thread> threads;
    vector<double> threadResults(numThreads, 0.0);
    int termsPerThread = MAX_DEGREE / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int start = i * termsPerThread;
        int end = (i == numThreads - 1) ? MAX_DEGREE : start + termsPerThread - 1;
        threads.push_back(thread(evaluatePolynomialChunk, x, start, end, ref(threadResults[i])));
    }

    for (auto& t : threads) {
        t.join();
    }

    double result = 0.0;
    for (int i = 0; i < numThreads; ++i) {
        result += threadResults[i];
    }

    return result;
}

int main() {
    // Initialize coefficients with random values
    for (int i = 0; i <= MAX_DEGREE; ++i) {
        coefficients[i] = rand() % MAX_COEFF;
       
     //   coefficients[i] = 1;
     
    }
    
    
     //3+2-1-2+2-7+8+1-5+9
     /* coefficients[0] = 9;
      coefficients[1] = -5;
      coefficients[2] = 1;
      coefficients[3] = 8;
      coefficients[4] = -7;
      coefficients[5] = 2;
      coefficients[6] = -2;
      coefficients[7] = -1;
      coefficients[8] = 2;
      coefficients[9] = 3;*/
      
     

    double x=1.0;
 

    // Serial evaluation
    auto startSerial = chrono::high_resolution_clock::now();
    double resultSerial = evaluatePolynomial(x);
    auto endSerial = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsedSerial = endSerial - startSerial;

    // Parallel evaluation with dynamic parallelism
    int numThreads=3;
   

    auto startParallel = chrono::high_resolution_clock::now();
    double resultParallel = parallelEvaluatePolynomial(x, numThreads);
    auto endParallel = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsedParallel = endParallel - startParallel;

    // Display results and timing information
    cout << "\nSerial Result: " << resultSerial << "\n";
    cout << "Parallel Result: " << resultParallel << "\n";

    cout << "\nSerial Evaluation Time: " << elapsedSerial.count() << " seconds\n";
    cout << "Parallel Evaluation Time: " << elapsedParallel.count() << " seconds\n";

    return 0;
}
