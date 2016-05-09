//
//	MonomialMatrix.h
//	krypto
//
//	Created by njhlai
//	Copyright (c) 2016 Kryptnostic. All rights reserved
//
//	Implementation of a representation of polynomials
//	Contains definition of all classes required for the polynomial representation
//

#ifndef krypto_MonomialMatrix_h
#define krypto_MonomialMatrix_h 

#include "BitMatrix.h"
#include <algorithm>

using namespace std;

/*
 * Template for MonomialMatrixChain
 * A class to represent polynomial as a chain of monomials WITHOUT CONSTANTS
 * Accessed publically 
 */
template<unsigned int NUM_INPUT, unsigned int NUM_OUTPUT>
class MonomialMatrixChain
{
public:
/* Generation */

	/*
	 * Function: zero()
	 * Returns a zero-initialized MonomialMatrixChain representing zero monomial
	 */
	static const MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> zero() {
		MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> result;
		result._node = BitMatrix<NUM_INPUT,NUM_OUTPUT>::zeroMatrix();
		result._next = NULL;
		return result;
	}

	/*
	 * Function: random()
	 * Returns a random MonomialMatrixChain representing a random polynomial
	 */
	static const MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> random() {
		MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> result;
		result._node = BitMatrix<NUM_INPUT,NUM_OUTPUT>::randomMatrix();

		unsigned int ran;
		std::fread(&ran, sizeof( unsigned int),1,urandom);
		if ((ran % 2) == 1)
		{
			result._next = new MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT>;
			*(result._next) = MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT>::random();
		} else {
			result._next = NULL;
		}

		return result;
	}

/* Operators */
	/*
	 * Operator: ()
	 * Returns the result of evaluating the monomial matrix chain with input
	 */
	const BitVector<NUM_OUTPUT> operator() (const BitVector<NUM_INPUT> & input) const {
		// RECALL: the polynomial is a homogenous degree d polynomial
		// in particular, NO CONSTANTS!!!!!!
		// so this will work even in the special case that input is a zero vector 
		// Besides, that case will only be a possible problem if the column is zero (i.e. representing 0) anyway
		// and this statement dealt with it now so all is good
		// why the hell is this comment so long anyway?
		// ... did I just lengthen the comment by commenting on how long it is?
		// ... did I just do it again?
		// ... and again...?
		// ... why can Aquaman communicate with dolphins, aren't dolphins mammals and not fish?
		// BitVector<NUM_OUTPUT> result = (input.isZero() ? BitVector<NUM_OUTPUT>::zeroVector() : BitVector<NUM_OUTPUT>::allOneVector());
		BitVector<NUM_OUTPUT> result = BitVector<NUM_OUTPUT>::allOneVector();

		for (int i = 0; i < NUM_INPUT; ++i)
		{
			if (!input.get(i)) result &= _node.getRow(i);
		}

		if (_next != NULL) result ^= (*_next)(input);

		return result;
	}

/* Access and Modification */

	/*
	 * Function: SetMonomialMatrix(M)
	 * Sets the current node to be the monomial representated by BitMatrix M
	 */
	void SetMonomialMatrix(const BitMatrix<NUM_INPUT,NUM_OUTPUT> & M) {
		_node = M;
	}

	/*
	 * Function: SetNext(aNext)
	 * Sets the _next pointer to aNext
	 */
	void SetNext(MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> * aNext) {
		_next = aNext;
	}

	/*
	 * Function: SetNext(M)
	 * Sets the next node in the chain to be the monomial representated by BitMatrix M
	 */
	void SetNext(const BitMatrix<NUM_INPUT,NUM_OUTPUT> & M) {
		_next = new MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT>;
		_next->SetMonomialMatrix(M);
		_next->SetNext(NULL);
	}

	/*
	 * Function: getMonomialMatrix()
	 * Returns the current node
	 */
	const BitMatrix<NUM_INPUT,NUM_OUTPUT> getMonomialMatrix() const {
		return _node;
	}

	/*
	 * Function: getNext()
	 * Returns the pointer to next monomials in the chain
	 */
	MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> * getNext() const {
		return _next;
	}

	/*
	 * Function: getNextNode()
	 * Returns the next node
	 */
	const BitMatrix<NUM_INPUT,NUM_OUTPUT> getNextNode() const {
		return _next->getMonomialMatrix();
	}

	/*
	 * Function: getNextMMC()
	 * Returns the MonomialMatrixChain which represents the next monomials in the chain
	 * (*getNext()) == getNextMMC()
	 */
	const MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> getNextMMC() const {
		return *_next;
	}

/* Operations on MMC */

	/*
	 * Function: append(M)
	 * Sets the next monomial in the chain to be the one represented by M
	 */
	void append(BitMatrix<NUM_INPUT,NUM_OUTPUT> & M) {
		if (_next == NULL)
		{
			MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> * newNode = new MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT>;
			newNode->SetMonomialMatrix(M);
			newNode->SetNext(NULL);
			SetNext(newNode);
			// cout << newNode << endl;
		} else {
			_next->append(M);
		}
	}

