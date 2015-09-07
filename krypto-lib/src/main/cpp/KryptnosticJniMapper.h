//
//  Created by Drew Bailey
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//

#include "KryptnosticServer.h"
#include "SearchPrivateKey.h"
#include "KryptnosticClient.h"
#include <jni.h>
#include "com_kryptnostic_krypto_engine_KryptnosticEngine.h"

#ifndef krypto_KryptnosticJniMapper_h
#define krypto_KryptnosticJniMapper_h

#define N 128

#define serverField "kryptnosticServerPointer"
#define clientField "kryptnosticClientPointer"
#define longType "J"

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
	jfieldID field = env->GetFieldID( c, serverField, longType );
	return field;
}

jfieldID getClientHandleField( JNIEnv *env, jobject javaContainer ) {
	jclass c = env->GetObjectClass( javaContainer );
	jfieldID field = env->GetFieldID( c, clientField, longType );

	return field;
}

template <typename T>
T *getKryptnosticServer( JNIEnv *env, jobject javaContainer ) {
	jlong handle = env->GetLongField( javaContainer, getServerHandleField( env, javaContainer ) );
	return reinterpret_cast<T *>( handle );
}

template <typename T>
T *getKryptnosticClient( JNIEnv *env, jobject javaContainer ) {
	jlong handle = env->GetLongField( javaContainer, getClientHandleField( env, javaContainer ) );
	return reinterpret_cast<T *>( handle );
}

template <typename T>
void setKryptnosticServer( JNIEnv *env, jobject javaContainer, T *t) {
	jlong serverAddress = reinterpret_cast<jlong>( t );
	env->SetLongField( javaContainer, getServerHandleField( env, javaContainer ), serverAddress );
}

template <typename T>
void setKryptnosticClient( JNIEnv *env, jobject javaContainer, T *t) {
	jlong clientAddress = reinterpret_cast<jlong>( t );
	env->SetLongField( javaContainer, getClientHandleField( env, javaContainer ), clientAddress );
}

/* JNI Functions */

void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_initKryptnosticService( JNIEnv * environment, jobject javaContainer, jbyteArray hash, jbyteArray encSearchToken ) {
	BitVector<2*N> * eSearchToken = convertJByteArrayToCppObject< BitVector<2*N> >( environment, encSearchToken );
	ClientHashFunction<N> * clientHashFunction = convertJByteArrayToCppObject< ClientHashFunction<N> >( environment, hash );
	KryptnosticServer<N> * serv = new KryptnosticServer<N>(*clientHashFunction, *eSearchToken);
	// set the long in java
	setKryptnosticServer( environment, javaContainer, serv );
}

jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_calculateMetadataAddress( JNIEnv * environment, jobject javaContainer, jbyteArray searchKey, jbyteArray conversionMatrix ) {
	BitMatrix<N, N> * objectConvMatrix = convertJByteArrayToCppObject< BitMatrix<N, N> >( environment, conversionMatrix );
	BitVector<2*N> * encObjectSearchKey = convertJByteArrayToCppObject< BitVector<2*N> >( environment, searchKey );
	KryptnosticServer<N> *serv = getKryptnosticServer<KryptnosticServer<N>>( environment, javaContainer );

	pair<BitVector<2*N>, BitMatrix<N, N>> searchPair = std::make_pair( *encObjectSearchKey, *objectConvMatrix );
	BitVector<N> metadataAddress = serv->getMetadataAddress( searchPair );

	jbyteArray finalRay = convertCppObjectToJByteArray< BitVector<N> >( environment, &metadataAddress );
	return finalRay;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    initClient
 * Signature: ()[B
 */
void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_initClient( JNIEnv * env, jobject javaContainer ) {
	KryptnosticClient<N> * client = new KryptnosticClient<N>();
	setKryptnosticClient( env, javaContainer, client );
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    initClient
 * Signature: ([B[B)[B
 */
void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_initClient___3B_3B( JNIEnv * env, jobject javaContainer, jbyteArray pKey, jbyteArray searchPKey ){
	PrivateKey<N> * pk = convertJByteArrayToCppObject< PrivateKey<N> >( env, pKey );
	SearchPrivateKey<N> * spk = convertJByteArrayToCppObject< SearchPrivateKey<N> >( env, searchPKey );

	KryptnosticClient<N> * client = new KryptnosticClient<N>( *pk, *spk );
	setKryptnosticClient( env, javaContainer, client );
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getSearchPrivateKey
 * Signature: ()[B
 */
jbyteArray  Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getSearchPrivateKey( JNIEnv * env, jobject javaContainer ){
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N>>( env, javaContainer );
	SearchPrivateKey<N> searchpkey = client->getSearchPrivateKey();
	jbyteArray finalRay = convertCppObjectToJByteArray< SearchPrivateKey<N> >( env, &searchpkey );
	return finalRay;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getPrivateKey
 * Signature: ()[B
 */
jbyteArray  Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getPrivateKey( JNIEnv * env, jobject javaContainer ){
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N>>( env, javaContainer );
	PrivateKey<N> pkey = client->getPrivateKey();
	jbyteArray finalRay = convertCppObjectToJByteArray< PrivateKey<N> >( env, &pkey );
	return finalRay;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getEncryptedSearchToken
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getEncryptedSearchToken( JNIEnv * env, jobject javaContainer, jbyteArray tok ) {
	BitVector<N> * token = convertJByteArrayToCppObject< BitVector<N> >( env, tok );
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N>>( env, javaContainer );

	BitVector<2*N> encryptedToken = client->getEncryptedSearchToken( *token );
	jbyteArray finalRay = convertCppObjectToJByteArray< BitVector<2*N> >( env, &encryptedToken );
	return finalRay;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    clientGetMetadatumAddress
 * Signature: ([B[B[B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_clientGetMetadatumAddress( JNIEnv * env, jobject javaContainer, jbyteArray objectAddressMatrix, jbyteArray objectSearchKey, jbyteArray tok ){
	BitMatrix<N> * objAddrMatrix = convertJByteArrayToCppObject< BitMatrix<N> >( env, objectAddressMatrix );
	BitVector<N> * objSearchKey = convertJByteArrayToCppObject< BitVector<N> >( env, objectSearchKey );
	BitVector<N> * token = convertJByteArrayToCppObject< BitVector<N> >( env, tok );

	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N>>( env, javaContainer );

	BitVector<N> address = client->getMetadatumAddress( *objAddrMatrix, *objSearchKey, *token );
	jbyteArray finalRay = convertCppObjectToJByteArray< BitVector<N> >( env, &address );
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

	ClientHashFunction<N> chf = spkActual.getClientHashFunction(pkActual);
	return convertCppObjectToJByteArray< ClientHashFunction<N> >(env, &chf);
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
