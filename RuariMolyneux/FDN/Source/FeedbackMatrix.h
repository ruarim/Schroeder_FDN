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

template<int numDelays>
class FeedbackMatrix
{
public:
    FeedbackMatrix() = default;
    ~FeedbackMatrix() = default;
    
    /// apply the feedback matrix to the vector via vector matrix multiplication
    std::array<float, numDelays> process(std::array<float, numDelays> signalIn, float feedbackGain)
    {
        assert(isPowerOfTwo(numDelays));
        
        std::array<float, numDelays> signalOut;
        
        /// apply vector matrix multiplication
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
    
    /// create the feedback matrix
    void init(){
        generateHadamard(matrix, numDelays);
    }
    
private:
    std::array<std::array<float, numDelays>, numDelays> matrix;
    
    /// generate a Hadamard matrix
    void generateHadamard(std::array<std::array<float, numDelays>, numDelays>& matrix, int N) {
        if (N == 1) {
            matrix[0][0] = 1;
            return;
        }
        
        int half = N / 2;
        generateHadamard(matrix, half);
        
        /// copy top-left quadrant to other quadrants
        for (int i = 0; i < half; ++i) {
            for (int j = 0; j < half; ++j) {
                matrix[i + half][j] = matrix[i][j]; /// bottom-left
                matrix[i][j + half] = matrix[i][j]; /// top-right
                matrix[i + half][j + half] = -matrix[i][j]; /// bottom-right
            }
        }
    }
        
    bool isPowerOfTwo(int n) {
        return (ceil(log2(n)) == floor(log2(n)));
    }
};
