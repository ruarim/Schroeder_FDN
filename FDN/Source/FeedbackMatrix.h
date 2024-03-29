/*
  ==============================================================================

    FeedbackMatrix.h
    Created: 29 Mar 2024 12:25:10pm
    Author:  Ruari Molyneux

  ==============================================================================
*/

#pragma once
#include <vector>
#include <cmath>

#include <cmath>

bool isPowerOfTwo(int n) {
    return (ceil(log2(n)) == floor(log2(n)));
}

class FeedbackMatrix
{
public:
    FeedbackMatrix(){};
    ~FeedbackMatrix(){};
    
    /// Performs the scattering operation - takes values from delays in - scatters - outputs scattered
    std::vector<float> process(std::vector<float> signalIn, int numDelays)
    {
        if (!isPowerOfTwo(numDelays)) {
                throw std::invalid_argument("numDelays must be a power of 2.");
        }
        
        /// create matrix
        std::vector<std::vector<float>> matrix(numDelays, std::vector<float>(numDelays, 0.0));
        generateHadamard(matrix, numDelays);
        
        float g = 1.0f; // feedback dampening
        
        /// apply vector matrix multiplication
        std::vector<float> signalOut(numDelays, 0.0f);
        for(size_t i = 0; i < numDelays; ++i)
        {
            for(size_t j = 0; j < numDelays; ++j)
            {
                signalOut[i] += (matrix[i][j] * signalIn[j]) * g;
            }
        }
        
        /// return result
        return signalOut;
    }
    
private:
    void generateHadamard(std::vector<std::vector<float>>& matrix, int n) {
        if (n == 1) {
            matrix[0][0] = 1;
            return;
        }
        
        int half = n / 2;
        generateHadamard(matrix, half);
        
        /// Copy top-left quadrant to other quadrants
        for (int i = 0; i < half; ++i) {
            for (int j = 0; j < half; ++j) {
                matrix[i + half][j] = matrix[i][j]; /// Bottom-left
                matrix[i][j + half] = matrix[i][j]; /// Top-right
                matrix[i + half][j + half] = -matrix[i][j]; /// Bottom-right
            }
        }
    }
};
