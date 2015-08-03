/*Accessed by client, used to generate components of the public keys*/
#ifndef krypto_BridgeKey_h
#define krypto_BridgeKey_h

#include <assert.h>
#include "PrivateKey.h"
#include "BitMatrix.h"
#include "MultiQuadTuple.h"

using namespace std;


template<unsigned int N, unsigned int L>
class BridgeKey{
public:
	BridgeKey(PrivateKey<N,L> &pk, BitMatrix<N> K) : 
	_pk(pk),
	_R(BitMatrix<N>::randomInvertibleMatrix()),
	_Rx(BitMatrix<N>::randomInvertibleMatrix()),
	_Ry(BitMatrix<N>::randomInvertibleMatrix()),
	_M(pk.getM()),
	_Cu1(pk.getUnaryObfChain()[0]),
	_Cu2(pk.getUnaryObfChain()[1]),
	_Cb1(pk.getBinaryObfChain()[0]),
	_Cb2(pk.getBinaryObfChain()[1]),
	_BKBi(pk.getB() * K * pk.getB().inv()),
	_BKBiAi(_BKBi * pk.getA().inv()),
	_ARAi(pk.getA() * _R * pk.getA().inv()),
	_ARxAi(pk.getA() * _Rx * pk.getA().inv()),
	_ARyAi(pk.getA() * _Ry * pk.getA().inv())
	{
	}

/* Unary unified code */

	const MultiQuadTuple<2*N, 2*N> get_UNARY_g1() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<2*N> M2 = _M.inv().split_v_2(1);
		BitMatrix<2*N> mat_top = _pk.getA().inv() * M2;
		BitMatrix<2*N> mat_bot = _R * mat_top;

		MultiQuadTuple<2*N, N> top = f.get(0) * mat_top;
		MultiQuadTuple<2*N, N> bot = f.get(0) * mat_bot;
		MultiQuadTuple<2*N, 2*N> aug = MultiQuadTuple<2*N, 2*N>::aug_v(top, bot);
		return aug.template rMult<2*N>(_Cu1);
	}

	const MultiQuadTuple<2*N, 2*N> get_UNARY_g2() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<2*N> mat_top = _Cu1.inv().split_v_2(0);
		BitMatrix<2*N> mat_bot = _Cu1.inv().split_v_2(1);

		MultiQuadTuple<2*N, N> top = f.get(1) * mat_top;
		MultiQuadTuple<2*N, N> bot = f.get(1) * mat_bot;
		MultiQuadTuple<2*N, 2*N> aug = MultiQuadTuple<2*N, 2*N>::aug_v(top, bot);
		return aug.template rMult<2*N>(_Cu2);
	}

/* Left Matrix Multiplication */

	const BitMatrix<4*N> get_LMM_Z() const{
		BitMatrix<N> zeroN = BitMatrix<N>::squareZeroMatrix();
		BitMatrix<N> RAi = _R * _pk.getA().inv();

		BitMatrix<2*N> X_top = BitMatrix<2*N>::aug_h(_BKBi, _BKBiAi ^ RAi);
		BitMatrix<2*N> X_bottom = BitMatrix<2*N>::aug_h(zeroN, _ARAi);
		BitMatrix<2*N> X = _M * BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();

		BitMatrix<2*N> Y_top = BitMatrix<2*N>::aug_h(_BKBi, BitMatrix<N>::squareIdentityMatrix());
		BitMatrix<2*N> Y_bottom = BitMatrix<2*N>::aug_h(zeroN, zeroN);
		BitMatrix<2*N> Y = _M * BitMatrix<2*N>::aug_v(Y_top, Y_bottom) * _Cu2.inv();
		return BitMatrix<4*N>::aug_h(X, Y);
		//move _M mult to return
	}

