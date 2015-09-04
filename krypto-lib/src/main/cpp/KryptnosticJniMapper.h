//
//  Created by Drew Bailey
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//

#include "KryptnosticServer.h"
#include "SearchPrivateKey.h"
#include <jni.h>
#include "com_kryptnostic_krypto_engine_KryptnosticEngine.h"

#ifndef krypto_KryptnosticJniMapper_h
#define krypto_KryptnosticJniMapper_h

#define N 128

/* JByte Array to C++ Object */

template <typename T>
T * convertJByteArrayToCppObject( JNIEnv * env, jbyteArray convertMe ) {
	jbyte* buffPtr = env->GetByteArrayElements( convertMe, NULL );
	return reinterpret_cast<T *>( buffPtr );
}

/* C++ Object to JByte Array */

template<typename T>
jbyteArray convertCppObjectToJByteArray( JNIEnv * env, T * object ) {
	jbyteArray result = env->NewByteArray( sizeof(T) );
	env->SetByteArrayRegion( result, 0, sizeof(T), reinterpret_cast<const signed char *>( object ) );
	return result;
}

/* Other */

jfieldID getServerHandleField( JNIEnv *env, jobject javaContainer ) {
	jclass c = env->GetObjectClass( javaContainer );
	jfieldID field = env->GetFieldID( c, "handle", "J" );
	return field;
}

template <typename T>
T *getKryptnosticServer( JNIEnv *env, jobject javaContainer ) {
	jlong handle = env->GetLongField( javaContainer, getServerHandleField( env, javaContainer ) );
	return reinterpret_cast<T *>( handle );
}

template <typename T>
void setKryptnosticServer( JNIEnv *env, jobject javaContainer, T *t) {
	jlong serverAddress = reinterpret_cast<jlong>( t );
	env->SetLongField( javaContainer, getServerHandleField( env, javaContainer ), serverAddress );
}

/* JNI Functions */

void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_initKryptnosticService( JNIEnv * environment, jobject javaContainer, jbyteArray hash, jbyteArray encSearchToken ) {
	BitVector<2*N> * eSearchToken = convertJByteArrayToCppObject< BitVector<2*N> >( environment, encSearchToken );
	ClientHashFunction<N> * clientHashFunction = convertJByteArrayToCppObject< ClientHashFunction<N> >( environment, hash );
	KryptnosticServer<N> serv = KryptnosticServer<N>( *clientHashFunction, *eSearchToken );
	// set the long in java
	setKryptnosticServer( environment, javaContainer, &serv );
}

jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_calculateMetadataAddress( JNIEnv * environment, jobject javaContainer, jbyteArray searchKey, jbyteArray conversionMatrix ) {
	BitMatrix<N, N> * objectConvMatrix = convertJByteArrayToCppObject< BitMatrix<N, N> >( environment, conversionMatrix );
	BitVector<2*N> * encObjectSearchKey = convertJByteArrayToCppObject< BitVector<2*N> >( environment, searchKey );
	KryptnosticServer<N> serv = *getKryptnosticServer<KryptnosticServer<N>>( environment, javaContainer );

	pair<BitVector<2*N>, BitMatrix<N, N>> searchPair = std::make_pair( *encObjectSearchKey, *objectConvMatrix );
	BitVector<N> metadataAddress = serv.getMetadataAddress( searchPair );
	jbyteArray finalRay = convertCppObjectToJByteArray< BitVector<N> >( environment, &metadataAddress );
	return finalRay;
}