	/*
	 * Function compactify()
	 * Throws out all zero monomials and shorten the chain, if possible
	 */
	void compactify() {
		if (_next != NULL) 
		{
			_next->compactify();
			for (int i = 0; i < NUM_OUTPUT; ++i)
			{
				MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> * current = this;
				BitMatrix<NUM_INPUT,NUM_OUTPUT> currM = _node;
				MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> * theNext = _next;
				if (currM.getCol(i).isZero())
				{
					while (theNext != NULL)
					{
						BitMatrix<NUM_INPUT> nextM = theNext->getMonomialMatrix();
						BitVector<NUM_INPUT> currCol = nextM.getCol(i);

						if (!currCol.isZero())
						{
							currM.setCol(i, currCol);
							current->SetMonomialMatrix(currM);
							nextM.setCol(i, BitVector<NUM_INPUT>::zeroVector());
							theNext->SetMonomialMatrix(nextM);

							currM = nextM;
							current = theNext;
							theNext = theNext->getNext();
						} else {
							break;
						}
					}
				}
			}
		}
		deleteAllEmpty();
	}

	/*
	 * Function: subMMC(super, input)
	 * Sets the current MonomialMatrixChain to be the MonomialMatrixChain obtained by partially evaluating super with partial inputs input
	 */
	void subMMC(const MonomialMatrixChain<2*NUM_INPUT,NUM_OUTPUT> & super, const BitVector<NUM_INPUT> & input) {
		// BitVector<NUM_OUTPUT> partialResult = (input.isZero() ? BitVector<NUM_OUTPUT>::zeroVector() : BitVector<NUM_OUTPUT>::allOneVector());
		BitVector<NUM_OUTPUT> partialResult = BitVector<NUM_OUTPUT>::allOneVector();

		for (int i = 0; i < NUM_INPUT; ++i)
		{
			if (!input.get(i)) partialResult &= super.getMonomialMatrix().getRow(i);
		}

		_node = super.getMonomialMatrix().splitV2(1).andEachRow(partialResult);

		if (super.getNext() != NULL) 
		{
			MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> * theNext = new MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT>;
			*theNext = MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT>::zero();
			theNext->subMMC(*(super.getNext()), input);
			_next = theNext;
		}
	}

	/*
	 * Function: length()
	 * Returns the length of the chain
	 */
	const unsigned int length() {
		unsigned int res = 1;
		if (_next != NULL) res += _next->length();
		return res;
	}

/* Print */

	/*
	 * Function: print()
	 * Prints all the monomials in the chain as BitMatrices
	 */
	void print() {
		int n = 0;
		BitMatrix<NUM_INPUT,NUM_OUTPUT> curr = _node;
		cout << "Matrix " << n << ": " << endl;
		curr.print();
		MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> * theNext = _next;
		
		while (theNext != NULL) {
			n++;
			curr = theNext->getMonomialMatrix();
			cout << "Matrix " << n << ": " << endl;
			curr.print();
			theNext = theNext->getNext();
		}
	}

	/*
	 * Function: printLast()
	 * Prints the last monomial in the chain
	 */
	void printLast() {
		if (_next != NULL)
		{
			_next->printLast();
		} else {
			_node.print();
		}
	}

/* Garbage Collection */

	/*
	 * Function: freeAllNext()
	 * Frees all the pointers which points to next monomial in chain
	 * NOTE: In every class which contains MMC (and hence ConstantHeaderChain) as a member, the user must write the destructor of the class such that the MMC calls freeAllNext(), to prevent memory leak
	 */
	void freeAllNext() {
		// cout << "called the clean up crew!!" << endl;
		// cout << _next << endl;
		if (_next != NULL) {
			_next->freeAllNext();
			delete _next;
		}
	}

	/* 
	 * Function: deleteAllEmpty()
	 * Deletes all zero monomials
	 */
	void deleteAllEmpty() {
		if (_next != NULL) 
		{
			_next->deleteAllEmpty();
			if (_next->getMonomialMatrix() == BitMatrix<NUM_INPUT,NUM_OUTPUT>::zeroMatrix())
			{
				delete _next;
				SetNext(NULL);
			}
		}
	}

private:
	BitMatrix<NUM_INPUT,NUM_OUTPUT> _node; // the current node, which is a BitMatrix representation of a (multivariate) monomial
	MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> * _next; // pointer to the next monomial in the chain
};

/*
 * Template for ConstantChainHeader
 * A class to represent an arbitrary polynomial, with constant and a MonomialMatrixChain representing its nonconstant monomials
 * Accessed publically 
 */
template<unsigned int NUM_INPUT, unsigned int NUM_OUTPUT>
class ConstantChainHeader
{
public:
/* Generation */

	/*
	 * Function: constructChain(constant, chain)
	 * Returns the ConstantChainHeader representing a polynomial with constant portion constant, and its monomial portions represented by chain
	 */
	static const ConstantChainHeader<NUM_INPUT,NUM_OUTPUT> constructChain(const BitVector<NUM_OUTPUT> & constant, const MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> & chain) {
		ConstantChainHeader<NUM_INPUT,NUM_OUTPUT> result;
		result._headConstant = constant;
		result._mmc = chain;
		return result;
	}