/* Binary unified code */

	const MultiQuadTuple<4*N, 3*N> get_BINARY_g1() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<2*N> M2 = _M.inv().split_v_2(1);
		BitMatrix<4*N> M2_projX = BitMatrix<4*N>::aug_h(M2, BitMatrix<2*N>::zeroMatrix(N << 6));
		BitMatrix<4*N> M2_projY = BitMatrix<4*N>::aug_h(BitMatrix<2*N>::zeroMatrix(N << 6), M2);

		BitMatrix<4*N> mat_top = _pk.getA().inv() * M2_projX;
		BitMatrix<4*N> mat_mid = _pk.getA().inv() * M2_projY;
		BitMatrix<4*N> mat_botX = _Rx * _pk.getA().inv() * M2_projX;
		BitMatrix<4*N> mat_botY = _Ry * _pk.getA().inv() * M2_projY;

		MultiQuadTuple<4*N, N> top = f.get(0) * mat_top;
		MultiQuadTuple<4*N, N> mid = f.get(0) * mat_mid;
		MultiQuadTuple<4*N, N> bot = f.get(0) * (mat_botX ^ mat_botY);
		MultiQuadTuple<4*N, 3*N> aug = MultiQuadTuple<4*N, 3*N>::aug_v(MultiQuadTuple<4*N, 2*N>::aug_v(top, mid), bot);
		return aug.template rMult<3*N>(_Cb1);
	}

	const MultiQuadTuple<3*N, 3*N>get_BINARY_g2() const{
		MultiQuadTupleChain<N,L> f = _pk.getf();

		BitMatrix<3*N> Cb1_inv = _Cb1.inv();
		BitMatrix<3*N> mat_top = Cb1_inv.split_v_3(0);
		BitMatrix<3*N> mat_mid = Cb1_inv.split_v_3(1);
		BitMatrix<3*N> mat_bot = Cb1_inv.split_v_3(2);

		MultiQuadTuple<3*N, N> top = f.get(1) * mat_top;
		MultiQuadTuple<3*N, N> mid = f.get(1) * mat_mid;
		MultiQuadTuple<3*N, N> bot = f.get(1) * mat_bot;
		MultiQuadTuple<3*N, 3*N> aug = MultiQuadTuple<3*N, 3*N>::aug_v(MultiQuadTuple<3*N, 2*N>::aug_v(top, mid), bot);
		return aug.template rMult<3*N>(_Cb2);
	}


/* XOR */

	const BitMatrix<2*N> get_XOR_Xx() const{
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<2*N> X_top = BitMatrix<2*N>::aug_h(idN, (idN ^ _Rx) * _pk.getA().inv());
		BitMatrix<2*N> X_bottom = BitMatrix<2*N>::aug_h(BitMatrix<N>::squareZeroMatrix(), _ARxAi);
		return _M * BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();
	}

	const BitMatrix<2*N> get_XOR_Xy() const{
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<2*N> X_top = BitMatrix<2*N>::aug_h(idN, (idN ^ _Ry) * _pk.getA().inv());
		BitMatrix<2*N> X_bottom = BitMatrix<2*N>::aug_h(BitMatrix<N>::squareZeroMatrix(), _ARyAi);
		return _M * BitMatrix<2*N>::aug_v(X_top, X_bottom) * _M.inv();
	}

	const BitMatrix<3*N> get_XOR_Y() const{
		BitMatrix<N> idN = BitMatrix<N>::squareIdentityMatrix();

		BitMatrix<3*N> Y_top = BitMatrix<3*N>::aug_h(idN, BitMatrix<2*N>::aug_h(idN, idN));
		return _M * BitMatrix<3*N>::aug_v(Y_top, BitMatrix<3*N>::zeroMatrix(N << 6)) * _Cb2.inv();
	}

/* AND */

	const MultiQuadTuple<7*N, 2*N> get_AND_z() const{
		BitMatrix<2*N> X = get_AND_X();
		BitMatrix<3*N> Y1 = get_AND_Y1();
		BitMatrix<3*N> Y2 = get_AND_Y2();
		BitMatrix<3*N> Y3 = get_AND_Y3();

		BitMatrix<N> contrib = get_AND_Pk(0, X, Y2);
		const int twoN = N << 7;
		const int threeN = 3 * (N << 6);

		for (int level = 1; level < twoN; ++level) {
			contrib = BitMatrix<N>::aug_v(contrib, get_AND_Pk(level, X, Y2)); //add P_k's
		}
		for (int level = 0; level < twoN; ++level) {
			contrib = BitMatrix<N>::aug_v(contrib, get_AND_Qk(level, X, Y1)); //add Q_k's
		}
		for (int level = 0; level < threeN; ++level) {
			contrib = BitMatrix<N>::aug_v(contrib, get_AND_Sk(level, Y1, Y2)); //add S_k's
		}

		MultiQuadTuple<7*N, N> z_top(BitMatrix<N>::aug_v(contrib, BitMatrix<N>::zeroMatrix(32)));
		MultiQuadTuple<7*N, N> zeroMQT = MultiQuadTuple<7*N, N>::zeroMultiQuadTuple();
		MultiQuadTuple<7*N, 2*N> z = MultiQuadTuple<7*N, 2*N>::aug_v(z_top, zeroMQT);
		MultiQuadTuple<7*N, 2*N> result = z.template rMult<2*N>(_M);

		return result;
	}

	const BitMatrix<2*N> get_AND_Z1() const{
		BitMatrix<2*N> M2 = _M.split_v_2(1);
		BitMatrix<2*N> top = _Rx * _pk.getA().inv() * M2;
		BitMatrix<2*N> bottom = _pk.getA() * top;
		return _M * BitMatrix<2*N>::aug_v(top, bottom);
	}

	const BitMatrix<2*N> get_AND_Z2() const{
		BitMatrix<2*N> M2 = _M.split_v_2(1);
		BitMatrix<2*N> top = _Ry * _pk.getA().inv() * M2;
		BitMatrix<2*N> bottom = _pk.getA() * top;
		return _M * BitMatrix<2*N>::aug_v(top, bottom);
	}


