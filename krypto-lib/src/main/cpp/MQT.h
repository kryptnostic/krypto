//the supposedly faster new version, under experiment

#ifndef __krypto__MQT__
#define __krypto__MQT__

#include "BitMatrix.h"

using namespace std;
template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MQT {
public:
	MQT(const BitMatrix<NUM_OUTPUTS> & contributionsT):_contributionsT(contributionsT){}

	const static MQT<NUM_INPUTS, NUM_OUTPUTS> rMQT(){
		return MQT<NUM_INPUTS, NUM_OUTPUTS>(BitMatrix<NUM_OUTPUTS>::randomMatrix(numInputMonomials));
	}

	const BitVector<NUM_OUTPUTS> operator()(const BitVector<NUM_INPUTS> & input) const {
		BitVector<NUM_OUTPUTS> result;
		unsigned int count = 0;
		for(size_t i = 0; i < numInputBits; ++i){
			if(input[i]){
				result ^= _contributionsT.getRow(count);
				++count;
				for(size_t j = i+1; j < numInputBits; ++j){
					if(input[j])result ^= _contributionsT.getRow(count);
					++count;
				}	
			} else count += (numInputBits - i);
		}
		return result;
	}

	const unsigned int getIndex(unsigned int main, unsigned int aux) const{
		size_t index1 = min(main, aux);
		size_t index2 = max(main, aux);
		//unsigned int toSubtract = (((numInputBits - main) * (numInputBits - main + 1)) >> 1);
		//return numInputMonomials - toSubtract + (aux - main);
		unsigned int toSubtract = (((numInputBits - index1) * (numInputBits - index1 + 1)) >> 1);
		unsigned int result = numInputMonomials - toSubtract + (index2 - index1);
		return result;
	}

	template<unsigned int NUM_INNERINPUTS> 
	const MQT<NUM_INNERINPUTS, NUM_OUTPUTS> operator*(const BitMatrix<NUM_INNERINPUTS> & C) const{
		assert(NUM_INPUTS << 6 == C.rowCount());
		BitMatrix<NUM_INPUTS> Ct = C.template T<NUM_INPUTS>();
		const unsigned int numInnerInputMonomials = MQT<NUM_INNERINPUTS, NUM_OUTPUTS>::getInputMonomialCount(); 
		const unsigned int numInnerInputBits = MQT<NUM_INNERINPUTS, NUM_OUTPUTS>::getInputCount();
		BitMatrix<NUM_OUTPUTS> result(numInnerInputMonomials);
		size_t input_count = 0;
		size_t subLength = numInputBits;
		for(size_t i = 0; i < numInputBits; ++i){ 
			BitMatrix<NUM_OUTPUTS> A_i = Ct.template pMult<NUM_OUTPUTS>(_contributionsT, i, numInputBits - 1, input_count, input_count + subLength - 1);
			assert(A_i.rowCount() == numInnerInputBits);
			assert(A_i.colCount() == numOutputBits);
			input_count += subLength;
			--subLength;
			for(size_t j = 0; j < numInnerInputBits; ++j){
				if(C.get(i, j)){ 
					for(size_t k = 0; k < numInnerInputBits; ++k){
						size_t newIndex = MQT<NUM_INNERINPUTS, NUM_OUTPUTS>::getIndex(j, k);
						result.setRow(newIndex, result.getRow(newIndex) ^ A_i.getRow(k));
					}
				}
			}
		}
		return MQT<NUM_INNERINPUTS, NUM_OUTPUTS>(result);
	}

	const unsigned int getInputCount() const{
		return numInputBits;
	}

	const unsigned int getInputMonomialCount() const{
		return numInputMonomials;
	}
private:
	BitMatrix<NUM_OUTPUTS> _contributionsT;
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numOutputBits = (NUM_OUTPUTS << 6);
	static const unsigned int numInputMonomials = ((numInputBits * (numInputBits + 1)) >> 1);
	static const unsigned int numOutputMonomials = ((numOutputBits * (numOutputBits + 1)) >> 1);
};

#endif