	/*
	 * Function: zero()
	 * Returns a zero polynomial
	 */
	static const ConstantChainHeader<NUM_INPUT,NUM_OUTPUT> zero() {
		ConstantChainHeader<NUM_INPUT,NUM_OUTPUT> result;
		result._headConstant = BitVector<NUM_OUTPUT>::zeroVector();
		result._mmc = MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT>::zero();
		return result;
	}

	/*
	 * Function: random()
	 * Returns a random ConstantChainHeader representing a random polynomial
	 */
	static const ConstantChainHeader<NUM_INPUT,NUM_OUTPUT> random() {
		return ConstantChainHeader<NUM_INPUT,NUM_OUTPUT>::constructChain(BitVector<NUM_OUTPUT>::randomVector(), MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT>::random());
	}

/* Operators */

	/*
	 * Operator: partialEval()
	 * Partially evaluates given partial input input
	 */
	// Currently we expect 2 * PARTIAL_INPUT == NUM_INPUT
	// and that we are given (x_1, ..., x_{PARTIAL_INPUT}), i.e. the first PARTIAL_INPUT elements of x
	// If this is not the case, this becomes a little trickier...
	template<unsigned int PARTIAL_INPUT>
	const ConstantChainHeader<NUM_INPUT-PARTIAL_INPUT,NUM_OUTPUT> partialEval(const BitVector<PARTIAL_INPUT> & input) const {
		ConstantChainHeader<NUM_INPUT-PARTIAL_INPUT,NUM_OUTPUT> result;
		result.SetConstant(_headConstant);
		MonomialMatrixChain<NUM_INPUT-PARTIAL_INPUT,NUM_OUTPUT> submmc = MonomialMatrixChain<NUM_INPUT-PARTIAL_INPUT,NUM_OUTPUT>::zero();
		submmc.subMMC(_mmc, input);
		result.SetMMC(submmc);
		return result;
	}

	/*
	 * Operator: ()
	 * Returns the result of evaluating with input
	 */
	const BitVector<NUM_OUTPUT> operator() (const BitVector<NUM_INPUT> & input) const {
		return _headConstant ^ _mmc(input);
	}

/* Access & Modification */

	/*
	 * Function: SetConstant(constant)
	 * Sets the constant
	 */
	void SetConstant(const BitVector<NUM_OUTPUT> constant) {
		_headConstant = constant;
	}

	/*
	 * Function: SetMMC(mmc)
	 * Sets the nonconstant portion of the polynomial
	 */
	void SetMMC(const MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> mmc) {
		_mmc = mmc;
	}

	/*
	 * Function: getConstant()
	 * Returns the constant of the polynomial
	 */
	const BitVector<NUM_OUTPUT> getConstant() const {
		return _headConstant;
	}

	/*
	 * Function: getMMC()
	 * Returns the nonconstant portion of the polynomial
	 */
	MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> getMMC() const {
		return _mmc;
	}

/* Operations on CCH */

	/*
	 * Function: compactify()
	 * Compactify the nonconstant portion of the polynomial
	 */
	void compactify() {
		_mmc.compactify();
	}

	/*
	 * Function: length()
	 * Returns the number of nonconstant monomials with nonzero coefficient
	 */
	const unsigned int length() {
		return _mmc.length();
	}

	/*
	 * Function: xorConstant(outsideConstant)
	 * Adds up the constant of the polynomial with outsideConstant
	 */
	void xorConstant(const BitVector<NUM_OUTPUT> outsideConstant) {
		SetConstant(_headConstant ^ outsideConstant);
	}

/* Print */

	/*
	 * Function: print()
	 * Prints out the polynomial
	 */
	void print() {
		cout << "Constant: " << endl;
		_headConstant.print();
		_mmc.print();
	}

/* Garbage Collection */

	/*
	 * WORK IN PROGRESS
	 */
	// ~ConstantChainHeader() {
	// 	cout << "called" << endl;
	// 	_mmc.freeAllNext();
	// }

private:
	BitVector<NUM_OUTPUT> _headConstant; // constant portion
	MonomialMatrixChain<NUM_INPUT,NUM_OUTPUT> _mmc; // nonconstant portion, i.e. monomials
};

/*
 * Template for LinearFactorPoly
 * A class to represent an arbitrary nonconstant polynomial as a product of its linear factors
 * Accessed by client only
 */
template<unsigned int ROWS, unsigned int COLS, unsigned int MON>
class LinearFactorPoly
{
public:
/* Generation */