private:
	PrivateKey<N,L> _pk;
	BitMatrix<N> _R; //TODO: delegate the random matrix generation task to some other class?
	BitMatrix<N> _Rx;
	BitMatrix<N> _Ry;
	BitMatrix<2*N> _M;
	BitMatrix<2*N> _Cu1;
	BitMatrix<2*N> _Cu2;
	BitMatrix<3*N> _Cb1;
	BitMatrix<3*N> _Cb2;
	BitMatrix<N> _BKBi; 
	BitMatrix<N> _BKBiAi;
	BitMatrix<N> _ARAi;
	BitMatrix<N> _ARxAi;
	BitMatrix<N> _ARyAi;
	MultiQuadTupleChain<2*N,L> _g_u; //obsfucated chain for unary operations
	MultiQuadTupleChain<3*N,L> _g_b; //obsfucated chain for binary operations
	int _dim_quad = 64; //dimension of bitmatrix used to represent quadratic poly's (why isn't this N << 6 in general?)


	//can be combined into get_z
	const BitMatrix<2*N> get_AND_X() const{
		BitMatrix<2*N> inner = BitMatrix<2*N>::aug_h(BitMatrix<N>::squareIdentityMatrix(), _pk.getA().inv());
		return _pk.getB().inv() * inner * _M.inv();
	}

	const BitMatrix<3*N> get_AND_Y1() const{
		BitMatrix<3*N> Cb_top = _Cb1.inv().split_v_3(0);
		return _pk.getB() * Cb_top;
	}

	const BitMatrix<3*N> get_AND_Y2() const{
		BitMatrix<3*N> Cb_middle = _Cb1.inv().split_v_3(1);
		return _pk.getB() * Cb_middle;
	}

	const BitMatrix<3*N> get_AND_Y3() const{
		return _Cb1.inv().split_v_3(2);
	}

	//top chunk of contrib matrix for z
	//level ranges from 0 to 64 * 2N - 1
	const BitMatrix<N> get_AND_Pk(const int level, const BitMatrix<2*N> &X, const BitMatrix<3*N> &Y2) const{
		//untested!
		const int twoN = N << 7;
		const int threeN = 3 * (N << 6);
		BitMatrix<N> top = BitMatrix<N>::zeroMatrix(twoN - level);

		BitMatrix<N> mid = BitMatrix<N>::zeroMatrix(twoN);
		for (int i = 0; i < twoN; i++) { //row within middle block
			for (int j = 0; j < (N << 6); j++) { //col within middle block
				bool lhs = X.get(j, level);
				bool rhs = X.get(j, i);
				mid.set(i, j, lhs && rhs);
			}
		}

		BitMatrix<N> bot = BitMatrix<N>::zeroMatrix(threeN);
		for (int i = 0; i < threeN; i++) { //row within bottom block
			for (int j = 0; j < (N << 6); j++) { //col within bottom block
				bool lhs = X.get(j, level);
				bool rhs = Y2.get(j, i);
				bot.set(i, j, lhs && rhs);
			}
		}
		return BitMatrix<N>::aug_v(BitMatrix<N>::aug_v(top, mid), bot);
	}

	//middle chunk of contrib matrix for z
	//level ranges from 0 to 64 * 2N - 1
	const BitMatrix<N> get_AND_Qk(const int level, const BitMatrix<2*N>&X, const BitMatrix<3*N> &Y1) const{
		//untested!
		const int twoN = N << 7;
		const int threeN = 3 * (N << 6);
		BitMatrix<N> top = BitMatrix<N>::zeroMatrix(twoN - level);

		BitMatrix<N> bot = BitMatrix<N>::zeroMatrix(threeN);
		for (int i = 0; i < threeN; i++) { //row within bottom block
			for (int j = 0; j < (N << 6); j++) { //col within bottom block
				bool lhs = X.get(j, level);
				bool rhs = Y1.get(j, i);
				bot.set(i, j, lhs && rhs);
			}
		}
		return BitMatrix<N>::aug_v(top, bot);
	}

	//bottom chunk of contrib matrix for z
	//level ranges from 0 to 64 * 2N - 1
	const BitMatrix<N> get_AND_Sk(const int level, const BitMatrix<3*N> &Y1, const BitMatrix<3*N> &Y2) const{
		//to be implemented
		const int threeN = 3 * (N << 6); //should be the number of coefficients (___ choose 2)

		BitMatrix<N> contrib = BitMatrix<N>::zeroMatrix(threeN - level);
		for (int j = 0; j < N; ++j) { //cols
			bool prod = Y1.get(j, level) && Y2.get(j, level); //first row
			contrib.set(0, j, prod);

			for (int i = 1; i < threeN - level; ++i) { //rows
				bool prod1 = Y1.get(j, level) && Y2.get(j, level + i);
				bool prod2 = Y1.get(j, level + i) && Y2.get(j, level);
				contrib.set(i, j, prod1 ^ prod2);
			}
		}
		return contrib;
	}
};

#endif