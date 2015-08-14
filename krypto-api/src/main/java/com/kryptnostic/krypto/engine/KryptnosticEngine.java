package com.kryptnostic.krypto.engine;

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;

public class KryptnosticEngine {

    static {
        final String osName = System.getProperty( "os.name" ).toLowerCase();
        final String libraryName = "mainlib";
        String extension = "so";
        if ( !osName.contains( "linux" ) ){
            extension = "dylib";
        }
        final InputStream binaryAsStream = KryptnosticEngine.class.getResourceAsStream( libraryName + extension );
        final Path outputPath = Paths.get( "tmp", "kryptnostic", "fhe" + extension );
        try {
            Files.copy( binaryAsStream , outputPath , StandardCopyOption.REPLACE_EXISTING );
        } catch ( IOException e1 ) {
            System.out.println( "Failed to copy file from resource storage to " + outputPath );
            System.out.println( e1.getStackTrace() );
        }
        System.load( outputPath.toAbsolutePath().toString() );
    }

    private final long handle;

    public KryptnosticEngine( String serverGlobalHash ) {
        handle = KryptnosticEngineNative( serverGlobalHash );
    }

    public KryptnosticEngine( String privateKey, String publicKey, String serverGlobalHash ) {
        handle = KryptnosticEngineNative( privateKey, publicKey, serverGlobalHash );
    }

    //
    //    Constructors
    //
    private native long KryptnosticEngineNative( String serverGlobalHash );
    private native long KryptnosticEngineNative( String privateKey, String publicKey, String serverGlobalHash );

    //
    //    Keys
    //
    public native String getPrivateKeys();
    public native String getPublicKeys();
    public native String getServerSearchFunction();
    public native String getXor();
    public native String getAnd();
    public native String getShift();

    //
    //    Transformers
    //
    public native void mapTokenToKey( String word, String objectID );
    public native void getEncryptedSearchTerm( String word );
    public native void getBridgeKey( String docKey );

    //
    //    Generators
    //
    public native void genDocKey( String objectId );

    //
    //    Setters
    //
    public native void setDocKey( String objectId, String docKey );

}