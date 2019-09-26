//------------------------------------------------------------------------------
// The MIT License (MIT)
// 
// Copyright (c) 2015 Benjamin Sherlock
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//------------------------------------------------------------------------------
//
// FilterFactory.h
//
//------------------------------------------------------------------------------
//
// https://bensherlock.co.uk/2015/08/21/windowed-sinc-filter-factory-in-c/
//
//------------------------------------------------------------------------------
//
// Matlab fir1 function is a "Window-based FIR filter design" tool.
// Windowed-Sinc Filters
// 
// http://www.labbookpages.co.uk/audio/firWindowing.html
// https://en.wikipedia.org/wiki/Window_function#Hamming_window
// 
//------------------------------------------------------------------------------
 
#ifndef FILTERFACTORY_H
#define FILTERFACTORY_H
 
// Includes
#define Float float
 
#include <vector>
#include <cmath>
#include <vector>
//#include 
 
 
//! Window Type enum container
struct FilterWindowType
{
    typedef enum
    {
        RECTANGULAR = 0,
        BARTLETT,
        HANNING,
        HAMMING,
        BLACKMAN
    } Type;
};
 
//! Filter Type enum container
struct FilterType
{
    typedef enum
    {
        LOWPASS = 0,
        HIGHPASS,
        BANDPASS,
        BANDSTOP,
        ALLPASS
    } Type;
};
 
//! Filter Properites Holder struct
struct FilterHolderType
{
    int order;
    std::vector<Float> coefficients;
};
 
 
//! Filter Factory class
class FilterFactory
{
public:
    //! Constructor
    FilterFactory();

    //! Destructor
    virtual ~FilterFactory() {}

    //! Create Filter
    FilterHolderType createFilter(int order, 
        FilterWindowType::Type windowType, FilterType::Type filterType, 
        Float Fs = 0, Float Fc1 = 0, Float Fc2 = 0);

    //! Create Window Weights
    std::vector<Float> createWindowWeights(int order, 
        FilterWindowType::Type windowType);

    //! Create Sinc Weights
    std::vector<Float> createSincWeights(int order, 
        FilterType::Type filterType, 
        Float Fs = 0, Float Fc1 = 0, Float Fc2 = 0);

    //! Create Filter Weights - Combines Sinc and Window Weights
    std::vector<Float> createFilterWeights(int order, 
        FilterWindowType::Type windowType, FilterType::Type filterType, 
        Float Fs = 0, Float Fc1 = 0, Float Fc2 = 0);

protected:

private:
 
};

//! Constructor
FilterFactory::FilterFactory() 
{
}
 
