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


bool isPowerOfTwo(int n) {
    return (ceil(log2(n)) == floor(log2(n)));
}

template<int numDelays>
class FeedbackMatrix
{
public:
    FeedbackMatrix(){};
    ~FeedbackMatrix(){};
    
    /// Performs the matrix multiplication
    std::array<float, numDelays> process(std::array<float, numDelays> signalIn, float feedbackGain)
    {
        assert(isPowerOfTwo(numDelays));
                
        /// create matrix
        std::array<std::array<float, numDelays>, numDelays> matrix;
        generateHadamard(matrix, numDelays);
                
        /// apply vector matrix multiplication
        std::array<float, numDelays> signalOut;
        for(size_t i = 0; i < numDelays; ++i)
        {
            for(size_t j = 0; j < numDelays; ++j)
            {
                signalOut[i] += (matrix[i][j] * signalIn[j]);
            }
        }
        
        
        /// apply feedback gain
        for (int i = 0; i < numDelays; ++i) {
            signalOut[i] *= feedbackGain;
        }
        
        return signalOut;
    }
    
private:
    void generateHadamard(std::array<std::array<float, numDelays>, numDelays>& matrix, int N) {
        if (N == 1) {
            matrix[0][0] = 1;
            return;
        }
        
        int half = N / 2;
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