	/*
	 * Function: random()
	 * Returns a random LinearFactorPoly
	 */
	static const LinearFactorPoly<ROWS,COLS,MON> random() {
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = BitMatrix<ROWS,COLS>::randomMatrix();
		}
		return result;
	}

	/*
	 * Function: randomInvertible()
	 * Returns a random LinearFactorPoly which has underlying BitMatrix which is invertible (see documentation)
	 */
	static const LinearFactorPoly<ROWS,COLS,MON> randomInvertible() { 
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = BitMatrix<ROWS,COLS>::randomInvertibleMatrix();
		}
		return result;
	}

	/*
	 * Function: generateSpecialInvertible()
	 * Returns a random LinearFactorPoly which has underlying BitMatrix which is a permutation matrix (see documentation)
	 */
	static const LinearFactorPoly<ROWS,COLS,MON> generateSpecialInvertible() {
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = BitMatrix<ROWS,COLS>::generateSparseInvertibleMatrix(1);
		}
		return result;
	}

	/*
	 * Function: zero()
	 * Returns a zero polynomial
	 */
	static const LinearFactorPoly<ROWS,COLS,MON> zero() {
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = BitMatrix<ROWS,COLS>::zeroMatrix();
		}
		return result;
	}

/* Operator */

	// /*
	//  * Operator: *
	//  * Multiplication operator, between LinearFactorPoly and LinearFactorPoly
	//  */
	// template<unsigned int ROWS1>
	// const LinearFactorPoly<ROWS1,COLS,MON> operator* (const LinearFactorPoly<ROWS1,ROWS,MON> G) const {
	// 	LinearFactorPoly<ROWS1,COLS,MON> result;

	// 	for (int i = 0; i < MON; ++i)
	// 	{
	// 		result._poly[i] = G.get(i) * _poly[i];
	// 	}

	// 	return result;
	// }

	/*
	 * Operator: *
	 * Multiplication operator, between LinearFactorPoly and BitMatrix
	 */
	template<unsigned int ROWS2>
	const LinearFactorPoly<ROWS2,COLS,MON> operator* (const BitMatrix<ROWS,ROWS2> K) const {
		LinearFactorPoly<ROWS2,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result.set(i, K.transpose() * _poly[i]);
		}
		return result;
	}

	/*
	 * Operatior: ()
	 * Evaluation operator
	 */
	const BitVector<COLS> operator() (const BitVector<ROWS> & input) const {
		BitVector<COLS> result = BitVector<COLS>::allOneVector();
		for (int i = 0; i < MON; ++i)
		{
			result &= _poly[i].tMult(input);
		}
		return result;
	}

/* Access & Modification */

	/*
	 * Function: set(n, M)
	 * Sets the nth matrix as M
	 */
	void set(const unsigned int n, const BitMatrix<ROWS,COLS> M) {
		_poly[n] = M;
	}

	/*
	 * Function: get(n)
	 * Returns the nth matrix
	 */
	const BitMatrix<ROWS,COLS> get(const unsigned int n) const {
		return _poly[n];
	}

