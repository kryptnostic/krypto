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

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    destroyServer
 * Signature: ()V
 */
void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_destroyServer( JNIEnv * env, jobject javaContainer ){
	KryptnosticServer<N> * serv = getKryptnosticServer<KryptnosticServer<N> >( env, javaContainer );
	delete serv;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    destroyClient
 * Signature: ()V
 */
void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_destroyClient( JNIEnv * env, jobject javaContainer ){
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );
	delete client;
}

void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_initKryptnosticService( JNIEnv * environment, jobject javaContainer, jbyteArray hash, jbyteArray encSearchToken ) {
	BitVector<2*N> * eSearchToken = convertJByteArrayToCppObject< BitVector<2*N> >( environment, encSearchToken );
	ClientHashFunction<N> * clientHashFunction = convertJByteArrayToCppObject< ClientHashFunction<N> >( environment, hash );
	KryptnosticServer<N> * serv = new KryptnosticServer<N>(*clientHashFunction, *eSearchToken);
	// set the long in java
	setKryptnosticServer( environment, javaContainer, serv );
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    calculateMetadataAddress
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_calculateMetadataAddress( JNIEnv * environment, jobject javaContainer, jbyteArray objIndexPair ) {
	std::pair<BitVector<2*N>, BitMatrix<N> > * objectSearchPair = convertJByteArrayToCppObject<std::pair<BitVector<2*N>, BitMatrix<N> > >( environment, objIndexPair );

	KryptnosticServer<N> * serv = getKryptnosticServer<KryptnosticServer<N> >( environment, javaContainer );
	BitVector<N> metadataAddress = serv->getMetadataAddress( *objectSearchPair );

	jbyteArray finalRay = convertCppObjectToJByteArray< BitVector<N> >( environment, &metadataAddress );
	return finalRay;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    initClient
 * Signature: ()V
 */
void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_initClient__( JNIEnv * env, jobject javaContainer ) {
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
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );
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
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );
	PrivateKey<N> pkey = client->getPrivateKey();
	jbyteArray finalRay = convertCppObjectToJByteArray< PrivateKey<N> >( env, &pkey );
	return finalRay;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getClientHashFunction
 * Signature: ()[B
 */
 jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getClientHashFunction( JNIEnv * env, jobject javaContainer){
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );
	ClientHashFunction<N> chf = client->getClientHashFunction();
	jbyteArray finalRay = convertCppObjectToJByteArray< ClientHashFunction<N> >( env, &chf );
	return finalRay;
 }

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getObjectIndexPair
 * Signature: ()[B
 */
 jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getObjectIndexPair( JNIEnv * env, jobject javaContainer){
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );
	std::pair<BitVector<N>, BitMatrix<N> > objectIndexPair = client->getObjectIndexPair();
	jbyteArray finalRay = convertCppObjectToJByteArray< std::pair<BitVector<N>, BitMatrix<N> > >( env, &objectIndexPair );
	return finalRay;
 }

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getObjectSearchPairFromObjectIndexPair
 * Signature: ([B)[B
 */

 jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getObjectSearchPairFromObjectIndexPair( JNIEnv * env, jobject javaContainer, jbyteArray objectIndexPair){
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );

	std::pair<BitVector<N>, BitMatrix<N> > *objectIndexPairActual = convertJByteArrayToCppObject<std::pair<BitVector<N>, BitMatrix<N> > >( env, objectIndexPair );

	std::pair<BitVector<2*N>, BitMatrix<N> > objectSearchPair = client->getObjectSearchPairFromObjectIndexPair( *objectIndexPairActual );
	jbyteArray finalRay = convertCppObjectToJByteArray< std::pair<BitVector<2*N>, BitMatrix<N> > >( env, &objectSearchPair );
	return finalRay;
 }

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getObjectSharePairFromObjectSearchPair
 * Signature: ([B)[B
 */
 jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getObjectSharePairFromObjectSearchPair( JNIEnv * env, jobject javaContainer, jbyteArray objectSearchPair ){
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );
	std::pair<BitVector<2*N>, BitMatrix<N> > *objectSearchPairActual = convertJByteArrayToCppObject<std::pair<BitVector<2*N>, BitMatrix<N> > >( env, objectSearchPair );

	std::pair<BitVector<N>, BitMatrix<N> > objectSharePair = client->getObjectSharePairFromObjectSearchPair( *objectSearchPairActual );
	jbyteArray finalRay = convertCppObjectToJByteArray< std::pair<BitVector<N>, BitMatrix<N> > >( env, &objectSharePair );
	return finalRay;
 }

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getObjectSearchPairFromObjectSharePair
 * Signature: ([B)[B
 */
 jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getObjectSearchPairFromObjectSharePair( JNIEnv * env, jobject javaContainer, jbyteArray objectSharePair ){
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );
	std::pair<BitVector<N>, BitMatrix<N> > *objectSharePairActual = convertJByteArrayToCppObject<std::pair<BitVector<N>, BitMatrix<N> > >( env, objectSharePair );

	std::pair<BitVector<2*N>, BitMatrix<N> > objectSearchPair = client->getObjectSearchPairFromObjectSharePair( *objectSharePairActual );
	jbyteArray finalRay = convertCppObjectToJByteArray< std::pair<BitVector<2*N>, BitMatrix<N> > >( env, &objectSearchPair );
	return finalRay;
 }

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    getEncryptedSearchToken
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_getEncryptedSearchToken( JNIEnv * env, jobject javaContainer, jbyteArray token ) {
	BitVector<N> * tokenActual = convertJByteArrayToCppObject< BitVector<N> >( env, token );
	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );

	BitVector<2*N> eToken = client->getEncryptedSearchToken( *tokenActual );
	jbyteArray finalRay = convertCppObjectToJByteArray< BitVector<2*N> >( env, &eToken );
	return finalRay;
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    clientGetMetadatumAddress
 * Signature: ([B[B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_clientGetMetadatumAddress( JNIEnv * env, jobject javaContainer, jbyteArray objectIndexPair, jbyteArray token ){
	std::pair<BitVector<N>, BitMatrix<N> > * objectIndexPairActual = convertJByteArrayToCppObject< std::pair<BitVector<N>, BitMatrix<N> > >(env, objectIndexPair);
	BitVector<N> * tokenActual = convertJByteArrayToCppObject< BitVector<N> >( env, token );

	KryptnosticClient<N> * client = getKryptnosticClient<KryptnosticClient<N> >( env, javaContainer );

	BitVector<N> address = client->getMetadataAddress( *objectIndexPairActual, *tokenActual );
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
 * Method:    testObjectIndexPair
 * Signature: ([B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testObjectIndexPair(JNIEnv * env, jclass javaContainer, jbyteArray spk) {
	SearchPrivateKey<N> spkActual = *convertJByteArrayToCppObject< SearchPrivateKey<N> >(env, spk);
	std::pair<BitVector<N>, BitMatrix<N> > oip = spkActual.getObjectIndexPair();
	return convertCppObjectToJByteArray< std::pair<BitVector<N>, BitMatrix<N> > >(env, &oip);
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
 * Method:    testObjectSearchPairFromIndexPair
 * Signature: ([B[B[B])[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testObjectSearchPairFromIndexPair(JNIEnv * env, jclass javaContainer, jbyteArray spk, jbyteArray oip, jbyteArray pk) {
	SearchPrivateKey<N> spkActual = *convertJByteArrayToCppObject< SearchPrivateKey<N> >(env, spk);
	PrivateKey<N> pkActual = *convertJByteArrayToCppObject< PrivateKey<N> >(env, pk);
	std::pair<BitVector<N>, BitMatrix<N> > oipActual = *convertJByteArrayToCppObject< std::pair<BitVector<N>, BitMatrix<N> > >(env, oip);
	std::pair<BitVector<2*N>, BitMatrix<N> > osp = spkActual.getObjectSearchPairFromObjectIndexPair(oipActual, pkActual);
	return convertCppObjectToJByteArray< std::pair<BitVector<2*N>, BitMatrix<N> > >(env, &osp);
}

/*
 * Class:     com_kryptnostic_krypto_engine_KryptnosticEngine
 * Method:    testClientMetadataAddress
 * Signature: ([B[B[B)[B
 */
jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_testClientMetadataAddress(JNIEnv * env, jclass javaContainer, jbyteArray spk, jbyteArray oip, jbyteArray token) {
	SearchPrivateKey<N> spkActual = *convertJByteArrayToCppObject< SearchPrivateKey<N> >(env, spk);
	std::pair<BitVector<N>, BitMatrix<N> > oipActual = *convertJByteArrayToCppObject< std::pair<BitVector<N>, BitMatrix<N> > >(env, oip);

	BitVector<N> tokenActual = *convertJByteArrayToCppObject< BitVector<N> >(env, token);
	BitVector<N> address = spkActual.getMetadataAddress(oipActual, tokenActual);
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
