//
//  Created by Drew Bailey
//  Copyright (c) 2015 Kryptnostic. All rights reserved.
//

#include "KryptnosticServer.h"
#include <jni.h>
#include "com_kryptnostic_krypto_engine_KryptnosticEngine.h"

#ifndef krypto_KryptnosticJniMapper_h
#define krypto_KryptnosticJniMapper_h

#define N 128
#define N_bytes 16

const BitMatrix<N, N> * convertJByteArrayToBitMatrix( JNIEnv * env, jbyteArray convertMe ) {
	jbyte* buffPtr = env->GetByteArrayElements( convertMe, NULL );
	return reinterpret_cast<BitMatrix<N, N>*>( buffPtr );
}

const BitVector<2*N> * convertJByteArrayToBitVector( JNIEnv * env, jbyteArray convertMe ) {
	jbyte* buffPtr = env->GetByteArrayElements( convertMe, NULL );
	return reinterpret_cast<BitVector<2*N>*>( buffPtr );
}

const jbyteArray convertBitVectorToJByteArray( JNIEnv * env, BitVector<N> bitVect ) {
	jbyteArray result=env->NewByteArray( N_bytes );
	env->SetByteArrayRegion( result, 0, N_bytes, reinterpret_cast<const signed char *>( bitVect.elements() ) );
	return result;
}

const ClientHashFunction<N> * convertJByteArrayToClientHashFunction( JNIEnv * env, jbyteArray convertMe ) {
	jbyte* buffPtr = env->GetByteArrayElements( convertMe, NULL );
	return reinterpret_cast<ClientHashFunction<N>*>( buffPtr );
}

const jfieldID getServerHandleField( JNIEnv *env, jobject javaContainer ) {
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

void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_initKryptnosticService( JNIEnv * environment, jobject javaContainer, jbyteArray hash, jbyteArray encSearchToken ) {
	const BitVector<2*N> * eSearchToken = convertJByteArrayToBitVector( environment, encSearchToken );
	const ClientHashFunction<N> * clientHashFunction = convertJByteArrayToClientHashFunction( environment, encSearchToken );
	KryptnosticServer<N> serv = KryptnosticServer<N>( *clientHashFunction, *eSearchToken );
	// set the long in java
	setKryptnosticServer( environment, javaContainer, &serv );
}

jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_calculateMetadataAddress( JNIEnv * environment, jobject javaContainer, jbyteArray searchKey, jbyteArray conversionMatrix ) {
	const BitMatrix<N, N> * objectConvMatrix = convertJByteArrayToBitMatrix( environment, conversionMatrix );
	const BitVector<2*N> * encObjectSearchKey = convertJByteArrayToBitVector( environment, searchKey );
	KryptnosticServer<N> serv = *getKryptnosticServer<KryptnosticServer<N>>( environment, javaContainer );
	pair<BitVector<2*N>, BitMatrix<N, N>> searchPair = std::make_pair( *encObjectSearchKey, *objectConvMatrix );
	BitVector<N> metadataAddress = serv.getMetadataAddress( searchPair );
	jbyteArray finalRay = convertBitVectorToJByteArray( environment, metadataAddress );
	return finalRay;
}

#endif