/* Operation on LinearFactorPoly */

	/*
	 * Function: permute(f, K)
	 * Returns a K \circ f
	 * NOTE: K must be a permutation matrix
	 */
	static const LinearFactorPoly<ROWS,COLS,MON> permute(const LinearFactorPoly<ROWS,COLS,MON> & f, const BitMatrix<COLS> & K) {
		if (DEBUG) assert(K.maxnnz() == 1 && K.transpose().maxnnz() == 1);
		LinearFactorPoly<ROWS,COLS,MON> result = LinearFactorPoly<ROWS,COLS,MON>::zero();
		for (int k = 0; k < MON; ++k)
		{
			for (int i = 0; i < COLS; ++i)
			{
				int j = 0;
				while (!K.get(j, i)) j++;
				result._poly[k].setCol(j, f.get(k).getCol(i));
			}
		}
		return result;
	}

	/*
	 * Function: inv()
	 * Returns the LinearFactorPoly where the underlying matrices are the inverse of the underlying matrices of the original LinearFactorPoly (see documentation)
	 */
	const LinearFactorPoly<ROWS,COLS,MON> inv() const {
		if (DEBUG) assert(ROWS == COLS);
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = _poly[i].inv();
		}
		return result;
	}

	/*
	 * Function: stackCols(Gtop, Gbot)
	 * Returns the LinearFactorPoly obtained by concatenating underlying matrices of Gtop and Gbot side by side
	 */
	template<unsigned int COLS1, unsigned int COLS2>
	static const LinearFactorPoly<ROWS,COLS,MON> stackCols(const LinearFactorPoly<ROWS,COLS1,MON> Gtop, const LinearFactorPoly<ROWS,COLS2,MON> Gbot) {
		if (DEBUG) assert(COLS == COLS1 + COLS2);
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = BitMatrix<ROWS,COLS>::augH(Gtop.get(i), Gbot.get(i));
		}
		return result;
	}

	/*
	 * Function stackCols(Gtop, Gmid, Gbot)
	 * Returns the LinearFactorPoly obtained by concatenating underlying matrices of Gtop, Gmid, Gbot side by side
	 */
	template<unsigned int COLS1, unsigned int COLS2, unsigned int COLS3>
	static const LinearFactorPoly<ROWS,COLS,MON> stackCols(const LinearFactorPoly<ROWS,COLS1,MON> Gtop, const LinearFactorPoly<ROWS,COLS2,MON> Gmid, const LinearFactorPoly<ROWS,COLS3,MON> Gbot) {
		if (DEBUG) assert(COLS == COLS1 + COLS2 + COLS3);
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = BitMatrix<ROWS,COLS>::augH(Gtop.get(i), Gmid.get(i), Gbot.get(i));
		}
		return result;
	}

	/*
	 * Function: stackRows(Gtop, Gbot)
	 * Returns the LinearFactorPoly obtained by stacking underlying matrices of Gtop on top of of Gbot
	 * NOTE: I am sorry for such a terrible comment....
	 */
	template<unsigned int ROWS1, unsigned int ROWS2>
	static const LinearFactorPoly<ROWS,COLS,MON> stackRows(const LinearFactorPoly<ROWS1,COLS,MON> Gtop, const LinearFactorPoly<ROWS2,COLS,MON> Gbot) {
		if (DEBUG) assert(ROWS == ROWS1 + ROWS2);
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = BitMatrix<ROWS,COLS>::augV(Gtop.get(i), Gbot.get(i));
		}
		return result;
	}

	/*
	 * Function: stackRows(Gtop, Gmid, Gbot)
	 * Returns the LinearFactorPoly obtained by stacking underlying matrices of Gtop on top of of Gmid on top of of Gbot
	 * NOTE: I am sorry for such a terrible comment....
	 */
	template<unsigned int ROWS1, unsigned int ROWS2, unsigned int ROWS3>
	static const LinearFactorPoly<ROWS,COLS,MON> stackRows(const LinearFactorPoly<ROWS1,COLS,MON> Gtop, const LinearFactorPoly<ROWS2,COLS,MON> Gmid, const LinearFactorPoly<ROWS3,COLS,MON> Gbot) {
		if (DEBUG) assert(ROWS == ROWS1 + ROWS2 + ROWS3);
		LinearFactorPoly<ROWS,COLS,MON> result;
		for (int i = 0; i < MON; ++i)
		{
			result._poly[i] = BitMatrix<ROWS,COLS>::augV(Gtop.get(i), Gmid.get(i), Gbot.get(i));
		}
		return result;
	}

	/*
	 * Function: expand2(f, index)
	 * Returns [f | 0] if index = 0, [0 | f] if index = 1
	 * NOTE: index < 2
	 */
	template<unsigned int COLS1>
	static const LinearFactorPoly<ROWS,COLS,MON> expand2(const LinearFactorPoly<ROWS,COLS1,MON> f, const unsigned int index) {
		if (DEBUG) assert(COLS == 2 * COLS1);
		LinearFactorPoly<ROWS,COLS,MON> result = 
		(index == 0 ? LinearFactorPoly<ROWS,COLS,MON>::stackCols(f, LinearFactorPoly<ROWS,COLS1,MON>::zero()) :
			LinearFactorPoly<ROWS,COLS,MON>::stackCols(LinearFactorPoly<ROWS,COLS1,MON>::zero(), f));
		return result;
	}

	/*
	 * Function: expand3(f, index)
	 * Returns [f | 0 | 0] if index = 0, [0 | f | 0] if index = 1, [0 | 0 | f] if index = 2
	 * NOTE: index < 3
	 */
	template<unsigned int COLS1>
	static const LinearFactorPoly<ROWS,COLS,MON> expand3(const LinearFactorPoly<ROWS,COLS1,MON> f, const unsigned int index) {
		if (DEBUG) assert(COLS == 3 * COLS1);
		LinearFactorPoly<ROWS,COLS,MON> result = 
		(index == 0 ? LinearFactorPoly<ROWS,COLS,MON>::stackCols(f, LinearFactorPoly<ROWS,2*COLS1,MON>::zero()) : 
			(index == 1 ? LinearFactorPoly<ROWS,COLS,MON>::stackCols(LinearFactorPoly<ROWS,COLS1,MON>::zero(), f, LinearFactorPoly<ROWS,COLS1,MON>::zero()) : 
				LinearFactorPoly<ROWS,COLS,MON>::stackCols(LinearFactorPoly<ROWS,2*COLS1,MON>::zero(), f)));
		return result;
	}

	/*
	 * Function: getBinaryF(f)
	 * Returns f' = [f | f | f] needed in XOR and AND (see documentation)
	 */
	template<unsigned int ROWS1, unsigned int COLS1>
	static const LinearFactorPoly<ROWS,COLS,MON> getBinaryF(const LinearFactorPoly<ROWS1,COLS1,MON> & f) {
		if (DEBUG) assert(COLS == 3 * COLS1 && ROWS == 3 * ROWS1);

		LinearFactorPoly<ROWS1,COLS,MON> top = LinearFactorPoly<ROWS1,COLS,MON>::expand3(f, 0);
		LinearFactorPoly<ROWS1,COLS,MON> mid = LinearFactorPoly<ROWS1,COLS,MON>::expand3(f, 1);
		LinearFactorPoly<ROWS1,COLS,MON> bot = LinearFactorPoly<ROWS1,COLS,MON>::expand3(f, 2);

		return LinearFactorPoly<ROWS,COLS,MON>::stackRows(top, mid, bot);
	}

	/*
	 * Function: getUnaryF(f)
	 * Returns f' = [f | f] needed in LMM (see documentation)
	 */
	template<unsigned int ROWS1, unsigned int COLS1>
	static const LinearFactorPoly<ROWS,COLS,MON> getUnaryF(const LinearFactorPoly<ROWS1,COLS1,MON> & f) {
		if (DEBUG) assert(COLS == 2 * COLS1 && ROWS == 2 * ROWS1);

		LinearFactorPoly<ROWS1,COLS,MON> top = LinearFactorPoly<ROWS1,COLS,MON>::expand2(f, 0);
		LinearFactorPoly<ROWS1,COLS,MON> bot = LinearFactorPoly<ROWS1,COLS,MON>::expand2(f, 1);

		return LinearFactorPoly<ROWS,COLS,MON>::stackRows(top, bot);
	}

	/*
	 * Function: convertToMMChain()
	 * Returns the underlying polynomial in MonomialMatrixChain form
	 * NOTES: assumes HAMMINGWEIGHT = 2, otherwise you are on your own
	 */
	const MonomialMatrixChain<ROWS,COLS> convertToMMChain() const {
		const unsigned int SUM = 1 << MON; 
		BitMatrix<ROWS,COLS> underlyingMatrices[SUM];
		unsigned int duplicates[SUM * COLS] = {0};

		for (int i = 0; i < SUM; ++i)
		{
			underlyingMatrices[i] = BitMatrix<ROWS,COLS>::zeroMatrix();
		}
		int curr = 1;
		for (int k = 0; k < MON; ++k)
		{
			int prev = curr;
			curr *= 2;
			for (int i = 0; i < COLS; ++i)
			{
				int num = 0;
				for (int j = 0; j < ROWS; ++j)
				{
					if (_poly[k].get(j, i))
					{
						for (int l = 0; l < SUM; ++l)
						{
							if ((num == 0 ? l % curr < prev : l % curr >= prev)) 
							{
								if (underlyingMatrices[l].get(j, i)) 
								{
									duplicates[l * COLS + i]++;
								} else {
									underlyingMatrices[l].set(j, i);
								}
							}
						}
						num++;
					}
				}
			}
		}

		MonomialMatrixChain<ROWS,COLS> M = MonomialMatrixChain<ROWS,COLS>::zero();
		underlyingMatrices[0].reverse();
		M.SetMonomialMatrix(underlyingMatrices[0]);
		for (int i = 1; i < SUM; ++i)
		{
			// clean up the matrices
			for (int j = 0; j < COLS; ++j)
			{
				if (underlyingMatrices[i].getCol(j).nnz() + duplicates[i * COLS + j] < MON) 
				{
					underlyingMatrices[i].setCol(j, BitVector<ROWS>::zeroVector());
				}
			}
			if (underlyingMatrices[i] != BitMatrix<ROWS,COLS>::zeroMatrix())
			{
				underlyingMatrices[i].reverse();
				M.append(underlyingMatrices[i]);
			}
		}
		
		return M;
	}

