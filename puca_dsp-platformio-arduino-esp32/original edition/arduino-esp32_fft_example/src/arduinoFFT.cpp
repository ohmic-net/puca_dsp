/*

	FFT libray
	Copyright (C) 2010 Didier Longueville
	Copyright (C) 2014 Enrique Condes

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "arduinoFFT.h"

arduinoFFT::arduinoFFT(float *vReal, float *vImag, uint16_t samples, float samplingFrequency)
{// Constructor
	this->_vReal = vReal;
	this->_vImag = vImag;
	this->_samples = samples;
	this->_samplingFrequency = samplingFrequency;
	this->_power = Exponent(samples);
}

arduinoFFT::~arduinoFFT(void)
{
// Destructor
}

uint8_t arduinoFFT::Revision(void)
{
	return(FFT_LIB_REV);
}

void arduinoFFT::Compute(uint8_t dir)
{// Computes in-place complex-to-complex FFT /
	// Reverse bits /
	uint16_t j = 0;
	for (uint16_t i = 0; i < (this->_samples - 1); i++) {
		if (i < j) {
			Swap(&this->_vReal[i], &this->_vReal[j]);
			if(dir==FFT_REVERSE)
				Swap(&this->_vImag[i], &this->_vImag[j]);
		}
		uint16_t k = (this->_samples >> 1);
		while (k <= j) {
			j -= k;
			k >>= 1;
		}
		j += k;
	}
	// Compute the FFT  /
	float c1 = -1.0;
	float c2 = 0.0;
	uint16_t l2 = 1;
	for (uint8_t l = 0; (l < this->_power); l++) {
		uint16_t l1 = l2;
		l2 <<= 1;
		float u1 = 1.0;
		float u2 = 0.0;
		for (j = 0; j < l1; j++) {
			 for (uint16_t i = j; i < this->_samples; i += l2) {
					uint16_t i1 = i + l1;
					float t1 = u1 * this->_vReal[i1] - u2 * this->_vImag[i1];
					float t2 = u1 * this->_vImag[i1] + u2 * this->_vReal[i1];
					this->_vReal[i1] = this->_vReal[i] - t1;
					this->_vImag[i1] = this->_vImag[i] - t2;
					this->_vReal[i] += t1;
					this->_vImag[i] += t2;
			 }
			 float z = ((u1 * c1) - (u2 * c2));
			 u2 = ((u1 * c2) + (u2 * c1));
			 u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == FFT_FORWARD) {
			c2 = -c2;
		}
		c1 = sqrt((1.0 + c1) / 2.0);
	}
	// Scaling for reverse transform /
	if (dir != FFT_FORWARD) {
		for (uint16_t i = 0; i < this->_samples; i++) {
			 this->_vReal[i] /= this->_samples;
			 this->_vImag[i] /= this->_samples;
		}
	}
}

void arduinoFFT::ComplexToMagnitude()
{ // vM is half the size of vReal and vImag
	for (uint16_t i = 0; i < this->_samples; i++) {
		this->_vReal[i] = sqrt(sq(this->_vReal[i]) + sq(this->_vImag[i]));
	}
}

void arduinoFFT::DCRemoval()
{
	// calculate the mean of vData
	float mean = 0;
	for (uint16_t i = 1; i < ((this->_samples >> 1) + 1); i++)
	{
		mean += this->_vReal[i];
	}
	mean /= this->_samples;
	// Subtract the mean from vData
	for (uint16_t i = 1; i < ((this->_samples >> 1) + 1); i++)
	{
		this->_vReal[i] -= mean;
	}
}

void arduinoFFT::Windowing(uint8_t windowType, uint8_t dir)
{// Weighing factors are computed once before multiple use of FFT
// The weighing function is symetric; half the weighs are recorded
	float samplesMinusOne = (float(this->_samples) - 1.0);
	for (uint16_t i = 0; i < (this->_samples >> 1); i++) {
		float indexMinusOne = float(i);
		float ratio = (indexMinusOne / samplesMinusOne);
		float weighingFactor = 1.0;
		// Compute and record weighting factor
		switch (windowType) {
		case FFT_WIN_TYP_RECTANGLE: // rectangle (box car)
			weighingFactor = 1.0;
			break;
		case FFT_WIN_TYP_HAMMING: // hamming
			weighingFactor = 0.54 - (0.46 * cos(twoPi * ratio));
			break;
		case FFT_WIN_TYP_HANN: // hann
			weighingFactor = 0.54 * (1.0 - cos(twoPi * ratio));
			break;
		case FFT_WIN_TYP_TRIANGLE: // triangle (Bartlett)
			weighingFactor = 1.0 - ((2.0 * abs(indexMinusOne - (samplesMinusOne / 2.0))) / samplesMinusOne);
			break;
		case FFT_WIN_TYP_NUTTALL: // nuttall
			weighingFactor = 0.355768 - (0.487396 * (cos(twoPi * ratio))) + (0.144232 * (cos(fourPi * ratio))) - (0.012604 * (cos(sixPi * ratio)));
			break;
		case FFT_WIN_TYP_BLACKMAN: // blackman
			weighingFactor = 0.42323 - (0.49755 * (cos(twoPi * ratio))) + (0.07922 * (cos(fourPi * ratio)));
			break;
		case FFT_WIN_TYP_BLACKMAN_NUTTALL: // blackman nuttall
			weighingFactor = 0.3635819 - (0.4891775 * (cos(twoPi * ratio))) + (0.1365995 * (cos(fourPi * ratio))) - (0.0106411 * (cos(sixPi * ratio)));
			break;
		case FFT_WIN_TYP_BLACKMAN_HARRIS: // blackman harris
			weighingFactor = 0.35875 - (0.48829 * (cos(twoPi * ratio))) + (0.14128 * (cos(fourPi * ratio))) - (0.01168 * (cos(sixPi * ratio)));
			break;
		case FFT_WIN_TYP_FLT_TOP: // flat top
			weighingFactor = 0.2810639 - (0.5208972 * cos(twoPi * ratio)) + (0.1980399 * cos(fourPi * ratio));
			break;
		case FFT_WIN_TYP_WELCH: // welch
			weighingFactor = 1.0 - sq((indexMinusOne - samplesMinusOne / 2.0) / (samplesMinusOne / 2.0));
			break;
		}
		if (dir == FFT_FORWARD) {
			this->_vReal[i] *= weighingFactor;
			this->_vReal[this->_samples - (i + 1)] *= weighingFactor;
		}
		else {
			this->_vReal[i] /= weighingFactor;
			this->_vReal[this->_samples - (i + 1)] /= weighingFactor;
		}
	}
}

float arduinoFFT::MajorPeak()
{
	float maxY = 0;
	uint16_t IndexOfMaxY = 0;
	//If sampling_frequency = 2 * max_frequency in signal,
	//value would be stored at position samples/2
	for (uint16_t i = 1; i < ((this->_samples >> 1) + 1); i++) {
		if ((this->_vReal[i-1] < this->_vReal[i]) && (this->_vReal[i] > this->_vReal[i+1])) {
			if (this->_vReal[i] > maxY) {
				maxY = this->_vReal[i];
				IndexOfMaxY = i;
			}
		}
	}
	float delta = 0.5 * ((this->_vReal[IndexOfMaxY-1] - this->_vReal[IndexOfMaxY+1]) / (this->_vReal[IndexOfMaxY-1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY+1]));
	float interpolatedX = ((IndexOfMaxY + delta)  * this->_samplingFrequency) / (this->_samples-1);
	if(IndexOfMaxY==(this->_samples >> 1)) //To improve calculation on edge values
		interpolatedX = ((IndexOfMaxY + delta)  * this->_samplingFrequency) / (this->_samples);
	// returned value: interpolated frequency peak apex
	return(interpolatedX);
}

void arduinoFFT::MajorPeak(float *f, float *v)
{
	float maxY = 0;
	uint16_t IndexOfMaxY = 0;
	//If sampling_frequency = 2 * max_frequency in signal,
	//value would be stored at position samples/2
	for (uint16_t i = 1; i < ((this->_samples >> 1) + 1); i++) {
		if ((this->_vReal[i - 1] < this->_vReal[i]) && (this->_vReal[i] > this->_vReal[i + 1])) {
			if (this->_vReal[i] > maxY) {
				maxY = this->_vReal[i];
				IndexOfMaxY = i;
			}
		}
	}
	float delta = 0.5 * ((this->_vReal[IndexOfMaxY - 1] - this->_vReal[IndexOfMaxY + 1]) / (this->_vReal[IndexOfMaxY - 1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY + 1]));
	float interpolatedX = ((IndexOfMaxY + delta)  * this->_samplingFrequency) / (this->_samples - 1);
	if (IndexOfMaxY == (this->_samples >> 1)) //To improve calculation on edge values
		interpolatedX = ((IndexOfMaxY + delta)  * this->_samplingFrequency) / (this->_samples);
	// returned value: interpolated frequency peak apex
	*f = interpolatedX;
	*v = abs(this->_vReal[IndexOfMaxY - 1] - (2.0 * this->_vReal[IndexOfMaxY]) + this->_vReal[IndexOfMaxY + 1]);
}

uint8_t arduinoFFT::Exponent(uint16_t value)
{
	// Calculates the base 2 logarithm of a value
	uint8_t result = 0;
	while (((value >> result) & 1) != 1) result++;
	return(result);
}

// Private functions

void arduinoFFT::Swap(float *x, float *y)
{
	float temp = *x;
	*x = *y;
	*y = temp;
}