/* Tests */

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testBitVectorConversion
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testBitVectorConversion(JNIEnv * env, jclass javaContainer, jbyteArray bytes) {
	BitVector<N> * vector = convertJByteArrayToCppObject< BitVector<N> >(env, bytes);
	jbyteArray newBytes = convertCppObjectToJByteArray< BitVector<N> >(env, vector);
	return newBytes;
}

 /*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testBitMatrixConversion
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testBitMatrixConversion(JNIEnv * env, jclass javaContainer, jbyteArray bytes) {
	BitMatrix<N, N> * matrix = convertJByteArrayToCppObject< BitMatrix<N, N> >(env, bytes);
	jbyteArray newBytes = convertCppObjectToJByteArray< BitMatrix<N, N> >(env, matrix);
	return newBytes;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testClientHashFunctionConversion
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testClientHashFunctionConversion(JNIEnv * env, jclass javaContainer, jbyteArray bytes) {
	ClientHashFunction<N> * chf = convertJByteArrayToCppObject< ClientHashFunction<N> >(env, bytes);
	jbyteArray newBytes = convertCppObjectToJByteArray< ClientHashFunction<N> >(env, chf);
	return newBytes;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testPrivateKey
 * Signature: ()[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testPrivateKey(JNIEnv * env, jclass javaContainer) {
	PrivateKey<N> pk;
	return convertCppObjectToJByteArray< PrivateKey<N> >(env, &pk);
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testSearchPrivateKey
 * Signature: ()[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testSearchPrivateKey (JNIEnv * env, jclass javaContainer) {
	SearchPrivateKey<N> spk;
	return convertCppObjectToJByteArray< SearchPrivateKey<N> >(env, &spk);
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testObjectSearchKey
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testObjectSearchKey(JNIEnv * env, jclass javaContainer, jbyteArray spk) {
	SearchPrivateKey<N> spkActual = *convertJByteArrayToCppObject< SearchPrivateKey<N> >(env, spk);
	BitVector<N> osk = spkActual.getObjectSearchKey();
	return convertCppObjectToJByteArray< BitVector<N> >(env, &osk);
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testObjectAddressMatrix
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testObjectAddressMatrix(JNIEnv * env, jclass javaContainer, jbyteArray spk) {
	SearchPrivateKey<N> spkActual = *convertJByteArrayToCppObject< SearchPrivateKey<N> >(env, spk);
	BitMatrix<N> oam = spkActual.getObjectAddressMatrix();
	return convertCppObjectToJByteArray< BitMatrix<N> >(env, &oam);
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testObjectConversionMatrix
 * Signature: ([B[B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testObjectConversionMatrix(JNIEnv * env, jclass javaContainer, jbyteArray spk, jbyteArray oam) {
	SearchPrivateKey<N> spkActual = *convertJByteArrayToCppObject< SearchPrivateKey<N> >(env, spk);
	BitMatrix<N> oamActual = *convertJByteArrayToCppObject< BitMatrix<N> >(env, oam);
	BitMatrix<N> ocm = spkActual.getObjectConversionMatrix(oamActual);
	return convertCppObjectToJByteArray< BitMatrix<N> >(env, &ocm);
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testClientMetadataAddress
 * Signature: ([B[B[B[B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testClientMetadataAddress(JNIEnv * env, jclass javaContainer, jbyteArray spk, jbyteArray oam, jbyteArray osk, jbyteArray token) {
	SearchPrivateKey<N> spkActual = *convertJByteArrayToCppObject< SearchPrivateKey<N> >(env, spk);
	BitMatrix<N> oamActual = *convertJByteArrayToCppObject< BitMatrix<N> >(env, oam);
	BitVector<N> oskActual = *convertJByteArrayToCppObject< BitVector<N> >(env, osk);
	BitVector<N> tokenActual = *convertJByteArrayToCppObject< BitVector<N> >(env, token);
	BitVector<N> address = spkActual.getMetadatumAddress(oamActual, oskActual, tokenActual);
	return convertCppObjectToJByteArray< BitVector<N> >(env, &address);
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testClientHashFunction
 * Signature: ([B[B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testClientHashFunction(JNIEnv * env, jclass javaContainer, jbyteArray spk, jbyteArray pk) {
	SearchPrivateKey<N> spkActual = *convertJByteArrayToCppObject< SearchPrivateKey<N> >(env, spk);
	PrivateKey<N> pkActual = *convertJByteArrayToCppObject< PrivateKey<N> >(env, pk);
	
	ClientHashFunction<N> * chf = new ClientHashFunction<N>;
	chf = spkActual.getClientHashFunction(pkActual); //takes forever here

	// ClientHashFunction<N> chf = spkActual.getClientHashFunction(pkActual);

	// SearchPrivateKey<N> spk2;
	// PrivateKey<N> pk2;
	// *chf = spk2.getClientHashFunction(pk2);
	return convertCppObjectToJByteArray< ClientHashFunction<N> >(env, chf);
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testEncryptionFHE
 * Signature: ([B[B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testEncryptionFHE(JNIEnv * env, jclass javaContainer, jbyteArray pk, jbyteArray v) {
	PrivateKey<N> pkActual = *convertJByteArrayToCppObject< PrivateKey<N> >(env, pk);
	BitVector<N> vActual = *convertJByteArrayToCppObject< BitVector<N> >(env, v);
	BitVector<2*N> eVActual = pkActual.encrypt(vActual);
	return convertCppObjectToJByteArray< BitVector<2*N> >(env, &eVActual);
}

#endif