/* Print */	

	/*
	 * Function: print()
	 * Prints LinearFactorPoly
	 */
	void print() const {
		for (int i = 0; i < MON; ++i)
		{
			cout << "Matrix " << i << ":" << endl;
			_poly[i].print();
		}
	}

private:
	BitMatrix<ROWS,COLS> _poly[MON]; // set of BitMatrices which represents each linear factor of polynomial
};

/*
 * Template for SecretPolynomial
 * A class to represent an arbitrary constant polynomial as sum of a constant and a product of its linear factors
 * Accessed by client only
 */
template<unsigned int ROWS, unsigned int COLS, unsigned int MON>
class SecretPolynomial
{
public:
/* Generation */

	/*
	 * Function: random()
	 * Returns a random polynomial
	 */
	static const SecretPolynomial<ROWS,COLS,MON> random() {
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = BitVector<COLS>::randomVector();
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::random();
		return result;
	}

	/*
	 * Function: randomInvertible()
	 * Returns a random polynomial, where the underlying matrix of the linear factors are invertible (see documentation)
	 */
	static const SecretPolynomial<ROWS,COLS,MON> randomInvertible() {
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = BitVector<COLS>::randomVector();
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::randomInvertible();
		return result;
	}

	/*
	 * Function: generateSpecialInvertible()
	 * Returns a random polynomial, where the underlying matrix of the linear factors are permutation matrix (see documentation)
	 */
	static const SecretPolynomial<ROWS,COLS,MON> generateSpecialInvertible() {
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = BitVector<COLS>::randomVector();
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::generateSpecialInvertible();
		return result;
	}

	/*
	 * Functon: zero()
	 * Returns a zero polynomial
	 */
	static const SecretPolynomial<ROWS,COLS,MON> zero() {
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = BitVector<COLS>::zeroVector();
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::zero();
		return result;
	}

/* Operators */

	/*
	 * Operator: *
	 * Multiplication operator, between SecretPolynomial and BitMatrix
	 */
	template<unsigned int ROWS1>
	const SecretPolynomial<ROWS1,COLS,MON> operator* (const BitMatrix<ROWS,ROWS1> K) const {
		SecretPolynomial<ROWS1,COLS,MON> result;
		result.setConstant(_constant);
		result.setLinearFactors(_linearFactors * K);
		return result;
	}