//! Create Filter
FilterHolderType FilterFactory::createFilter(int order, 
    FilterWindowType::Type windowType, FilterType::Type filterType, 
    Float Fs /* = 0 */, Float Fc1 /* = 0 */, Float Fc2 /* = 0 */)
{
    FilterHolderType filterHolder;

    filterHolder.order = order;
    filterHolder.coefficients = createFilterWeights(order, windowType, 
    filterType, Fs, Fc1, Fc2);

    return filterHolder;
}
 
 
//! Create Window Weights
std::vector<Float> FilterFactory::createWindowWeights(int order, 
    FilterWindowType::Type windowType)
{
    // Equations from http://www.labbookpages.co.uk/audio/firWindowing.html
    // where M is the order
    switch( windowType )
    {
        case FilterWindowType::RECTANGULAR:
        {
            // w(n) = 1
            std::vector<Float> window(order+1, 1.0);
            return window;
            break;
        }

        case FilterWindowType::BARTLETT:
        {
            // w(n) = 1 - 2|n-M/2|/M
            std::vector<Float> window(order+1);

            for( int n = 0; n < (order+1); n++ )
            {
            window[n] = 1.0 
            - (2.0 * fabs( (Float)(n - (order/2)) ) / (Float)order);
            }

            return window;
            break;
        }

        case FilterWindowType::HANNING:
        {
            // w(n) = 0.5 - 0.5cos(2*PI*n/M)
            std::vector<Float> window(order+1);

            for( int n = 0; n < (order+1); n++ )
            {
                window[n] = 0.5 
                    - 0.5 * cos( 2.0 * M_PI * ((Float)n / (Float)order) );
            }

            return window;
            break;
        }

        case FilterWindowType::HAMMING:
        {
            // w(n) = 0.54 - 0.46cos(2*PI*n/M)
            std::vector<Float> window(order+1);

            for( int n = 0; n < (order+1); n++ )
            {
                // Approximation lowers levels of sidelobes - as used in matlab
                window[n] = 0.54 
                    - 0.46 * cos( 2.0 * M_PI * ((Float)n / (Float)order) ); 

                // Actual value?
                //window[n] = (25.0/46.0) 
                //- (21.0/46.0) * cos( 2.0 * M_PI * ((Float)n / (Float)order) ); 

                // Approximation to create equirriple?
                //window[n] = 0.53836 
                //- 0.46164 * cos( 2.0 * M_PI * ((Float)n / (Float)order) );
            }

            return window;
            break;
        }

        case FilterWindowType::BLACKMAN:
        {
            // w(n) = 0.42 - 0.5cos(2*PI*n/M) + 0.08cos(4*PI*n/M)
            std::vector<Float> window(order+1);

            for( int n = 0; n < (order+1); n++ )
            {
                window[n] = 0.42 
                    - 0.5 * cos( 2.0 * M_PI * ((Float)n / (Float)order) ) 
                    + 0.08 * cos( 4.0 * M_PI * ((Float)n / (Float)order) );
            }

            return window;
            break;
        }

        default: 
        {
            // w(n) = 1 - rectangular
            std::vector<Float> window(order+1, 1.0);

            return window;
            break;
        }

    }
}
 
 
//! Create Sinc Weights
std::vector<Float> FilterFactory::createSincWeights(int order, 
    FilterType::Type filterType, 
    Float Fs /* = 0 */, Float Fc1 /* = 0 */, Float Fc2 /* = 0 */)
{
    switch( filterType )
    {
        case FilterType::LOWPASS:
        {
            Float Ft = Fc1 / Fs;
            std::vector<Float> weights(order+1, 0.0);

            for( int n = 0; n < (order+1); n++ )
            {
                if( n == (order/2) )
                {
                    // Centre
                    weights[n] = 2.0 * Ft;
                }
                else
                {
                    weights[n] = sin( 2.0 * M_PI * Ft * (Float)(n - (order/2)) ) 
                        / (M_PI * (Float)(n - (order/2)));
                }
            }

            return weights;
            break;
        }

        case FilterType::HIGHPASS:
        {
            Float Ft = Fc1 / Fs;
            std::vector<Float> weights(order+1, 0.0);

            for( int n = 0; n < (order+1); n++ )
            {
                if( n == (order/2) )
                {
                    // Centre
                    weights[n] = 1.0 - (2.0 * Ft);
                }
                else
                {
                    weights[n] = -1.0 
                        * sin( 2.0 * M_PI * Ft * (Float)(n - (order/2)) ) 
                        / (M_PI * (Float)(n - (order/2)));
                }
            }

            return weights;
            break;
        }

        case FilterType::BANDPASS:
        {
            Float Ft1 = Fc1 / Fs;
            Float Ft2 = Fc2 / Fs;
            std::vector<Float> weights(order+1, 0.0);

            for( int n = 0; n < (order+1); n++ )
            {
                if( n == (order/2) )
                {
                    // Centre
                    weights[n] = 2.0 * (Ft2 - Ft1);
                }
                else
                {
                    weights[n] = sin( 2.0 * M_PI * Ft2 * (Float)(n - (order/2)) ) 
                        / (M_PI * (Float)(n - (order/2)))
                        - sin( 2.0 * M_PI * Ft1 * (Float)(n - (order/2)) ) 
                        / (M_PI * (Float)(n - (order/2)));
                }
            }

            return weights;
            break;
        }

        case FilterType::BANDSTOP:
        {
            Float Ft1 = Fc1 / Fs;
            Float Ft2 = Fc2 / Fs;
            std::vector<Float> weights(order+1, 0.0);

            for( int n = 0; n < (order+1); n++ )
            {
                if( n == (order/2) )
                {
                    // Centre
                    weights[n] = 1.0 - (2.0 * (Ft2 - Ft1));
                }
                else
                {
                    weights[n] = sin( 2.0 * M_PI * Ft1 * (Float)(n - (order/2)) ) 
                        / (M_PI * (Float)(n - (order/2)))
                        - sin( 2.0 * M_PI * Ft2 * (Float)(n - (order/2)) ) 
                        / (M_PI * (Float)(n - (order/2)));
                }
            }

        return weights;
        break;
    }
  
        case FilterType::ALLPASS:
        {
            // w(n) = 0 except at M/2 where w(n) = 1
            std::vector<Float> weights(order+1, 0.0);
            weights[order/2] = 1.0;
            return weights;
            break;
        }

        default:
        {
            // w(n) = 0 except at M/2 where w(n) = 1
            std::vector<Float> weights(order+1, 0.0);
            weights[order/2] = 1.0;
            return weights;
            break;
        }
    }
}
 
 
//! Create Filter Weights - Combines Sinc and Window Weights
std::vector<Float> FilterFactory::createFilterWeights(int order, 
    FilterWindowType::Type windowType, FilterType::Type filterType, 
    Float Fs /* = 0 */, Float Fc1 /* = 0 */, Float Fc2 /* = 0 */)
{
    std::vector<Float> window = createWindowWeights(order, windowType);
    std::vector<Float> weights = createSincWeights(order, filterType, 
                                                    Fs, Fc1, Fc2);

    // Now multiply window weights with sinc weights.
    for( int n = 0; n < (order+1); n++ )
    {
        weights[n] = window[n] * weights[n];
    }


    // Now normalise for constant gain at a specific frequency
    // http://dsp.stackexchange.com/questions/4693/fir-filter-gain
    // http://dspguru.com/dsp/faqs/fir/properties

    // H(jw) is frequency response (so calculate complex gain for a given frequency then take magnitude).

    Float FtSelected = 0.0;

    switch( filterType )
    {
        case FilterType::LOWPASS:
        case FilterType::BANDSTOP:
        {
            /*
            // Simplified version to calculate gain at DC.
            for( int n = 0; n < (order+1); n++ )
            {
            gain = gain + weights[n];
            }
            */

            FtSelected = 0.0; // DC

            break;
        }

        case FilterType::HIGHPASS:
        {
            // Select frequency above the cut frequency
            FtSelected = 0.499; // Just below Nyquist

            break;
        }

        case FilterType::BANDPASS:
        {
            // Select mid-frequency of the pass band
            Float Ft1 = Fc1 / Fs;
            Float Ft2 = Fc2 / Fs;
            FtSelected = (Ft1 + Ft2) / 2.0;

            break;
        }
    }


    // Calculate the frequency response's complex gain for the given frequency.
    Float gainReal = 0.0;
    Float gainImag = 0.0;

    for( int n = 0; n < (order+1); n++ )
    {
        gainReal = gainReal + weights[n] 
            * cos( 2.0 * M_PI * FtSelected * (Float)(n - (order/2)) );

        gainImag = gainImag - weights[n] 
            * sin( 2.0 * M_PI * FtSelected * (Float)(n - (order/2)) );
    }

    // Get the magnitude of the complex gain.
    Float gain = sqrt( gainReal * gainReal + gainImag * gainImag );


    // Now divide weights by the gain magnitude.
    for( int n = 0; n < (order+1); n++ )
    {
        weights[n] = weights[n] / gain;
    }

    return weights;
}



#endif // FILTERFACTORY_H
