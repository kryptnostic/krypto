#include "KryptnosticServer.h"
#include <jni.h>
#include "com_kryptnostic_krypto_engine_KryptnosticEngine.h"

#ifndef krypto_KryptnosticJniMapper_h
#define krypto_KryptnosticJniMapper_h

#define N 128

const BitMatrix<N, N> * convertJByteArrayToBitMatrix( JNIEnv * env, jbyteArray convertMe ) {
	jbyte* buffPtr = env->GetByteArrayElements( convertMe, NULL );
	return reinterpret_cast<BitMatrix<N, N>*>( buffPtr );
}

const BitVector<2*N> * convertJByteArrayToBitVector( JNIEnv * env, jbyteArray convertMe ) {
	jbyte* buffPtr = env->GetByteArrayElements( convertMe, NULL );
	return reinterpret_cast<BitVector<2*N>*>( buffPtr );
}

const ClientHashFunction<N> * convertJByteArrayToClientHashFunction( JNIEnv * env, jbyteArray convertMe ) {
	jbyte* buffPtr = env->GetByteArrayElements( convertMe, NULL );
	return reinterpret_cast<ClientHashFunction<N>*>( buffPtr );
}

void destroy( JNIEnv * env, jbyte * killMe, jbyteArray original ) {
	env->ReleaseByteArrayElements( original, killMe, 0) ;
}

void Java_com_kryptnostic_krypto_engine_KryptnosticEngine_initKryptnosticService( JNIEnv * environ, jobject javaContainer, jbyteArray hash, jbyteArray encSearchToken ) {
	printf("Initing Kryptnostic Service calledddd \n");
	fflush(stdout);

	const BitVector<2*N> * eSearchToken = convertJByteArrayToBitVector( environ, encSearchToken );
	const ClientHashFunction<N> * clientHashFunction = convertJByteArrayToClientHashFunction( environ, encSearchToken );
	KryptnosticServer serv = KryptnosticServer::KryptnosticServer( *clientHashFunction, *eSearchToken );

	// set the long in java
	jclass c = environ->GetObjectClass( javaContainer );
	jfieldID field = environ->GetFieldID( c, "handle", "J" );
	jlong serverPtr = (jlong)&serv;
	environ->SetLongField( javaContainer, field, serverPtr );
	//return serverPtr;
}

jbyteArray Java_com_kryptnostic_krypto_engine_KryptnosticEngine_calculateMetadataAddress( JNIEnv * environ, jobject, jbyteArray searchKey, jbyteArray conversionMatrix ) {
	printf("Calculate Metadata address calledddd \n");
	const BitMatrix<N, N> * objectConvMatrix = convertJByteArrayToBitMatrix( environ, conversionMatrix );
	const BitVector<2*N> * encObjectSearchKey = convertJByteArrayToBitVector( environ, searchKey );
}

#endif