	/*
	 * Operator: ()
	 * Evaluation operator
	 */
	const BitVector<COLS> operator() (const BitVector<ROWS> & input) const {
		return _constant ^ _linearFactors(input);
	}

/* Access & Modifications */

	/*
	 * Function: getConstant()
	 * Returns constant
	 */
	const BitVector<COLS> getConstant() const {
		return _constant;
	}

	/*
	 * Function: getLinearFactors()
	 * Return the linear factors of the nonconstant portion of the polynomial
	 */
	const LinearFactorPoly<ROWS,COLS,MON> getLinearFactors() const {
		return _linearFactors;
	}

	/*
	 * Function: getLinearFactors(n)
	 * Return the nth linear factors of the nonconstant portion of the polynomial
	 */
	const BitMatrix<ROWS,COLS> getLinearFactors(const unsigned int n) const {
		return _linearFactors.get(n);
	}

	/*
	 * Function: setConstant(newConstant)
	 * Sets the constant as newConstant
	 */
	void setConstant(const BitVector<COLS> newConstant) {
		_constant = newConstant;
	}

	/*
	 * Function: setLinearFactors(newLF)
	 * Sets the nonconstant portion of the polynomial to be one whose linear factorisation is newLF
	 */
	void setLinearFactors(const LinearFactorPoly<ROWS,COLS,MON> newLF) {
		_linearFactors = newLF;
	}

	/*
	 * Function: setLinearFactors(n, newLF)
	 * Sets the nth linear factor of the nonconstant portion of the polynomial
	 */
	void setLinearFactors(const unsigned int n, const BitMatrix<ROWS,COLS> M) {
		_linearFactors.set(n, M);
	}

/* Operation on SecretPolynomial */

	/*
	 * Function: permute(f, K)
	 * Returns K \circ f
	 * NOTE: K has to be a permutation matrix
	 */
	static const SecretPolynomial<ROWS,COLS,MON> permute(const SecretPolynomial<ROWS,COLS,MON> & f, const BitMatrix<COLS> & K) {
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = K * f.getConstant();
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::permute(f.getLinearFactors(), K);
		return result;
	}

	/*
	 * Function: stackCols(Gtop, Gbot)
	 * Returns the SecretPolynomial obtained by concatenating underlying matrices of Gtop and Gbot side by side
	 */
	template<unsigned int COLS1, unsigned int COLS2>
	static const SecretPolynomial<ROWS,COLS,MON> stackCols(const SecretPolynomial<ROWS,COLS1,MON> Gtop, const SecretPolynomial<ROWS,COLS2,MON> Gbot) {
		if (DEBUG) assert(COLS == COLS1 + COLS2);
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = BitVector<COLS>::vCat(Gtop.getConstant(), Gbot.getConstant());
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::stackCols(Gtop.getLinearFactors(), Gbot.getLinearFactors());
		return result;
	}

	/*
	 * Function: stackCols(Gtop, Gmid, Gbot)
	 * Returns the SecretPolynomial obtained by concatenating underlying matrices of Gtop, Gmid and Gbot side by side
	 */
	template<unsigned int COLS1, unsigned int COLS2, unsigned int COLS3>
	static const SecretPolynomial<ROWS,COLS,MON> stackCols(const SecretPolynomial<ROWS,COLS1,MON> Gtop, const SecretPolynomial<ROWS,COLS2,MON> Gmid, const SecretPolynomial<ROWS,COLS3,MON> Gbot) {
		if (DEBUG) assert(COLS == COLS1 + COLS2 + COLS3);
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = BitVector<COLS>::vCat(Gtop.getConstant(), Gmid.getConstant(), Gbot.getConstant());
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::stackCols(Gtop.getLinearFactors(), Gmid.getLinearFactors(), Gbot.getLinearFactors());
		return result;
	}

	/*
	 * Function: stackRows(Gtop, Gbot)
	 * Returns the SecretPolynomial obtained by stacking underlying matrices of Gtop on top of of Gbot
	 * NOTE: I am sorry for such a terrible comment....
	 */
	template<unsigned int ROWS1, unsigned int ROWS2>
	static const SecretPolynomial<ROWS,COLS,MON> stackRows(const SecretPolynomial<ROWS1,COLS,MON> Gtop, const SecretPolynomial<ROWS2,COLS,MON> Gbot) {
		if (DEBUG) assert(ROWS == ROWS1 + ROWS2);
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = Gtop.getConstant() ^ Gbot.getConstant();
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::stackRows(Gtop.getLinearFactors(), Gbot.getLinearFactors());
		return result;
	}

