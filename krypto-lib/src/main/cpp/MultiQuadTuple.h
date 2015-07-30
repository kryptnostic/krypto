#ifndef __krypto__MultiQuadTuple__
#define __krypto__MultiQuadTuple__

#include "BitMatrix.h"

using namespace std;
template<unsigned int NUM_INPUTS, unsigned int NUM_OUTPUTS>
class MultiQuadTuple {
public:
	//f(x) = A^T\tilde{x} (did not do A\tilde{x} because if that's the case we cannot assure that 
	//the number of cols in A has a multiple of 64; in particular, 64|A.numCols() iff 2|NUM_INPUTS)
	MultiQuadTuple(const BitMatrix<NUM_OUTPUTS> & contributions):
	_contributions(contributions){
	}

	//contributions A =  F_2{(n_i+1)C2 \times n_o}, so that f(x) = A^T\tilde{x}
	const static MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> randomMultiQuadTuple(){
		MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> f(BitMatrix<NUM_OUTPUTS>::randomMatrix(monomialCount));
		return f;
	}

	static const unsigned int getMonomialCount() {
		return monomialCount;
	}

	const BitMatrix<NUM_OUTPUTS> getPaddedContribution() const{
		if(!needPadding) return _contributions;
		return BitMatrix<NUM_OUTPUTS>::aug_v(_contributions, BitMatrix<NUM_OUTPUTS>::zeroMatrix(32));
	}

    const BitVector<NUM_OUTPUTS> operator()(const BitVector<NUM_INPUTS> & input) const {
		const unsigned int TRANSFORMED_INPUT = paddedMonomialCount >> 6;
		BitVector<TRANSFORMED_INPUT> t = BitVector<TRANSFORMED_INPUT>::zeroVector();
		int count = 0;
		for(int i = 0; i < numInputBits; ++i){ //building (padded) \tilde{x}
			t.set(count, input[i]); //x_i
			++count;
			for(int j = i+1; j < numInputBits; ++j){
				t.set(count, input[i]^input[j]);
				++count;
			}
		}
		BitMatrix<NUM_OUTPUTS> paddedContributions = getPaddedContribution();
		return paddedContributions.tMult(t); 
	}

	/**
	 * Fuctional tuple evaluation composed with left matrix multiplication
	 * C, fC in F_2^{paddedMonomialCount \times numOutputBits}
	 * Example usage:
	 * MultiQuadTuple<2, 3> f = MultiQuadTuple<2, 3>::randomMultiQuadTuple();
	 * BitMatrix<2> C = BitMatrix<129>::randomMatrix(2<<6);
	 * Usage: MultiQuadTuple<2, 3> g = f*C; //not using f(C) to avoid confusion with contribution matrix input variable
	 * This means: g(x) = f(C(x))
	 */
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> operator*(const BitMatrix<NUM_OUTPUTS> & C) const{
		const unsigned int NUM_MONOMIALS = paddedMonomialCount >> 6; //dimension for the matrix CC := \mathcal{C}
		BitMatrix<NUM_MONOMIALS> CC_T = BitMatrix<NUM_MONOMIALS>::squareZeroMatrix(); 
		//Warning: Untested!
		//constructs the padded version of \mathcal{C}^T section 3.2.1 in implementation.pdf 
		int count_i = 0;
		for(int i = 0; i < numInputBits; ++i){
			for(int ii = i; ii < numInputBits; ++ii){
				int count_j = 0;
				for(int j = 0; j < numInputBits; ++j){
					for(int jj = j; jj < numInputBits; ++jj){
						if(i == ii){
							if(j == jj){
								CC_T.set(count_j, count_i, C.get(i, j)); 
							} //else the bit is zero
						} else {
							if(j == jj){
								CC_T.set(count_j, count_i, C.get(i, j) & C.get(ii, j));
							} else {
								CC_T.set(count_j, count_i, (C.get(i, j) & C.get(ii, jj)) ^ (C.get(i, jj) ^ C.get(ii, j)));
							}
						}
						++count_j;
					}
				}
			}
			++count_i;
		}
		BitMatrix<NUM_OUTPUTS> AA_T = getPaddedContribution(); //this is actually A^T
		BitMatrix<NUM_OUTPUTS> AACC_T = CC_T * AA_T; //this should be (AC)^T = C^TA^T
		MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> g(CC_T*AA_T); //TODO:just to be careful, check the padding implementation again
		return g;
	}

	/**
	 * Composition from the right, TODO: make this more user intuitive
	 * f.rMult(C) := Cf(*)
	 * //TODO: Figure out how to make PADDED_INPUTS const
	 */
	const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> rMult(const BitMatrix<NUM_OUTPUTS> & C) {
		assert(NUM_OUTPUTS << 6 == C.colCount());
		const unsigned int PADDED_INPUTS = C.rowCount() >> 6;//paddedMonomialCount >> 6;
		//BitMatrix<PADDED_INPUTS> Ct = C.T<PADDED_INPUTS>();
		BitMatrix<NUM_OUTPUTS> Cf = getPaddedContribution() * C; //C -> Ct, HOW?
		MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS> g(Cf);
		return g;                     
	}

	/**
	 * Vertically concatenates 2 function tuples
	 * MultiQuadTuple<1,1> f1 = MultiQuadTuple<1,1>::randomMultiQuadTuple();
	 * MultiQuadTuple<1,2> f2 = MultiQuadTuple<1,1>::randomMultiQuadTuple();
	 * MultiQuadTuple<1,3> f = aug_v(f1,f2);
	 */
	template<unsigned int NUM_OUTPUTS1, unsigned int NUM_OUTPUTS2>
	static const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1+NUM_OUTPUTS2> aug_v
	(const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS1> & f1, const MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS2> & f2){
		BitMatrix<NUM_OUTPUTS1> C1 = f1.getPaddedContribution();
		BitMatrix<NUM_OUTPUTS2> C2 = f2.getPaddedContribution();
		const int NUM_OUTPUTS_SUM = NUM_OUTPUTS1 + NUM_OUTPUTS2;
		BitMatrix<NUM_OUTPUTS_SUM> C = BitMatrix<NUM_OUTPUTS_SUM>::aug_h(C1, C2);
		MultiQuadTuple<NUM_INPUTS, NUM_OUTPUTS_SUM> f(C);
		return f;
	}

private:
	BitMatrix<NUM_OUTPUTS> _contributions;
	static const bool needPadding = NUM_INPUTS & 1;
	static const unsigned int numInputBits = (NUM_INPUTS << 6);
	static const unsigned int numOutputBits = (NUM_OUTPUTS << 6);
	static const unsigned int monomialCount = ((numInputBits * (numInputBits + 1)) >> 1);
	static const unsigned int paddedMonomialCount = needPadding ? monomialCount + 32 : monomialCount;
};

#endif