#ifndef __krypto__MultiQuadTuple__
#define __krypto__MultiQuadTuple__

#include "BitMatrix.h"

template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultiQuadTuple {
public:
	MultiQuadTuple(const BitMatrix<NUM_OUTPUTS> & contributionsT):_contributionsT(contributionsT){}

	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> randomMultiQuadTuple(){
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(BitMatrix<NUM_OUTPUTS>::randomMatrix(numInputMonomials));
	}

	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> zeroMultiQuadTuple(){
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(BitMatrix<NUM_OUTPUTS>::zeroMatrix(numInputMonomials));
	}

	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> getMultiQuadTuple(const BitMatrix<NUM_INPUTS> & M){
		assert(NUM_OUTPUTS << 6 == M.rowCount());
		BitMatrix<NUM_OUTPUTS> Mt = M.template T<NUM_OUTPUTS>();
		BitMatrix<NUM_OUTPUTS> result = BitMatrix<NUM_OUTPUTS>::zeroMatrix(numInputMonomials);
		size_t count = 0;
		for(size_t i = 0; i < numInputBits; i++){
			result.setRow(count, Mt.getRow(i));
			count += (numInputBits - i);
		}
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(result);
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

	template<unsigned int NUM_INNERINPUTS> 
	const MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS> operator*(const BitMatrix<NUM_INNERINPUTS> & C) const{
		assert(numInputBits == C.rowCount());
		BitMatrix<NUM_INPUTS> Ct = C.template T<NUM_INPUTS>();
		const unsigned int numInnerInputMonomials = MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>::getInputMonomialCount(); 
		const unsigned int numInnerInputBits = MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>::getInputCount();
		BitMatrix<NUM_OUTPUTS> result(numInnerInputMonomials);
		size_t input_count = 0;
		size_t subLength = numInputBits;
		for(size_t i = 0; i < numInputBits; ++i){ 
			BitMatrix<NUM_OUTPUTS> A_i = Ct.template pMult<NUM_OUTPUTS>(_contributionsT, i, numInputBits - 1, input_count, input_count + subLength - 1);
			input_count += subLength;
			--subLength;
			for(size_t j = 0; j < numInnerInputBits; ++j){
				if(C.get(i, j)){ 
					for(size_t k = 0; k < numInnerInputBits; ++k){
						size_t newIndex = MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>::getIndex(j, k); 
						result.setRow(newIndex, result.getRow(newIndex) ^ A_i.getRow(k));
					}
				}
			}
		}
		return MultiQuadTuple<NUM_INNERINPUTS, NUM_OUTPUTS>(result);
	}

	template<unsigned int NUM_OUTEROUTPUTS>
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS> rMult(const BitMatrix<NUM_OUTPUTS> & C) {
		assert(C.rowCount() == NUM_OUTEROUTPUTS << 6); 
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTEROUTPUTS>(_contributionsT * (C.template T<NUM_OUTEROUTPUTS>()));
	}

	const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> operator^(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> & rhs) const {
 		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS>(_contributionsT ^ rhs._contributionsT);
	}

	template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
	static const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1+NUM_OUTPUTS2> aug_v(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1> & f1, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2> & f2){
		BitMatrix<NUM_OUTPUTS1> C1 = f1.getTransposedContributionMatrix();
		BitMatrix<NUM_OUTPUTS2> C2 = f2.getTransposedContributionMatrix();
		const unsigned int NUM_OUTPUTS_SUM = NUM_OUTPUTS1 + NUM_OUTPUTS2;
		return MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS_SUM>(BitMatrix<NUM_OUTPUTS_SUM>::aug_h(C1, C2));
	}	

	static const unsigned int getInputCount(){
		return numInputBits;
	}

	static const unsigned int getInputMonomialCount(){
		return numInputMonomials;
	}

	static const unsigned int getIndex(unsigned int main, unsigned int aux){
		size_t index1 = min(main, aux);
		size_t index2 = max(main, aux);
		unsigned int toSubtract = (((numInputBits - index1) * (numInputBits - index1 + 1)) >> 1);
		unsigned int result = numInputMonomials - toSubtract + (index2 - index1);
		return result;
	}		

	const BitMatrix<NUM_OUTPUTS> getTransposedContributionMatrix() const{
		return _contributionsT;
	}
private:
	BitMatrix<NUM_OUTPUTS> _contributionsT;
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numInputMonomials = ((numInputBits * (numInputBits + 1)) >> 1);
};
#endif