	/*
	 * Function: stackRows(Gtop, Gmid, Gbot)
	 * Returns the SecretPolynomial obtained by stacking underlying matrices of Gtop on top of of Gmid on top of of Gbot
	 * NOTE: I am sorry for such a terrible comment....
	 */
	template<unsigned int ROWS1, unsigned int ROWS2, unsigned int ROWS3>
	static const SecretPolynomial<ROWS,COLS,MON> stackRows(const SecretPolynomial<ROWS1,COLS,MON> Gtop, const SecretPolynomial<ROWS2,COLS,MON> Gmid, const SecretPolynomial<ROWS3,COLS,MON> Gbot) {
		if (DEBUG) assert(ROWS == ROWS1 + ROWS2 + ROWS3);
		SecretPolynomial<ROWS,COLS,MON> result;
		result._constant = Gtop.getConstant() ^ Gmid.getConstant() ^ Gbot.getConstant();
		result._linearFactors = LinearFactorPoly<ROWS,COLS,MON>::stackRows(Gtop.getLinearFactors(), Gmid.getLinearFactors(), Gbot.getLinearFactors());
		return result;
	}

	/*
	 * Function: expand2(f, index)
	 * Returns [f | 0] if index = 0, [0 | f] if index = 1
	 * NOTE: index < 2
	 */
	template<unsigned int COLS1>
	static const SecretPolynomial<ROWS,COLS,MON> expand2(const SecretPolynomial<ROWS,COLS1,MON> f, const unsigned int index) {
		if (DEBUG) assert(COLS == 2 * COLS1);
		return (index == 0 ? SecretPolynomial<ROWS,COLS,MON>::stackCols(f, SecretPolynomial<ROWS,COLS1,MON>::zero()) :
			SecretPolynomial<ROWS,COLS,MON>::stackCols(SecretPolynomial<ROWS,COLS1,MON>::zero(), f));
	}

	/*
	 * Function: expand3(f, index)
	 * Returns [f | 0 | 0] if index = 0, [0 | f | 0] if index = 1, [0 | 0 | f] if index = 2
	 * NOTE: index < 3
	 */
	template<unsigned int COLS1>
	static const SecretPolynomial<ROWS,COLS,MON> expand3(const SecretPolynomial<ROWS,COLS1,MON> f, const unsigned int index) {
		if (DEBUG) assert(COLS == 3 * COLS1);
		return (index == 0 ? SecretPolynomial<ROWS,COLS,MON>::stackCols(f, SecretPolynomial<ROWS,2*COLS1,MON>::zero()) :
			(index == 1 ? SecretPolynomial<ROWS,COLS,MON>::stackCols(SecretPolynomial<ROWS,COLS1,MON>::zero(), f, SecretPolynomial<ROWS,COLS1,MON>::zero()) :
				SecretPolynomial<ROWS,COLS,MON>::stackCols(SecretPolynomial<ROWS,2*COLS1,MON>::zero(), f)));
	}

	/*
	 * Function: getUnaryF(f)
	 * Returns f' = [f | f] needed in LMM (see documentation)
	 */
	template<unsigned int ROWS1, unsigned COLS1>
	static const SecretPolynomial<ROWS,COLS,MON> getUnaryF(const SecretPolynomial<ROWS1,COLS1,MON> & f) {
		if (DEBUG) assert(COLS == 2 * COLS1 && ROWS == 2 * ROWS1);

		SecretPolynomial<ROWS1,COLS,MON> top = SecretPolynomial<ROWS1,COLS,MON>::expand2(f, 0);
		SecretPolynomial<ROWS1,COLS,MON> bot = SecretPolynomial<ROWS1,COLS,MON>::expand2(f, 1);

		return SecretPolynomial<ROWS,COLS,MON>::stackRows(top, bot);
	}

	/*
	 * Function: getBinaryF(f)
	 * Returns f' = [f | f | f] needed in XOR and AND (see documentation)
	 */
	template<unsigned int ROWS1, unsigned int COLS1>
	static const SecretPolynomial<ROWS,COLS,MON> getBinaryF(const SecretPolynomial<ROWS1,COLS1,MON> & f) {
		if (DEBUG) assert(COLS == 3 * COLS1 && ROWS == 3 * ROWS1);

		SecretPolynomial<ROWS1,COLS,MON> top = SecretPolynomial<ROWS1,COLS,MON>::expand3(f, 0);
		SecretPolynomial<ROWS1,COLS,MON> mid = SecretPolynomial<ROWS1,COLS,MON>::expand3(f, 1);
		SecretPolynomial<ROWS1,COLS,MON> bot = SecretPolynomial<ROWS1,COLS,MON>::expand3(f, 2);

		return SecretPolynomial<ROWS,COLS,MON>::stackRows(top, mid, bot);
	}

	/*
	 * Function: convertToPublicPolynomial()
	 * Returns the polynomial in CCH form
	 */
	const ConstantChainHeader<ROWS,COLS> convertToPublicPolynomial() const {
		return ConstantChainHeader<ROWS,COLS>::constructChain(_constant, _linearFactors.convertToMMChain());
	}

/* Print */

	/*
	 * Function: print()
	 * Prints SecretPolynomial
	 */
	void print() const {
		cout << "Constant: " << endl;
		_constant.print();
		_linearFactors.print();
	}

private:
	LinearFactorPoly<ROWS,COLS,MON> _linearFactors; // nonconstant portion of polynomial in its linear factorisation
	BitVector<COLS> _constant; // constant portion of polynomial
};

#endif